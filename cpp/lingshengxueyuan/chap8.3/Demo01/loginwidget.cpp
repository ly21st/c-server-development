#include "loginwidget.h"
#include "ui_loginwidget.h"
#include "qdebug.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

#include "common.h"
#include "httputil.h"

#define SERVER_ADDRESS     "42.194.195.184"
#define SERVER_PORT        80


LoginWidget::LoginWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_pushButton_clicked()
{
    QString user = ui->txt_username->text();
    QString pwd = ui->txt_password->text();

    Common *common = new Common();

    //请求数据
    QJsonObject obj;
    obj.insert("user", user);
    obj.insert("pwd", common->getMd5(pwd));

    QJsonDocument doc(obj);
    // 将json对象转换为 字符串
    QByteArray array = doc.toJson();

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    // 准备工作 - 初始化一个QNetworkRequest
    QNetworkRequest res;

    // 设置登录的url
    QNetworkRequest request;
    QString url = QString("http://%1:%2/login").arg(SERVER_ADDRESS).arg(SERVER_PORT);
    request.setUrl(QUrl(url));
    // 请求头信息
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QVariant(array.size()));
    res.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.116 Safari/537.36");

    // 向服务器发送post请求
    QNetworkReply* reply = manager->post(request, array);
    qDebug() << "post url:" << url << "post data: " << array;

    // 读服务器回写的数据
    connect(reply, &QNetworkReply::readyRead, this, [=]()
    {
        // 读数据
        QVariant str = reply->header(QNetworkRequest::LocationHeader);
        // 数据 - 不包括头
        QByteArray data = reply->readAll();
        qDebug() << data;
        qDebug() << str.toString();
    });

    delete common;
}
