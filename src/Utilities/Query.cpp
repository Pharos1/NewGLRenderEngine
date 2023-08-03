#include "../pch.h"
#include "Query.hpp"

Query::Query(int type) {
	loadQuery(type);
}
Query::~Query() {
	mLog(std::string("~Query has been triggered! Hint: Query ID -> ") + std::to_string(this->id), Log::LogDestructorInfo);
	deleteQuery();
}

Query::Query(Query&& other) noexcept {
	std::swap(id, other.id);
	std::swap(type, other.type);
	std::swap(inUse, other.inUse);
	std::swap(resultReady, other.resultReady);
	std::swap(result, other.result);
}
Query& Query::operator==(Query&& other) noexcept {
	if (this == &other) return *this;

	deleteQuery();

	std::swap(id, other.id);
	std::swap(type, other.type);
	std::swap(inUse, other.inUse);
	std::swap(resultReady, other.resultReady);
	std::swap(result, other.result);
}

void Query::loadQuery(int type) {
	this->type = type;
	if (!id) glGenQueries(1, &id);
}
void Query::begin() {
	if (inUse) {
		mLog("Attempted to begin a query that is in use!", Log::LogWarning);
		return;
	}

	glBeginQuery(this->type, this->id);
	this->inUse = true;
}
void Query::end() {
	if (!inUse) {
		mLog("Attempted to end a query that isn't in use!", Log::LogWarning);
		return;
	}

	glEndQuery(this->type);
	this->inUse = false;

	//retrieveResult();
}
bool Query::resultAvailable() const {
	if (inUse) {
		mLog("Attempted to get availability of result for a query that is in use!", Log::LogWarning);
		return false;
	}

	int param;
	glGetQueryObjectiv(this->id, GL_QUERY_RESULT_AVAILABLE, &param);
	return param;
}
void Query::retrieveResult() {
	if (inUse) {
		mLog("Attempted to get result for a query that is in use!", Log::LogWarning);
		return;
	}
	//if (resultAvailable()) {
		glGetQueryObjectiv(this->id, GL_QUERY_RESULT, &this->result);
	//}
}
int Query::getResult() const {
	return this->result;
}
void Query::deleteQuery() {
	glDeleteQueries(1, &id);
	
	id = 0;
	type = 0;
	inUse = false;
	resultReady = false;
	result = 0;
}

class QueryCounter {
public:
	unsigned int id;
	bool resultReady = false;
	int result = 0;

	void loadQuery() {
		if (!id) glGenQueries(1, &id);

		//Activate and deactivate the query as glGenQueries activates it which gives errors
		glBeginQuery(GL_TIMESTAMP, this->id);
		glEndQuery(GL_TIMESTAMP);
	}
	QueryCounter(int type) {
		loadQuery();
	}
	QueryCounter() = default;
	~QueryCounter() {
		glDeleteQueries(1, &id);
	}

	void queryTime() {
		if (isResultReady())
			getResult();

		glQueryCounter(this->id, GL_TIMESTAMP);
	}
	bool isResultReady() {
		int param;

		glGetQueryObjectiv(this->id, GL_QUERY_RESULT_AVAILABLE, &param);
		return param;
	}
	void getResult() {
		glGetQueryObjectiv(this->id, GL_QUERY_RESULT, &this->result);
	}
};