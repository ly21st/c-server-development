/*
 * @Descripttion: C++自己实现反射案例
 *
 * 根据一个字符串(类的名称)，创建出一个对象
 * 实现思路
 * 1. 可以用一个map来存储类名-构造函数这样的键值对，每新增一个类，就在这个map“注册”一个键值对
 * 2. 因为这个map是一个全局都会用到的，所以可以使用单例模式
 * 3. 在进入main函数之前，这个map就应该生成好，即每一个类在实例化之前就执行了“注册”的代码
 *
 * @version: 1.0
 * @Author: Milo
 * @Date: 2020-06-05 15:14:40
 * @LastEditors: Milo
 * @LastEditTime: 2020-06-05 15:14:40
 */

#include <iostream>
#include <map>
#include <memory>

#define FUNC function<void*(void)>

using namespace std;

class Reflector
{
private:
    map<std::string, FUNC>objectMap;
    static shared_ptr<Reflector> ptr;

public:
    void* CreateObject(const string &str)
    {
        for (auto & x : objectMap)
        {
            if(x.first == str)
                return x.second();
        }
        return nullptr;
    }

    void Register(const string &class_name, FUNC && generator)
    {
        objectMap[class_name] = generator;
    }

    static shared_ptr<Reflector> Instance()
    {
        if(ptr == nullptr)
        {
            ptr.reset(new Reflector());
        }

        return ptr;
    }

};

shared_ptr<Reflector> Reflector::ptr = nullptr;

class RegisterAction
{
public:
    RegisterAction(const string &class_name, FUNC && generator)
    {
        Reflector::Instance()->Register(class_name, forward<FUNC>(generator));
    }
};

#define REGISTER(CLASS_NAME) \
RegisterAction g_register_action_##CLASS_NAME(#CLASS_NAME, []()\
{\
    return new CLASS_NAME(); \
});


class Base
{
public:
    explicit Base() = default;
    virtual void Print()
    {
        cout << "Base" << endl;
    }
};
REGISTER(Base);

class ObjectA : public Base
{
public:
    void Print() override
    {
        cout << "ObjectA" << endl;
    }
};
REGISTER(ObjectA);

class ObjectB : public Base
{
public:
    void Print() override
    {
        cout << "ObjectB" << endl;
    }
};
REGISTER(ObjectB);

int main()
{
    shared_ptr<Base> p1((Base*)Reflector::Instance()->CreateObject("Base"));
    p1->Print();

    shared_ptr<Base> p2((Base*)Reflector::Instance()->CreateObject("ObjectA"));
    p2->Print();

    shared_ptr<Base> p3((Base*)Reflector::Instance()->CreateObject("ObjectB"));
    p3->Print();
}
