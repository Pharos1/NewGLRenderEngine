#include "../pch.h"
#include "../Utilities/Logger.hpp"

class Query {
private:
	GLuint id = 0;
	GLuint type = 0;
	bool inUse = false;
	bool resultReady = false;
	int result = 0;

public:
	Query(int type);
	Query() = default;
	~Query();

	Query(const Query&) = delete;
	Query& operator=(const Query&) = delete;

	Query(Query&& other) noexcept;
	Query& operator==(Query&& other) noexcept;

	void loadQuery(int type);
	void begin();
	void end();
	bool resultAvailable() const;
	void retrieveResult();
	int getResult() const;
	void deleteQuery();
};