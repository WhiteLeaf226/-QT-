#include "creategroup.h"
#include "ui_creategroup.h"

CreateGroup::CreateGroup(QTcpSocket *socket, QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateGroup)
{
    ui->setupUi(this);
    this->tcpsocket = socket;
    this->userName = name;
}

CreateGroup::~CreateGroup()
{
    delete ui;
}

//点击取消安妮
void CreateGroup::on_cancelbut_clicked()
{
    this->close();
}

//点击确定按钮
void CreateGroup::on_createbut_clicked()
{
    QString groupName = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "create_group");
    obj.insert("user", this->userName);
    obj.insert("group", groupName);

    QByteArray ba = QJsonDocument(obj).toJson();
    this->tcpsocket->write(ba);
    this->close();
}

