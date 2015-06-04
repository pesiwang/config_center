#include "CCFileMonitor.h"
#include <stdexcept>
#include <iostream>
#include <errno.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>
using namespace std;

#define INOTIFY_EVENT_SIZE	(sizeof (struct inotify_event))
#define INOTIFY_BUFFER_LEN	(1024 * (INOTIFY_EVENT_SIZE + 16))
static int fd = 0;
static int fwd = 0;
static int dwd = 0;

CCFileMonitorDelegate::CCFileMonitorDelegate() {
}

CCFileMonitorDelegate::~CCFileMonitorDelegate() {
}

/////////////////////////////////////////////////

CCFileMonitor::CCFileMonitor(const string& pathname, CCFileMonitorDelegate *delegate) : _pathname(pathname), _delegate(delegate){
	char *buffer = strdup(this->_pathname.c_str());
	this->_filename = ::basename(buffer);
	this->_dirname = ::dirname(buffer);
	free(buffer);
}

CCFileMonitor::~CCFileMonitor() {
}

void CCFileMonitor::start() {
	this->setup();

	while(1) {
		char buffer[INOTIFY_BUFFER_LEN];
		int length = read(fd, buffer, INOTIFY_BUFFER_LEN);
		if (length > 0) {
			for (char *p = buffer; p < buffer + length; ) {
				struct inotify_event* event = (struct inotify_event *)p;
				if (event->wd == fwd) {
					this->handleFileEvent(event);
				}
				else {
					this->handleDirectoryEvent(event);
				}
				p += sizeof(struct inotify_event) + event->len;
			}
		}
	}
	this->cleanup();
}

void CCFileMonitor::setup() {
	fd = inotify_init();
	if (fd < 0) {
		throw runtime_error("inotify_init call failed");
	}

	fwd = inotify_add_watch(fd, this->_pathname.c_str(), IN_ALL_EVENTS);
	dwd = inotify_add_watch(fd, this->_dirname.c_str(), IN_ALL_EVENTS);
	if ((fwd < 0) || (dwd < 0)) {
		throw runtime_error("inotify_add_watch call failed");
	}
}

void CCFileMonitor::cleanup() {
	inotify_rm_watch(fd, fwd);
	inotify_rm_watch(fd, dwd);
	close(fd);
	fwd = 0;
	dwd = 0;
	fd = 0;
}

void CCFileMonitor::handleFileEvent(struct inotify_event* event) {
	if (event->mask & IN_CLOSE_WRITE) {
		if (this->_delegate != NULL) {
			this->_delegate->onFileChanged(this->_pathname);
		}
	}
}

void CCFileMonitor::handleDirectoryEvent(struct inotify_event* event) {
	if ((event->mask & IN_CREATE) && !(event->mask & IN_ISDIR)) {
		if (strcmp(event->name, this->_filename.c_str()) == 0) {
			cout<<"reinit it!"<<endl;
			inotify_rm_watch(fd, fwd);
			fwd = inotify_add_watch(fd, this->_pathname.c_str(), IN_ALL_EVENTS);
			if (fwd < 0) {
				throw runtime_error("inotify_add_watch call failed");
			}
		}
	}
}
