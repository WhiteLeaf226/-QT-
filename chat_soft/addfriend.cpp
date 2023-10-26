#include "addfriend.h"
#include "ui_addfriend.h"

Addfriend::Addfriend(QTcpSocket *socket, QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Addfriend)
{
    ui->setupUi(this);
    this->tcpsocket = socket;
    this->userName = name;
}

Addfriend::~Addfriend()
{
    delete ui;
}

//点击取消
void Addfriend::on_cancelbut_clicked()
{
    this->close();
}

//点击添加
void Addfriend::on_addbut_clicked()
{
    QString friendName = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "add");
    obj.insert("user", this->userName);
    obj.insert("friend", friendName);

    QByteArray ba = QJsonDocument(obj).toJson();
    this->tcpsocket->write(ba);

    this->close();
}


