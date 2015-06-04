#include "CCConfig.h"
using namespace std;

CCConfig* CCConfig::sharedInstance() {
	static CCConfig self;
	return &self;
}

void CCConfig::update(const string &hash, const string &content) {
	this->_hash = hash;
	this->_content = content;
}

const string& CCConfig::getHash() const {
	return this->_hash;
}

const string& CCConfig::getContent() const {
	return this->_content;
}

CCConfig::CCConfig() {
}

CCConfig::~CCConfig() {
}
