#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->tcpsocket = new QTcpSocket();
    this->tcpsocket->connectToHost(QHostAddress("8.130.110.193"), 10010);
    //连接成功，出现提示窗口
    connect(this->tcpsocket, &QTcpSocket::connected, this, &Widget::connect_success);

    connect(this->tcpsocket, &QTcpSocket::readyRead, this, &Widget::server_reply);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::connect_success()
{
    QMessageBox::information(this, "连接提示", "连接服务器成功！");
}

void Widget::on_registerbut_clicked()
{
    QString username = ui->userLineEdit->text();
    QString password = ui->passwordlineEdit->text();

    QJsonObject obj;
    obj.insert("cmd","register");
    obj.insert("user", username);
    obj.insert("password", password);

    QByteArray ba = QJsonDocument(obj).toJson();
    this->tcpsocket->write(ba);
}

void Widget::on_loginbut_clicked()
{
    QString username = ui->userLineEdit->text();
    QString password = ui->passwordlineEdit->text();

    this->userName = username;

    QJsonObject obj;
    obj.insert("cmd","login");
    obj.insert("user", username);
    obj.insert("password", password);

    QByteArray ba = QJsonDocument(obj).toJson();
    this->tcpsocket->write(ba);

}

void Widget::server_reply()
{
    QByteArray ba = this->tcpsocket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QString cmd = obj.value("cmd").toString();

    if(cmd == "register_reply")
        client_register_handler(obj.value("result").toString());
    else if(cmd == "login_replay")
    {
        client_login_handler(obj.value("result").toString(),
                            obj.value("friend").toString(),
                            obj.value("group").toString());

    }
}

void Widget::client_register_handler(QString res)
{
    if(res == "success")
        QMessageBox::information(this, "注册提示", "注册成功！");
    else if(res == "failure")
        QMessageBox::warning(this, "注册提示", "注册失败！");
}

void Widget::client_login_handler(QString res, QString fri, QString group)
{
    if(res == "user_not_exist")
        QMessageBox::warning(this, "登入提示", "用户不存在！");
    else if(res == "password_error")
        QMessageBox::warning(this, "登入提示", "密码错误！");
    else if(res == "success")
    {
        this->hide();
        this->tcpsocket->disconnect(SIGNAL(readyRead()));
        Chatlist *c = new Chatlist(this->tcpsocket, fri, group, this->userName);
        c->setWindowTitle(this->userName);
        c->show();
    }
}



