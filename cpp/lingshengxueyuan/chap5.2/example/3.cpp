/*
* 3.cpp
　*iostream模式，appender输出到控制台。
　*/
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/loglevel.h"
#include <log4cplus/loggingmacros.h>
#include <log4cplus/initializer.h>
#include <iomanip>

using namespace log4cplus;
using namespace std;


//  g++ -o 3 3.cpp -llog4cplus -std=c++11 
int main()
{
    /*step1:Instantiateanappenderobject*/
    SharedAppenderPtr _append(new  ConsoleAppender());
    _append->setName(LOG4CPLUS_TEXT2("appendtest"));
    
    /*step4:Instantiatealoggerobject*/
    Logger _logger = Logger::getInstance(LOG4CPLUS_TEXT2("test"));
    
    /*step5:Attachtheappenderobjecttothelogger*/
    _logger.addAppender(_append);
    
    /*logactivity*/
    LOG4CPLUS_TRACE(_logger, "Thisis" << "justat" << "est." <<  std::endl);
    LOG4CPLUS_DEBUG(_logger, "Thisisabool:" << true);
    LOG4CPLUS_INFO(_logger, "Thisisachar:" << 'x');
    LOG4CPLUS_WARN(_logger, "Thisisaint:" << 1000);
    LOG4CPLUS_ERROR(_logger, "Thisisalong(hex):" << std::hex  << 100000000);
    LOG4CPLUS_FATAL(_logger, "Thisisadouble:" <<  std::setprecision(15) << 1.2345234234);
    
    return 0;
}