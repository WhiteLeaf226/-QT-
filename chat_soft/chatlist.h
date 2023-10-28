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
    //处理服务器回复
    void server_reply();

    void on_addFriendbut_clicked();

    void on_createGroupbut_clicked();

    void on_addGroupbut_clicked();

    void on_friendList_double_clicked();

    void on_groupList_double_clicked();


signals:
    //发送显示聊天窗口信号
    void signal_to_sub_widget(QJsonObject);
    //发送显示群成员信号
    void signal_to_sub_widget_member(QJsonObject);
    //发送显示群聊信息信号
    void signal_to_sub_widget_group(QJsonObject);
    //启动工作对象
    void recv_file_working();

private:
    //收到好友登入回复
    void client_login_reply(QString);
    //收到添加好友回复
    void client_add_friend_reply(QJsonObject &);
    //收到被添加好友回复
    void client_be_add_friend_reply(QString);
    //收到创建群回复
    void client_create_group_reply(QJsonObject &);
    //收到添加群回复
    void client_add_group_reply(QJsonObject &);
    //收到发送消息回复
    void client_private_chat_reply(QString);
    //收到聊天消息
    void client_chat_reply(QJsonObject &);
    //收到获取群成员回复
    void client_get_group_member_reply(QJsonObject &);
    //收到群聊消息
    void client_group_chat_reply(QJsonObject &);
    //收到发送文件请求异常回复
    void client_send_file_reply(QString);
    //收到回复:可以发送文件
    void client_send_file_port_reply(QJsonObject &);
    //收到回复:接收文件
    void client_recv_file_port_reply(QJsonObject &);
    //收到好友下线回复
    void client_friend_offline(QString);

private:
    Ui::Chatlist *ui;
    QTcpSocket *tcpsocket;
    QString userName;
    QList<ChatWidgetInfo> chatWidgetList;
    QList<groupWidgetInfo> groupWidgetList;
};

#endif // CHATLIST_H
