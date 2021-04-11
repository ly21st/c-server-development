#include "common.h"

#include <QRegexp>
#include <QCryptographicHash>
#include <QDebug>
#include <QByteArray>
#include <QMessageBox>

#define USER_REG        "^[a-zA-Z\\d_@#-\*]\{3,16\}$"
#define PASSWD_REG      "^[a-zA-Z\\d_@#-\*]\{6,18\}$"

Common::Common(QObject *parent) : QObject(parent)
{

}

QString Common::getMd5(QString str) {
    QByteArray array;
    //md5加密
    array = QCryptographicHash::hash (str.toLocal8Bit(), QCryptographicHash::Md5);
    return array.toHex();
}

int Common::regexp(QString user, QString pwd) {
    // 数据校验
    QRegExp regexp(USER_REG);
    if (!regexp.exactMatch(user))
    {
        return -1;
    }
    regexp.setPattern(PASSWD_REG);
    if (!regexp.exactMatch(pwd))
    {
        return -2;
    }
    return 0;
}

QString Common::encode(QString str) {
    QByteArray base = str.toLocal8Bit();
    return base.toBase64();
}

QString Common::decode(QString str) {
    QByteArray base = str.toLocal8Bit();
    QByteArray::fromBase64(base);
    return base.toBase64();
}
