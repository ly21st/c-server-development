// 文件名2.cpp
/*
　*简洁使用，仅实现步骤1、4、5。
 　*/
#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
 
using namespace log4cplus;
using namespace log4cplus::helpers;
//  g++ -o 2 2.cpp -llog4cplus -std=c++11
int main()
{
    /* step 1: Instantiate an appender object */
    SharedAppenderPtr _append(new ConsoleAppender());
    _append->setName(LOG4CPLUS_TEXT2("append  test"));
    
    /* step 4: Instantiate a logger object */
    Logger _logger = Logger::getInstance(LOG4CPLUS_TEXT2("test"));
    
    /* step 5: Attach the appender object to the  logger  */
    _logger.addAppender(_append);
    
    /* log activity */
    _logger.log(log4cplus::DEBUG_LOG_LEVEL,  "This is the  FIRST log message...");

    _logger.log(log4cplus::WARN_LOG_LEVEL,  "This is the  SECOND log message...");
    
    return 0;
}