// 8.cpp
#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/ndc.h>
#include <log4cplus/mdc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/thread/threads.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/initializer.h>
#include <iostream>
#include <string>

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;
// g++ -o 8 8.cpp -llog4cplus --std=c++11 
int main()
{
    log4cplus::Initializer initializer;
    SharedObjectPtr<Appender> append(new ConsoleAppender());
    append->setName(LOG4CPLUS_TEXT("console1"));
    append->setLayout( std::unique_ptr<Layout>(new TTCCLayout(true)) );

  
    log4cplus::helpers::Properties props;
    props.setProperty(LOG4CPLUS_TEXT("DateFormat"), LOG4CPLUS_TEXT("%Y-%m-%d %H:%M:%S,%Q"));
    props.setProperty(LOG4CPLUS_TEXT("ThreadPrinting"), LOG4CPLUS_TEXT("false"));

    SharedObjectPtr<Appender> append2(new ConsoleAppender());
    append2->setName(LOG4CPLUS_TEXT("console2"));
    append2->setLayout(std::unique_ptr<Layout>(new TTCCLayout(props))); 


    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("darren.logger"));
    logger.addAppender(append); // 使用了2个appender输出，不同的appender输出格式不一样
    logger.addAppender(append2);

    LOG4CPLUS_DEBUG(logger, "This is the FIRST log message...");
    LOG4CPLUS_INFO(logger, "This is the SECOND log message...");
    LOG4CPLUS_WARN(logger, "This is the THIRD log message...");
    LOG4CPLUS_ERROR(logger, "This is the FOURTH log message...");
    LOG4CPLUS_FATAL(logger, "This is the FIFTH log message...");
    
    return 0;
}
