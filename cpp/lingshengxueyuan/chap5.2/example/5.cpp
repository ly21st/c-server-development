/*
5.cpp
通过loglog来控制输出调试、警告或错误信息，appender输出到屏幕。
*/
#include <iostream>
#include <log4cplus/helpers/loglog.h>
using namespace log4cplus::helpers;
 
void printMsgs(void)
{
    std::cout << "Entering  printMsgs()..." << std::endl;
    LogLog::getLogLog()->debug("This is a  Debug statement...");
    LogLog::getLogLog()->warn("This is a  Warning...");
    LogLog::getLogLog()->error("This is a  Error...");
    std::cout << "Exiting  printMsgs()..." << std::endl << std::endl;
}
//  g++ -o 5 5.cpp -llog4cplus -std=c++11  
int main()
{
    printMsgs();
    std::cout << "Turning on  debug..." << std::endl;
    LogLog::getLogLog()->setInternalDebugging(true);
    printMsgs();
    std::cout << "Turning on  quiet mode..." << std::endl;
    LogLog::getLogLog()->setQuietMode(true);
    printMsgs();

    return 0;
}