#include "CCOnline.h"
using namespace std;

CCOnline* CCOnline::sharedInstance() {
	static CCOnline self;
	return &self;
}

void CCOnline::add(int clientFd) {
	this->_clients.insert(clientFd);
}

void CCOnline::remove(int clientFd) {
	this->_clients.erase(clientFd);
}

const set<int>& CCOnline::getClients() const {
	return this->_clients;
}

CCOnline::CCOnline() {
}

CCOnline::~CCOnline() {
}
