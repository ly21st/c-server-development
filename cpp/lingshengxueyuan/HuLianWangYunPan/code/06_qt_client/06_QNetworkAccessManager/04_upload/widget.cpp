#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>

#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QNetworkReply>

#include <QDebug>
#define cout qDebug() << "[ " << __FILE__ << ":"  << __LINE__ << " ] " //没有分号

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    //一旦一个QNetworkAccessManager对象被创建了，那么应用程序就可以使用它在网络上发送请求。
    //它提供了一组标准的函数，可以承载网络请求和一些可选的数据，并且每一个请求返回一个QNetworkReply对象。
    //该返回的对象(QNetworkReply对象)包含着返回的请求应带的所有数据。
    manager = new QNetworkAccessManager(this); //新建 QNetworkAccessManager 对象

}

Widget::~Widget()
{
    delete ui;
}

//产生分隔线
QString Widget::getBoundary()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime())); //随机种子
    QString tmp;

    // 48~122, '0'~'A'~'z'
    for(int i = 0; i < 16; i++)
    {
        tmp[i] = qrand() % (122-48) + 48;
    }
    //cout << "tmp = " << tmp;

    return QString("------WebKitFormBoundary%1").arg(tmp);
}

//按钮槽函数
void Widget::on_pushButton_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "打开文件");

    if(path.isEmpty())
    {
        cout << "path.isEmpty()";
        return;
    }

    //获取文件属性信息
    QFileInfo info(path);
    //获取文件大小，返回bytes
    if(info.size() > 30*1024*1024) //最大文件只能是30M
    {
        cout << "file is to big\n";
        return;
    }

    //获取文件名
    QString fileName = info.fileName();
    cout << "fileName = " << fileName;

    QFile file(path); //创建文件对象

    if( !file.open(QIODevice::QIODevice::ReadOnly) )
    { //如果打开文件失败，则删除 file，并使 file 指针为 0，然后返回
        cout << "file open error";
        return;
    }

     /*
    ------WebKitFormBoundaryO=>em3xo``g3Pliy\r\n
    Content-Disposition: form-data; user="milo"; filename="xxx.jpg"; md5="xxxx"; size=10240\r\n
    Content-Type: application/octet-stream\r\n
    \r\n
    真正的文件内容\r\n
    ------WebKitFormBoundaryO=>em3xo``g3Pliy
    */

    QString boundary = getBoundary(); //产生分隔线

    QByteArray data;

    //第1行，分隔线
    data.append(boundary);
    data.append("\r\n");

    //上传文件信息
    data.append("Content-Disposition: form-data; ");
    data.append( QString("user=\"%1\" ").arg( "milo" ) ); //上传用户
    data.append( QString("filename=\"%1\" ").arg(fileName) ); //文件名字
    data.append( QString("md5=\"%1\" ").arg("aed81d0a06d2456970014bf3c3aa8e99") ); //文件md5码
    data.append( QString("size=%1").arg(info.size())  );   //文件大小
    data.append("\r\n");

    data.append("Content-Type: application/octet-stream");
    data.append("\r\n");
    data.append("\r\n");

    //上传文件内容
    data.append( file.readAll() ); //文件内容
    data.append("\r\n");

    //结束分隔线
    data.append(boundary);

    file.close(); //关闭文件


    QNetworkRequest request; //请求对象
    request.setUrl(QUrl( ui->lineEdit->text() )); //设置url

    //qt默认的请求头
    //request.setRawHeader("Content-Type","text/html");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    //发送post请求
    QNetworkReply * reply = manager->post( request, data );
    if(reply == NULL)
    {
        cout << "reply == NULL";
        return;
    }

    ui->progressBar->setValue(0); //进度条当前值设置为0

    //有可用数据更新时
    connect(reply, &QNetworkReply::uploadProgress,
        [=](qint64 bytesRead, qint64 totalBytes)
        {
            if(totalBytes != 0) //这个条件很重要
            {
                ui->progressBar->setMaximum(totalBytes/1024); //最大值
                ui->progressBar->setValue(bytesRead/1024);    //当前值
            }

        }
    );

    //获取请求的数据完成时，就会发送信号SIGNAL(finished())
    connect(reply, &QNetworkReply::finished,
        [=]() mutable
        {
            if (reply->error() != QNetworkReply::NoError) //有错误
            {
                cout << reply->errorString();
                reply->deleteLater(); //释放资源
                return;
            }


            reply->deleteLater();
            QByteArray array = reply->readAll();
            cout << "array = " << array.data();
        }
    );

}
