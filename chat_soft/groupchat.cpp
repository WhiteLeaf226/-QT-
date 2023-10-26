#include "groupchat.h"
#include "ui_groupchat.h"

GroupChat::GroupChat(QTcpSocket *socket, QString u_name, QString g_name, Chatlist *c, QList<groupWidgetInfo> *l, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupChat)
{
    ui->setupUi(this);
    this->tcpsocket = socket;
    this->userName = u_name;
    this->groupName = g_name;
    this->mainWidget = c;
    this->groupWidgetList = l;

    QJsonObject obj;
    obj.insert("cmd", "get_group_member");
    obj.insert("group", this->groupName);

    QByteArray ba = QJsonDocument(obj).toJson();
    this->tcpsocket->write(ba);

    connect(mainWidget, &Chatlist::signal_to_sub_widget_member, this, &GroupChat::show_group_member);

    connect(mainWidget, &Chatlist::signal_to_sub_widget_group, this, &GroupChat::show_group_text);
}

GroupChat::~GroupChat()
{
    delete ui;
}



//显示群成员
void GroupChat::show_group_member(QJsonObject obj)
{
    if(obj.value("cmd").toString() == "get_group_member_reply")
        if(obj.value("group").toString() == this->groupName)
        {
            QStringList strList = obj.value("member").toString().split("|");
            for(int i = 0; i < strList.size(); i++)
                ui->listWidget->addItem(strList.at(i));
        }

}

//点击发送按钮
void GroupChat::on_sendbut_clicked()
{
    QString text = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "group_chat");
    obj.insert("user", this->userName);
    obj.insert("group", this->groupName);
    obj.insert("text", text);

    QByteArray ba = QJsonDocument(obj).toJson();
    this->tcpsocket->write(ba);

    ui->lineEdit->clear();
    ui->textEdit->append(text);
    ui->textEdit->append("\n");
}

//将收到的消息显示在对话窗口
void GroupChat::show_group_text(QJsonObject obj)
{
    if(obj.value("cmd").toString() == "group_chat")
        if(obj.value("group").toString() == this->groupName)
        {
            if(this->isMinimized())
                this->showNormal();
            this->activateWindow();
            ui->textEdit->append(obj.value("text").toString());
            ui->textEdit->append("\n");
        }


}

void GroupChat::closeEvent(QCloseEvent *event)
{
    for(int i = 0; i < this->groupWidgetList->size(); ++i)
        if(this->groupWidgetList->at(i).name == this->groupName)
            groupWidgetList->removeAt(i);

    event->accept();
}
