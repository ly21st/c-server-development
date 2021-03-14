// 6.cpp
#include <log4cplus/logger.h>//这些头文件因为版本而异
#include <log4cplus/layout.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/ndc.h>
using namespace log4cplus;
using namespace helpers;
// g++ -o 6 6.cpp -llog4cplus -std=c++11
int main()
{
	//用Initializer进行初始化，初始化函数不同版本可能用法不一样
	log4cplus::initialize();

	//第1步：创建ConsoleAppender
	log4cplus::SharedAppenderPtr appender(new log4cplus::ConsoleAppender());

	//第2步：设置Appender的名称和输出格式（SimpleLayout）
	appender->setName(LOG4CPLUS_TEXT2("console"));
	appender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::SimpleLayout)); 

	//第3步：获得一个Logger实例，并设置其日志输出等级阈值
	log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT2("test"));
	logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);

	//第4步：为Logger实例添加ConsoleAppender
	logger.addAppender(appender);

	//第5步：使用宏将日志输出
	LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT2("This  is the simple formatted log message.."));

	return 0;
}