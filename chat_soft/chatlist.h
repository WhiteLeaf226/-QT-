#ifndef CHATLIST_H
#define CHATLIST_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument> //解析
#include <QMessageBox>
#include <QList>
#include <QCloseEvent>
#include <QThread>
#include <QProgressDialog>
#include "addfriend.h"
#include "creategroup.h"
#include "addgroup.h"
#include "sendthread.h"
#include "recvfile.h"

class GroupChat;
#include "groupchat.h"

class PrivateChat;
#include "privatechat.h"

namespace Ui {
class Chatlist;
}

struct ChatWidgetInfo
{
    PrivateChat *w;
    QString name;
};

struct groupWidgetInfo
{
    GroupChat *w;
    QString name;
};

class Chatlist : public QWidget
{
    Q_OBJECT

public:
    explicit Chatlist(QTcpSocket *, QString, QString,
                    QString, QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *);
    ~Chatlist();

private slots:
    void server_reply();

    void on_addFriendbut_clicked();

    void on_createGroupbut_clicked();

    void on_addGroupbut_clicked();

    void on_friendList_double_clicked();

    void on_groupList_double_clicked();


signals:
    void signal_to_sub_widget(QJsonObject);
    void signal_to_sub_widget_member(QJsonObject);
    void signal_to_sub_widget_group(QJsonObject);
    void recv_file_working();

private:
    void client_login_reply(QString);
    void client_add_friend_reply(QJsonObject &);
    void client_be_add_friend_reply(QString);
    void client_create_group_reply(QJsonObject &);
    void client_add_group_reply(QJsonObject &);
    void client_private_chat_reply(QString);
    void client_chat_reply(QJsonObject &);
    void client_get_group_member_reply(QJsonObject &);
    void client_group_chat_reply(QJsonObject &);
    void client_send_file_reply(QString);
    void client_send_file_port_reply(QJsonObject &);
    void client_recv_file_port_reply(QJsonObject &);
    void client_friend_offline(QString);

private:
    Ui::Chatlist *ui;
    QTcpSocket *tcpsocket;
    QString userName;
    QList<ChatWidgetInfo> chatWidgetList;
    QList<groupWidgetInfo> groupWidgetList;
};

#endif // CHATLIST_H
