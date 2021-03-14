/* 4.cpp
*文件模式，appender输出到文件。
　*/
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/ndc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/initializer.h>
#include <sys/time.h>
#include <iostream>

using namespace log4cplus;
using namespace std;
 
static uint64_t get_tick_count()
{
    struct timeval tval;
    uint64_t ret_tick;
    
    gettimeofday(&tval, NULL);
    
    ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
    return ret_tick;
}

//  g++ -o 4 4.cpp -llog4cplus -std=c++11 
int main()
{
    /*step1:Instantiateanappenderobject*/
    SharedAppenderPtr _append(new FileAppender(LOG4CPLUS_TEXT2("Test.log")));
    _append->setName(LOG4CPLUS_TEXT2("filelogtest"));
    
    /*step4:Instantiatealoggerobject*/
    Logger _logger = Logger::getInstance(LOG4CPLUS_TEXT2("test.subtestof_filelog"));
    
    /*step5:Attachtheappenderobjecttothelogger*/
    _logger.addAppender(_append);
    
    /*logactivity*/
    for (int i = 0; i < 5; ++i) {
        LOG4CPLUS_DEBUG(_logger, "Enteringloop#" << i  << "Endline#");
    }

    uint64_t begin_time = get_tick_count();
    std::cout << "begin_time: " << begin_time << std::endl;
    for (int i = 0; i < 100000; i++)
    {
        LOG4CPLUS_DEBUG(_logger, "NO." << i  << " Root Error Message!");
    }
    uint64_t end_time = get_tick_count();
    std::cout << "end_time: " << end_time << std::endl;
    std::cout << "need the time1: " << end_time << " " << begin_time << ", " << end_time - begin_time << "毫秒\n" ;
 
    return 0;
}