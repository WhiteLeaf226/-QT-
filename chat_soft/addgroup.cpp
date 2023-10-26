#include "addgroup.h"
#include "ui_addgroup.h"

AddGroup::AddGroup(QTcpSocket *socket, QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddGroup)
{
    ui->setupUi(this);
    this->tcpsocket = socket;
    this->userName = name;
}

AddGroup::~AddGroup()
{
    delete ui;
}

//点击取消按钮
void AddGroup::on_cancelbut_clicked()
{
    this->close();
}

//点击添加按钮
void AddGroup::on_addbut_clicked()
{
    QString groupName = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "add_group");
    obj.insert("user", this->userName);
    obj.insert("group", groupName);

    QByteArray ba = QJsonDocument(obj).toJson();
    this->tcpsocket->write(ba);
    this->close();
}

