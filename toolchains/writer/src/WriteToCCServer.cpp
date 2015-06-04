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
#include <openssl/md5.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <zlib.h>

using namespace std;

#pragma pack(1)
typedef struct _CC_MANAGER_PACKAGE_HEAD {
	unsigned char command;
	unsigned int length;
} CC_MANAGER_PACKAGE_HEAD;
#pragma pack()

#define CC_MANAGER_CMD_UPDATE   0x01

class CCServerManager : public CCFileMonitorDelegate {
public:
	CCServerManager(const string& host, int port);
	virtual ~CCServerManager();

	void onFileChanged(const string& pathname);

private:
	bool compress(const char* jsonFile, string& compressedDoc);

private:
	string _host;
	int _port;
};

CCServerManager::CCServerManager(const string& host, int port) : _host(host), _port(port) {
}

CCServerManager::~CCServerManager() {
}

void CCServerManager::onFileChanged(const string& pathname) {
	//step 1. compress the doc
	string compressedDoc = "";
	if (!this->compress(pathname.c_str(), compressedDoc)) {
		return;
	}

	//step 2. serialze to binary
	CCWriter writer;
	string hash = "";
	try {
		unsigned char szHash[MD5_DIGEST_LENGTH] = {0};
		MD5((const unsigned char*)(compressedDoc.data()), compressedDoc.size(), szHash);
		hash.assign((const char*)szHash, MD5_DIGEST_LENGTH);
	}
	catch (...) {
		cerr<<"failed to calculate hash of the json document"<<endl;
		return;
	}

	//step 3. write to shared memory
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		cerr<<"failed to create socket"<<endl;
		return;
	}

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_addr.s_addr = inet_addr(this->_host.c_str());
	sin.sin_family = AF_INET;
	sin.sin_port = htons(this->_port);
	int res = connect(fd, (struct sockaddr *)&sin, sizeof(sin));
	if (res != 0) {
		cerr<<"failed to connect to server"<<endl;
		::close(fd);
		return;
	}


	CC_MANAGER_PACKAGE_HEAD head;
	head.command = CC_MANAGER_CMD_UPDATE;
	head.length = MD5_DIGEST_LENGTH + compressedDoc.size();
	head.length = htonl(head.length);

	if ((::write(fd, &head, sizeof(head)) != sizeof(head)) 
		|| (::write(fd, hash.data(), hash.size()) != (int)hash.size())
			|| (::write(fd, compressedDoc.data(), compressedDoc.size()) != (int)compressedDoc.size())) {
		cerr<<"failed to send to server"<<endl;
		::close(fd);
		return;
	}

	::close(fd);
	cout<<"upload succ, len = "<<compressedDoc.size()<<endl;
}

bool CCServerManager::compress(const char* jsonFile, string& compressedDoc) {
	//step 1. load file data
	ifstream ifs(jsonFile);
	string jsonData;
	ifs.seekg(0, ios::end);   
	jsonData.reserve(ifs.tellg());
	ifs.seekg(0, ios::beg);
	jsonData.assign((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
	ifs.close();

	//step 2. compress data
	compressedDoc = "";
	unsigned char outChunk[0x4000];

	z_stream stream;
	stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
		return false;
	}

	stream.next_in = (unsigned char *)(jsonData.data());
	stream.avail_in = jsonData.size();
	do {
		stream.avail_out = 0x4000;
		stream.next_out = outChunk;
		if (deflate(&stream, Z_FINISH) == Z_STREAM_ERROR) {
			return false;
		}
		compressedDoc.append((const char*)outChunk, 0x4000 - stream.avail_out);
	} while (stream.avail_out == 0);
	deflateEnd(&stream);
	return true;
}

//////////////////////////////////////

int main(int argc, char *argv[]) {
	if (argc != 4) {
		cerr<<"usage: "<<argv[0]<<" <cc.json> <host> <port>"<<endl;
		return -1;
	}

	CCServerManager mgr(argv[2], atoi(argv[3]));
	mgr.onFileChanged(argv[1]);

	CCFileMonitor monitor(argv[1], &mgr);
	monitor.start();
	return 0;
}
