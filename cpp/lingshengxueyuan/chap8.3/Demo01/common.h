#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QObject>

class Common : public QObject
{
    Q_OBJECT
public:
    explicit Common(QObject *parent = nullptr);

    QString getMd5(QString str = "");

    int regexp(QString user, QString pwd);

    QString encode(QString str);

    QString decode(QString str);

signals:

public slots:
};

#endif // COMMON_H
