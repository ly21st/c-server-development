#include <iostream>

using namespace std;

class ExportFileProduct {
public:
    ExportFileProduct() {}
    virtual ~ExportFileProduct() {}

    virtual bool Export(string data) = 0; 
};

class ExportTextProduct : public ExportFileProduct {
public:
    ExportTextProduct() {}
    virtual ~ExportTextProduct() {}
    virtual bool Export(string data) {
        cout <<"导出数据:[" << data << "]保存成文本的方式" << endl;
        return true;
    }
};

class ExportDBProduct : public ExportFileProduct {
public:
    ExportDBProduct() {}
    virtual ~ExportDBProduct() {}
    virtual bool Export(string data) {
        cout <<"导出数据:[" << data << "]保存成数据库的方式" << endl;
        return true;
    }
};

class ExportFactory {
public:
    ExportFactory() {}
    virtual ~ExportFactory() {}
    virtual bool Export(int type, string data) {
        ExportFileProduct *product = nullptr;
        product = factoryMethod(type);
        bool ret = false;
        if (product) {
            ret = product->Export(data);
            return ret;
        } else {
            cout<<"没有对应的导出方式" << endl;
        }
        return ret;
    }
protected:
    virtual ExportFileProduct *factoryMethod(int type) {
        ExportFileProduct *product = nullptr;
        if (1 == type) {
            product = new ExportTextProduct();
        } else if ( 2== type) {
            product = new ExportDBProduct();
        } 
        return product;
    }
};

int main() {
    ExportFactory *exportFactory = new ExportFactory();
    exportFactory->Export(1, "上课人数");
    exportFactory->Export(2, "hello");
    exportFactory->Export(3, "通信录");
}