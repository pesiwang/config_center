#ifndef _CC_WRITER_H_
#define _CC_WRITER_H_

#include <string>
#include <json/json.h>
#include <stdint.h>

class CCWriter
{
public:
	CCWriter();
	virtual ~CCWriter();

	const std::string& write(const Json::Value& node);

private:
	uint32_t _write(const Json::Value& node);
	uint32_t _writeObject(const Json::Value& node);
	uint32_t _writeArray(const Json::Value& node);
	uint32_t _writeInteger(const Json::Value& node);
	uint32_t _writeBool(const Json::Value& node);
	uint32_t _writeString(const Json::Value& node);

private:
	std::string _data;
};
#endif
