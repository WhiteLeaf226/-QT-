#include "recvthread.h"

RecvThread::RecvThread(QJsonObject obj)
{
    QString pathName = obj.value("filename").toString();
    QStringList strList = pathName.split('/');
    this->fileName = strList.at(strList.size() - 1);
    this->fileLength = obj.value("length").toInt();
    this->total = 0;
    this->port = obj.value("port").toInt();
}

void RecvThread::run()
{
    this->file = new QFile(this->fileName);
    this->file->open(QIODeviceBase::WriteOnly);
    this->tcpsocket = new QTcpSocket;

    qDebug()<<"bbb";
    connect(this->tcpsocket, &QTcpSocket::readyRead, this, &RecvThread::recv_file, Qt::DirectConnection);

    this->tcpsocket->connectToHost(QHostAddress("8.130.110.193"), port);
    if(!this->tcpsocket->waitForConnected(10000))
    {
        qDebug()<<"eee";
        this->quit();
    }
    exec();
}

void RecvThread::recv_file()
{
    qDebug()<<"ccc";
    QByteArray ba = this->tcpsocket->readAll();
    this->total += ba.size();
    this->file->write(ba);

    if(this->total >= fileLength)
    {
        this->file->close();
        this->tcpsocket->close();
        delete this->file;
        delete this->tcpsocket;
        this->quit();
    }
}

