#include "CCWriter.h"
#include <arpa/inet.h>
#include <stdexcept>
using namespace std;

#define CC_APPEND_UINT32(str, value) do{uint32_t v = htonl(value); str.append((const char*)&v, sizeof(v));}while(0)
#define CC_APPEND_UINT8(str, value) do{str.append(1, (char)value);}while(0)

CCWriter::CCWriter() : _data("") {
}

CCWriter::~CCWriter() {
}

const string& CCWriter::write(const Json::Value& node) {
	uint32_t rootOffset = this->_write(node);

	string data = "";
	CC_APPEND_UINT32(data, rootOffset);
	this->_data = data + this->_data;
	return this->_data;
}

uint32_t CCWriter::_write(const Json::Value& node) {
	if (node.isObject()) {
		return this->_writeObject(node);
	}
	else if (node.isArray()) {
		return this->_writeArray(node);
	}
	else if (node.isInt()) {
		return this->_writeInteger(node);
	}
	else if (node.isBool()) {
		return this->_writeBool(node);
	}
	else if (node.isString()) {
		return this->_writeString(node);
	}
	else {
		throw runtime_error("unrecognized node type");
	}
}

uint32_t CCWriter::_writeObject(const Json::Value& node) {
	string data = "";
	data.append(1, 'O');

	CC_APPEND_UINT32(data, node.size());

	for (Json::ValueIterator iter = node.begin(); iter != node.end(); iter++) {
		string key = iter.key().asString();
		CC_APPEND_UINT32(data, key.size());
		data.append(key.data(), key.size());
		CC_APPEND_UINT32(data, this->_write(*iter));
	}

	this->_data += data;
	return this->_data.size() - data.size();
}

uint32_t CCWriter::_writeArray(const Json::Value& node) {
	string data = "";
	data.append(1, 'A');

	CC_APPEND_UINT32(data, node.size());
	for (Json::ValueIterator iter = node.begin(); iter != node.end(); iter++) {
		CC_APPEND_UINT32(data, this->_write(*iter));
	}

	this->_data += data;
	return this->_data.size() - data.size();
}

uint32_t CCWriter::_writeInteger(const Json::Value& node) {
	string data = "";
	data.append(1, 'I');

	CC_APPEND_UINT32(data, sizeof(uint32_t));
	CC_APPEND_UINT32(data, node.asInt());

	this->_data += data;
	return this->_data.size() - data.size();
}

uint32_t CCWriter::_writeBool(const Json::Value& node) {
	string data = "";
	data.append(1, 'B');
	
	CC_APPEND_UINT32(data, sizeof(uint8_t));
	CC_APPEND_UINT8(data, node.asBool() ? 1 : 0);

	this->_data += data;
	return this->_data.size() - data.size();
}

uint32_t CCWriter::_writeString(const Json::Value& node) {
	string data = "";
	data.append(1, 'S');

	CC_APPEND_UINT32(data, node.asString().size());
	data.append(node.asString().data(), node.asString().size());

	this->_data += data;
	return this->_data.size() - data.size();
}
