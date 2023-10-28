#ifndef RECVFILE_H
#define RECVFILE_H

#include <QObject>
#include <QFile>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

class RecvFile : public QObject
{
    Q_OBJECT
public:
    explicit RecvFile(QJsonObject, QObject *parent = nullptr);
    void working();

signals:
    void recv_done();
    void start_recv();
public slots:
    void recv_file();

private:
    QString fileName;
    int fileLength;
    int total;
    int port;
    QTcpSocket *tcpsocket;
    QFile *file;

    int flag;
};

#endif // RECVFILE_H
