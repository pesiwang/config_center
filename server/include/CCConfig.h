#ifndef _CC_CONFIG_H_
#define _CC_CONFIG_H_

#include <string>

class CCConfig {
public:
	static CCConfig *sharedInstance();

	void update(const std::string &hash, const std::string &content);
	const std::string& getHash() const;
	const std::string& getContent() const;

private:
	CCConfig();
	virtual ~CCConfig();

private:
	std::string _hash;
	std::string _content;
};

#endif
