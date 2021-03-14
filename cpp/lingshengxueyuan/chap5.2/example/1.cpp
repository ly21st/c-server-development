// 文件名 4-1.cpp
/*
*标准使用，严格实现步骤1-6。
　*/
#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/initializer.h>
#include <iostream>
#include <string>

// #include <log4cplus/ 
using namespace log4cplus;
using namespace log4cplus::helpers;
//  g++ -o 1 1.cpp -llog4cplus  -std=c++11
int main()
{
    /* step 1: Instantiate an appender object */
    SharedObjectPtr<Appender> _append (new ConsoleAppender());
    _append->setName("append for  test");

    /* step 2: Instantiate a layout object */
    std::string pattern = "%d{%m/%d/%y  %H:%M:%S}  - %m [%l]%n";
    // std::unique_ptr<log4cplus::Layout> _layout(new PatternLayout(pattern));
    
    /* step 3: Attach the layout object to the appender */
    _append->setLayout(std::unique_ptr<Layout>(new PatternLayout(pattern)));
    
    /* step 4:  Instantiate a logger object */
    Logger _logger = Logger::getInstance(LOG4CPLUS_TEXT2("test"));
    
    /* step 5: Attach the appender object to the  logger  */
    _logger.addAppender(_append);
    
    /* step 6: Set a priority for the logger  */
    _logger.setLogLevel(ALL_LOG_LEVEL);
    
    /* log activity */
    _logger.log(log4cplus::DEBUG_LOG_LEVEL,  "This is the  FIRST log message...");

    _logger.log(log4cplus::WARN_LOG_LEVEL,  "This is the  SECOND log message...");
    return 0;
}