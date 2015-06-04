#include "CCWriter.h"
#include "CCFileMonitor.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

class CCSharedMemoryManager : public CCFileMonitorDelegate {
public:
	CCSharedMemoryManager(int key);
	virtual ~CCSharedMemoryManager();

	void onFileChanged(const string& pathname);

private:
	bool parse(const char* jsonFile, Json::Value& root);

private:
	int _key;
};

CCSharedMemoryManager::CCSharedMemoryManager(int key) : _key(key) {
}

CCSharedMemoryManager::~CCSharedMemoryManager() {
}

void CCSharedMemoryManager::onFileChanged(const string& pathname) {
	//step 1. parse the doc
	Json::Value root;
	if (!this->parse(pathname.c_str(), root)) {
		cerr<<"failed to parse json file"<<endl;
		return;
	}

	//step 2. serialze to binary
	CCWriter writer;
	string data = "";
	try {
		data = writer.write(root);
	}
	catch (...) {
		cerr<<"failed to serialize json document"<<endl;
		return;
	}

	//step 3. write to shared memory
	int shmId = shmget(this->_key, data.size(), IPC_CREAT|0666);
	if (shmId < 0) {
		cerr<<"failed to create shared memory"<<endl;
		return;
	}

	void *buf = shmat(shmId, NULL, 0);
	if (buf == (void *)-1) {
		cerr<<"failed to attach to shared memory"<<endl;
		return;
	}

	memcpy(buf, data.data(), data.size());
	shmdt(buf);

	cout<<"upload succ, len = "<<data.size()<<endl;
}

bool CCSharedMemoryManager::parse(const char* jsonFile, Json::Value& root) {
	Json::Reader reader;
	ifstream ifs(jsonFile, ifstream::binary);
	return reader.parse(ifs, root, false);
}

//////////////////////////////////////

int main(int argc, char *argv[]) {
	if (argc != 3) {
		cerr<<"usage: "<<argv[0]<<" <cc.json> <hex_key>"<<endl;
		return -1;
	}

	unsigned int key = 0;
	std::stringstream ss;
	ss<<argv[2]<<hex;
	ss>>key;

	CCSharedMemoryManager mgr((int)key);
	mgr.onFileChanged(argv[1]);

	CCFileMonitor monitor(argv[1], &mgr);
	monitor.start();
	return 0;
}
