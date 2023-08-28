#version 450
out vec4 fragOut;

in vec2 texCoord;

layout(binding = 0) uniform sampler2D colorBuffer;

//FXAA options
uniform bool fxaaEnabled;
uniform vec2 inverseScreenSize;

uniform float EDGE_THRESHOLD_MIN;
uniform float EDGE_THRESHOLD_MAX;
uniform int   ITERATIONS;
uniform float SUBPIXEL_QUALITY;

float QUALITY[12] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0};

float fxaaLuma(vec3 color);
vec3 fxaa();

void main(){
	fragOut = fxaaEnabled ? vec4(fxaa(), 1.f) : texture(colorBuffer, texCoord);
};

float fxaaLuma(vec3 color) {
	//return color.g * (0.587/0.299) + color.r; //Note: This should be faster
	return sqrt(dot(color, vec3(0.299, 0.587, 0.114)));
}

/*Thanks to Simon Rodriguez for explaining FXAA in detail while also making it easy to comprehand(http://blog.simonrodriguez.fr/articles/2016/07/implementing_fxaa.html).*/
vec3 fxaa(){
	vec3 colorCenter = texture(colorBuffer, texCoord).rgb;

	float lumaCenter = fxaaLuma(colorCenter);
	float lumaD = fxaaLuma(textureOffset(colorBuffer, texCoord, ivec2( 0,-1)).rgb);
	float lumaU = fxaaLuma(textureOffset(colorBuffer, texCoord, ivec2( 0, 1)).rgb);
	float lumaL = fxaaLuma(textureOffset(colorBuffer, texCoord, ivec2(-1, 0)).rgb);
	float lumaR = fxaaLuma(textureOffset(colorBuffer, texCoord, ivec2( 1, 0)).rgb);
	
	float lumaMin = min(lumaCenter, min(min(lumaD, lumaU), min(lumaL,lumaR)));
	float lumaMax = max(lumaCenter, max(max(lumaD, lumaU), max(lumaL,lumaR)));
	
	float lumaRange = lumaMax - lumaMin;
	
	//If the luma variation is lower that a threshold don't perform any anti-aliasing.
	if(lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX))
		return colorCenter;

	float lumaDL = fxaaLuma(textureOffset(colorBuffer,texCoord,ivec2(-1,-1)).rgb);
	float lumaUR = fxaaLuma(textureOffset(colorBuffer,texCoord,ivec2( 1, 1)).rgb);
	float lumaUL = fxaaLuma(textureOffset(colorBuffer,texCoord,ivec2(-1, 1)).rgb);
	float lumaDR = fxaaLuma(textureOffset(colorBuffer,texCoord,ivec2( 1,-1)).rgb);
	
	float lumaDU = lumaD + lumaU;
	float lumaLR = lumaL + lumaR;
	
	float lumaLeftCorners  = lumaDL + lumaUL;
	float lumaDownCorners  = lumaDL + lumaDR;
	float lumaRightCorners = lumaDR + lumaUR;
	float lumaUpCorners    = lumaUR + lumaUL;
	
	//Estimation of the gradient along the horizontal and vertical axis.
	float edgeHorizontal =  abs(-2.f * lumaL + lumaLeftCorners) + abs(-2.f * lumaCenter + lumaDU) * 2.f + abs(-2.f * lumaR + lumaRightCorners);
	float edgeVertical   =  abs(-2.f * lumaU + lumaUpCorners)   + abs(-2.f * lumaCenter + lumaLR) * 2.f + abs(-2.f * lumaD + lumaDownCorners);
	
	bool isHorizontal = (edgeHorizontal >= edgeVertical);

	//Select the two neighboring texels lumas in the opposite direction to the local edge.
	float luma1 = isHorizontal ? lumaD : lumaL;
	float luma2 = isHorizontal ? lumaU : lumaR;

	//Compute gradients in this direction.
	float gradient1 = luma1 - lumaCenter;
	float gradient2 = luma2 - lumaCenter;
	
	bool is1Steepest = abs(gradient1) >= abs(gradient2);
	
	//Gradient in the corresponding direction
	float gradientScaled = .25f*max(abs(gradient1),abs(gradient2));

	float stepLength = isHorizontal ? inverseScreenSize.y : inverseScreenSize.x;
	
	float lumaLocalAverage = .0f;
	if(is1Steepest){
		//Switch the direction
		stepLength = -stepLength;
		lumaLocalAverage = .5f*(luma1 + lumaCenter);
	} 
	else
		lumaLocalAverage = .5f*(luma2 + lumaCenter);
	
	//Shift UV in the correct direction by half a pixel.
	vec2 currentUv = texCoord;
	if(isHorizontal)
		currentUv.y += stepLength * .5f;
	else
		currentUv.x += stepLength * .5f;

	//Compute offset (for each iteration step) in the right direction.
	vec2 offset = isHorizontal ? vec2(inverseScreenSize.x, 0.f) : vec2(0.f, inverseScreenSize.y);

	//Compute UVs to explore on each side of the edge, orthogonally. The QUALITY allows us to step faster.
	vec2 uv1 = currentUv - offset;
	vec2 uv2 = currentUv + offset;
	
	//Read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma.
	float lumaEnd1 = fxaaLuma(texture(colorBuffer,uv1).rgb);
	float lumaEnd2 = fxaaLuma(texture(colorBuffer,uv2).rgb);
	lumaEnd1 -= lumaLocalAverage;
	lumaEnd2 -= lumaLocalAverage;
	
	//If the luma deltas at the current extremities are larger than the local gradient, we have reached the side of the edge.
	bool reached1 = abs(lumaEnd1) >= gradientScaled;
	bool reached2 = abs(lumaEnd2) >= gradientScaled;
	bool reachedBoth = reached1 && reached2;
	
	//If the side is not reached, we continue to explore in this direction.
	if(!reached1)
		uv1 -= offset;
	if(!reached2)
		uv2 += offset; 
	if(!reachedBoth){
		for(int i = 2; i < ITERATIONS; i++){
			//If needed, read luma in 1st direction, compute delta.
			if(!reached1){
				lumaEnd1 = fxaaLuma(texture(colorBuffer, uv1).rgb);
				lumaEnd1 = lumaEnd1 - lumaLocalAverage;
			}
			//If needed, read luma in opposite direction, compute delta.
			if(!reached2){
				lumaEnd2 = fxaaLuma(texture(colorBuffer, uv2).rgb);
				lumaEnd2 = lumaEnd2 - lumaLocalAverage;
			}
			//If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
			reached1 = abs(lumaEnd1) >= gradientScaled;
			reached2 = abs(lumaEnd2) >= gradientScaled;
			reachedBoth = reached1 && reached2;
	
			//If the side is not reached, we continue to explore in this direction, with a variable quality.
			if(!reached1)
				uv1 -= offset * QUALITY[i];
			if(!reached2)
				uv2 += offset * QUALITY[i];
	
			//If both sides have been reached, stop the exploration.
			if(reachedBoth) break;
		}
	}
	
	//Compute the distances to each extremity of the edge.
	float distance1 = isHorizontal ? (texCoord.x - uv1.x) : (texCoord.y - uv1.y);
	float distance2 = isHorizontal ? (uv2.x - texCoord.x) : (uv2.y - texCoord.y);
	
	//In which direction is the extremity of the edge closer
	bool isDirection1 = distance1 < distance2;
	float distanceFinal = min(distance1, distance2);
	
	float edgeThickness = (distance1 + distance2);
	
	//UV offset: read in the direction of the closest side of the edge.
	float pixelOffset = - distanceFinal / edgeThickness + 0.5;

	//Is the luma at center smaller than the local average
	bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;
	
	//If the luma at center is smaller than at its neighbour, the delta luma at each end should be positive (same variation).
	//(in the direction of the closer side of the edge.)
	bool correctVariation = ((isDirection1 ? lumaEnd1 : lumaEnd2) < 0.0) != isLumaCenterSmaller;
	float finalOffset = correctVariation ? pixelOffset : 0.0;

	//Sub-pixel shifting
	//Full weighted average of the luma over the 3x3 neighborhood.
	float lumaAverage = (1.0/12.0) * (2.0 * (lumaDU + lumaLR) + lumaLeftCorners + lumaRightCorners);

	//Ratio of the delta between the global average and the center luma, over the luma range in the 3x3 neighborhood.
	float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter)/lumaRange,0.0,1.0);
	float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;

	//Compute a sub-pixel offset based on this delta.
	float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;
	
	//Pick the biggest of the two offsets.
	finalOffset = max(finalOffset, subPixelOffsetFinal);
	//Compute the final UV coordinates.
	vec2 finalUv = texCoord;
	if(isHorizontal)
		finalUv.y += finalOffset * stepLength;
	else
		finalUv.x += finalOffset * stepLength;
	
	vec3 finalColor = texture(colorBuffer,finalUv).rgb;
	return finalColor;
}