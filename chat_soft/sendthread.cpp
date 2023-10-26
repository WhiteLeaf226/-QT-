#include "sendthread.h"

SendThread::SendThread(QJsonObject obj)
{
    this->fileName = obj.value("filename").toString();
    this->fileLength = obj.value("length").toInt();
    this->port = obj.value("port").toInt();

    qDebug()<<fileName;
    qDebug()<<fileLength;
    qDebug()<<port;
}

void SendThread::run()
{
    QTcpSocket tcpsocket;
    tcpsocket.connectToHost(QHostAddress("8.130.110.193"), this->port);
    //等待套接字连接完成，最多10s
    if(!tcpsocket.waitForConnected(10000))
    {
        this->quit();
    }
    else
    {
        emit start_send();

        QFile file(this->fileName);
        file.open(QIODeviceBase::ReadOnly);

        while(1)
        {
            QByteArray ba = file.read(MAXSEND);
            if(ba.size() == 0)
            {
                emit end_send();
                break;
            }
            tcpsocket.write(ba);
            tcpsocket.flush();
            sleep(1);
        }
        if(tcpsocket.waitForDisconnected())
        {
            file.close();
            tcpsocket.close();
            this->quit();
        }
    }
}
