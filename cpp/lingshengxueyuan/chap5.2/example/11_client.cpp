// 11_client.cpp
#include <log4cplus/log4cplus.h>
#include <iomanip>
#include <iostream>
using namespace std;
using namespace log4cplus;

// 编译：g++ -o 11_client 11_client.cpp -llog4cplus -std=c++1
// 运行: ./11_client , 日志将输出到11_server
int main(int argc, char **argv)
{
    log4cplus::Initializer initializer;

    tstring serverName = (argc > 1
        ? LOG4CPLUS_C_STR_TO_TSTRING(argv[1]) : tstring());
    tstring host = LOG4CPLUS_TEXT("localhost");
    std::cout << "serverName: " << serverName << std::endl ;
    SharedAppenderPtr append_1(new SocketAppender(host, 9000, serverName)); // 主机ip，端口，server名
    append_1->setName( LOG4CPLUS_TEXT("First") );
    Logger::getRoot().addAppender(append_1);

    Logger root = Logger::getRoot();
    Logger test = Logger::getInstance( LOG4CPLUS_TEXT("socket.test") );

    LOG4CPLUS_DEBUG(root,
                    "This is"
                    << " a reall"
                    << "y long message." << endl
                    << "Just testing it out" << endl
                    << "What do you think?");
    test.setLogLevel(NOT_SET_LOG_LEVEL);
    LOG4CPLUS_DEBUG(test, "This is a bool: " << true);
    LOG4CPLUS_INFO(test, "This is a char: " << 'x');
    LOG4CPLUS_INFO(test, "This is a short: " << static_cast<short>(-100));
    LOG4CPLUS_INFO(test, "This is a unsigned short: "
        << static_cast<unsigned short>(100));
    std::this_thread::sleep_for (std::chrono::microseconds (500));
    LOG4CPLUS_INFO(test, "This is a int: " << 1000);
    LOG4CPLUS_INFO(test, "This is a unsigned int: " << 1000u);
    LOG4CPLUS_INFO(test, "This is a long(hex): " << hex << 100000000l);
    LOG4CPLUS_INFO(test, "This is a unsigned long: " << 100000000ul);
    LOG4CPLUS_WARN(test, "This is a float: " << 1.2345f);
    LOG4CPLUS_ERROR(test,
                    "This is a double: "
                    << setprecision(15)
                    << 1.2345234234);
    LOG4CPLUS_FATAL(test,
                    "This is a long double: "
                    << setprecision(15)
                    << 123452342342.342L);

    return 0;
}
