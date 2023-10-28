#include "recvfile.h"

RecvFile::RecvFile(QJsonObject obj, QObject *parent)
    : QObject{parent}
{
    QString pathName = obj.value("filename").toString();
    QStringList strList = pathName.split('/');
    this->fileName = strList.at(strList.size() - 1);
    this->fileLength = obj.value("length").toInt();
    this->total = 0;
    this->port = obj.value("port").toInt();

    this->flag = 0;
}

void RecvFile::working()
{
    this->file = new QFile(this->fileName);
    this->file->open(QIODeviceBase::WriteOnly);
    this->tcpsocket = new QTcpSocket;

    connect(this->tcpsocket, &QTcpSocket::readyRead, this, &RecvFile::recv_file);

    this->tcpsocket->connectToHost(QHostAddress("8.130.110.193"), port);

    //连接超时
    if(!this->tcpsocket->waitForConnected(10000))
    {
        emit recv_connect_timeout();
    }
    else if(!this->flag)  //连接成功
    {
        emit start_recv();
        this->flag = 1;
    }


    //exec();
}

void RecvFile::recv_file()
{
    QByteArray ba = this->tcpsocket->readAll();
    this->total += ba.size();
    this->file->write(ba);

    if(this->total >= fileLength)
    {
        this->file->close();
        this->tcpsocket->close();
        delete this->file;
        delete this->tcpsocket;

        //接收完成
        emit recv_done();
    }
}


