#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument> //解析
#include "Chatlist.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    //连接成功
    void connect_success();
    //接收到了消息
    void server_reply();

    void on_registerbut_clicked();

    void on_loginbut_clicked();

private:
    //注册回复
    void client_register_handler(QString);
    //登入回复
    void client_login_handler(QString, QString, QString);

private:
    Ui::Widget *ui;
    QTcpSocket *tcpsocket;
    QString userName;
};

#endif // WIDGET_H
