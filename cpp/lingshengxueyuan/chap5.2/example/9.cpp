// 9.cpp
#include <iostream>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/ndc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/initializer.h>
#include <sys/time.h>

using namespace log4cplus;
using namespace std;

const int LOOP_COUNT = 20000;
static uint64_t get_tick_count()
{
    struct timeval tval;
    uint64_t ret_tick;
    
    gettimeofday(&tval, NULL);
    
    ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
    return ret_tick;
}

int main()
{
    log4cplus::Initializer initializer;
    helpers::LogLog::getLogLog()->setInternalDebugging(true);    // 是否debug LogLog
    SharedFileAppenderPtr append_1(
        new RollingFileAppender(LOG4CPLUS_TEXT("9Test.log"), 200*1024, 5,
            false,      // 可以修改下立即刷新true和非立即刷新false对比速度 
            true));
    append_1->setName(LOG4CPLUS_TEXT("Darren老师Appender"));
    append_1->setLayout( std::unique_ptr<Layout>(new TTCCLayout()) );
    Logger::getRoot().addAppender(SharedAppenderPtr(append_1.get ()));

    Logger test = Logger::getInstance(LOG4CPLUS_TEXT("test"));
    uint64_t begin_time = get_tick_count();
    std::cout << "begin_time: " << begin_time << std::endl;
    for(int i=0; i<LOOP_COUNT; ++i) {
        NDCContextCreator _context(LOG4CPLUS_TEXT("上下文"));
        LOG4CPLUS_DEBUG(test, "Entering loop #" << i);
    }
    uint64_t end_time = get_tick_count();
    std::cout << "end_time: " << end_time << std::endl;
    std::cout << "need the time1: " << end_time << " " << begin_time << ", " << end_time - begin_time << "毫秒\n" ;
    return 0;
}
