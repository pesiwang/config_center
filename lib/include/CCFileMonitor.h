#ifndef _CC_FILE_MONITOR_
#define _CC_FILE_MONITOR_

#include <string>
#include <sys/inotify.h>

class CCFileMonitorDelegate
{
public:
	CCFileMonitorDelegate();
	virtual ~CCFileMonitorDelegate();

	virtual void onFileChanged(const std::string& pathname) = 0;
};

class CCFileMonitor
{
public:
	CCFileMonitor(const std::string& pathname, CCFileMonitorDelegate *delegate);
	virtual ~CCFileMonitor();

	void start();

private:
	void setup();
	void cleanup();
	void handleFileEvent(struct inotify_event* event);
	void handleDirectoryEvent(struct inotify_event* event);

private:
	std::string _pathname;
	std::string _filename;
	std::string _dirname;
	CCFileMonitorDelegate* _delegate;
};
#endif
