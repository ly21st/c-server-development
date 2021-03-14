/*
StringQueueAppender的功能是将日志记录到一个字符串队列中，该字符串队列使用了STL中的两个容器，
即字符串容器std::string和队列容器std::queue，具体如下：
std::queue<std::string> _queue;
_queue变量是StringQueueAppender类中用于具体存储日志的内存队列。StringQueueAppender的使用方法
与OstreamAppender类似，其创建函数只接收一个参数“名称”，记录完成后需要程序员自己从队列中取出每条日志
*/
#include <iostream>
#include <sys/time.h>
#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/StringQueueAppender.hh>

using namespace std;
static uint64_t get_tick_count()
{
    struct timeval tval;
    uint64_t ret_tick;
    
    gettimeofday(&tval, NULL);
    
    ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
    return ret_tick;
}
// g++ -o StringQueueAppender StringQueueAppender.cpp -llog4cpp -lpthread
int main(int argc, char* argv[])
{
     log4cpp::StringQueueAppender* strQAppender = new log4cpp::StringQueueAppender("strQAppender");
    strQAppender->setLayout(new log4cpp::BasicLayout());
    
    log4cpp::Category& root = log4cpp::Category::getRoot();
    root.addAppender(strQAppender);
    root.setPriority(log4cpp::Priority::DEBUG);
    
    root.error("Hello log4cpp in a Error Message!");
    root.warn("Hello log4cpp in a Warning Message!");
    
    cout<<"Get message from Memory Queue!"<<endl;
    cout<<"-------------------------------------------"<<endl;

    uint64_t begin_time = get_tick_count();
    std::cout << "begin_time: " << begin_time << std::endl;
    for (int i = 0; i < 100000; i++)
    {
        string strError;
        ostringstream oss;
        oss<<"NO." << i<<" Root Error Message!";        // 47字节 * 10000 = 470000=470k
        strError = oss.str();
        root.error(strError);
    }
    uint64_t end_time = get_tick_count();
    std::cout << "end_time: " << end_time << std::endl;
    std::cout << "need the time1: " << end_time << " " << begin_time << ", " << end_time - begin_time << "毫秒\n" ;

    queue<string>& myStrQ = strQAppender->getQueue();
    std::string bufString;
    int bufCount = 0;

     begin_time = get_tick_count();
    std::cout << "begin_time: " << begin_time << std::endl;
    FILE *file = fopen("StringQueueAppender.log", "wt");
    while(!myStrQ.empty())
    {
        // cout<<myStrQ.front();
        // std::cout << "append\n";
        bufString.append(myStrQ.front().c_str(), myStrQ.front().size());
        // std::cout << "append2\n";
        if(bufCount++ >= 100000) 
        {
            bufCount = 0;
            // std::cout << myStrQ.front() << std::endl;
            fwrite(bufString.c_str(), bufString.size(), 1, file);
            bufString.clear();
        }
        myStrQ.pop();
    }
    fwrite(bufString.c_str(), bufString.size(), 1, file);
    fclose(file);
    end_time = get_tick_count();
    std::cout << "need the time1: " << end_time << " " << begin_time << ", " << end_time - begin_time << "毫秒\n" ;
    log4cpp::Category::shutdown();    
    return 0;
}