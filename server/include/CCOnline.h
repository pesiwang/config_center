#ifndef _CC_ONLINE_H_
#define _CC_ONLINE_H_

#include <set>

class CCOnline {
public:
	static CCOnline *sharedInstance();

	void add(int clientFd);
	void remove(int clientFd);
	const std::set<int>& getClients() const;

private:
	CCOnline();
	virtual ~CCOnline();

private:
	std::set<int> _clients;
};

#endif
