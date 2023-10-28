#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QCloseEvent>
#include <QFileDialog>
#include <QCoreApplication>
#include <QFile>

class Chatlist;
struct ChatWidgetInfo;
#include "chatlist.h"

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QTcpSocket *, QString, QString, Chatlist *, QList<ChatWidgetInfo> *,  QWidget *parent = nullptr);
    ~PrivateChat();

    void closeEvent(QCloseEvent *);

private slots:
    void on_sendbut_clicked();

    //添加消息到消息框
    void show_text_slot(QJsonObject);

    void on_filebut_clicked();

private:
    Ui::PrivateChat *ui;
    QTcpSocket *tcpsocket;
    QString userName;
    QString friendName;
    Chatlist *mainWifget;
    QList<ChatWidgetInfo> *chatWidgetList;
};

#endif // PRIVATECHAT_H
