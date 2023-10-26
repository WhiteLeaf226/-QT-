#ifndef GROUPCHAT_H
#define GROUPCHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QList>
#include <QDebug>
#include <QCloseEvent>

class Chatlist;
struct groupWidgetInfo;
#include "chatlist.h"

namespace Ui {
class GroupChat;
}

class GroupChat : public QWidget
{
    Q_OBJECT

public:
    explicit GroupChat(QTcpSocket *, QString, QString, Chatlist *, QList<groupWidgetInfo> *, QWidget *parent = nullptr);
    ~GroupChat();
    void closeEvent(QCloseEvent *);

private slots:
    void show_group_member(QJsonObject);

    void on_sendbut_clicked();

    void show_group_text(QJsonObject);

private:
    Ui::GroupChat *ui;
    QTcpSocket *tcpsocket;
    QString userName;
    QString groupName;
    Chatlist *mainWidget;
    QList<groupWidgetInfo> *groupWidgetList;
};

#endif // GROUPCHAT_H
