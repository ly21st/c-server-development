/*
 * @Descripttion: 测试内存的xml保存到本地 
 * @version: 1.0
 * @Author: Milo
 * @Date: 2020-06-05 21:03:32
 * @LastEditors: Milo
 * @LastEditTime: 2020-06-05 21:03:32
 */
#include <iostream>
#include "tinyxml2.h"

int main(void)
{
    const char *xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>   \
                        <note>                      \
                            <to>beijing</to>             \
                            <from>shenzhen</from>           \
                            <heading>Reminder</heading> \
                            <body>Don't forget the meeting!</body> \
                        </note>";
    tinyxml2::XMLDocument doc;
    doc.Parse(xml);
    std::cout << doc.ErrorID() << std::endl;

    // 1. 第一种刷新到本地
    FILE *fp = fopen("memory_1.xml", "wb");
    tinyxml2::XMLPrinter printer(fp);
    doc.Print(&printer); // 打印到文件，则.CStr()就返回空值了

    std::cout << "xml:" << printer.CStr() << std::endl;
    fclose(fp);

    // 2. 第二种刷新到本地
    doc.SaveFile("memory_2.xml");

    return 0;
}
