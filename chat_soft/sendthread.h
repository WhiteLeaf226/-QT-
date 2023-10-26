#ifndef SENDTHREAD_H
#define SENDTHREAD_H

#include <QThread>
#include <QJsonObject>
#include <QJsonObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDebug>
#include <QProgressDialog>

#define MAXSEND 128 * 1024

class SendThread : public QThread
{
    Q_OBJECT
public:
    SendThread(QJsonObject);

    void run();

signals:
    void start_send();
    void end_send();

private:
    QString fileName;
    int fileLength;
    int port;
};

#endif // SENDTHREAD_H
