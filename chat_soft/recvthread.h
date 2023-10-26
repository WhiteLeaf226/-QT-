#ifndef RECVTHREAD_H
#define RECVTHREAD_H

#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDebug>

class RecvThread : public QThread
{
    Q_OBJECT
public:
    explicit RecvThread(QJsonObject);

    void run();

private slots:
    void recv_file();

private:
    QString fileName;
    int fileLength;
    int total;
    int port;
    QTcpSocket *tcpsocket;
    QFile *file;
};

#endif // RECVTHREAD_H
