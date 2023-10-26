#include "chatlist.h"
#include "ui_chatlist.h"

Chatlist::Chatlist(QTcpSocket *socket, QString fri, QString group,
                   QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chatlist)
{
    ui->setupUi(this);
    setStyleSheet("QTabBar::tab{height:25;width:150}");

    this->userName = name;
    this->tcpsocket = socket;

    connect(this->tcpsocket, &QTcpSocket::readyRead, this, &Chatlist::server_reply);

    //获取好友
    QStringList friList = fri.split('|');
    for(int i = 0; i < friList.size(); ++i)
        if(friList.at(i) != "")
            ui->friendList->addItem(friList.at(i));
    //获取群
    QStringList groList = group.split('|');
    for(int i = 0; i < groList.size(); ++i)
        if(groList.at(i) != "")
            ui->groupList->addItem(groList.at(i));

    connect(ui->friendList, &QListWidget::itemDoubleClicked, this, &on_friendList_double_clicked);
    connect(ui->groupList, &QListWidget::itemDoubleClicked, this, &Chatlist::on_groupList_double_clicked);

}

void Chatlist::closeEvent(QCloseEvent *event)
{
    QJsonObject obj;
    obj.insert("cmd", "offline");
    obj.insert("user", this->userName);
    QByteArray ba = QJsonDocument(obj).toJson();
    this->tcpsocket->write(ba);
    this->tcpsocket->flush();

    event->accept();
}

Chatlist::~Chatlist()
{
    delete ui;
}

//服务器回复
void Chatlist::server_reply()
{
    QByteArray ba = this->tcpsocket->readAll();
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    QString cmd = obj.value("cmd").toString();
    if(cmd == "friend_login")
        client_login_reply(obj.value("friend").toString());
    else if(cmd == "add_reply")  //添加好友
        client_add_friend_reply(obj);
    else if(cmd == "add_friend_reply")  //被添加好友
        client_be_add_friend_reply(obj.value("result").toString());
    else if(cmd == "create_group_reply")  //创建群
        client_create_group_reply(obj);
    else if(cmd == "add_group_reply")  //添加群
        client_add_group_reply(obj);
    else if(cmd == "private_chat_reply")  //对方不在线
        client_private_chat_reply(obj.value("result").toString());
    else if(cmd == "private_chat")  //收到聊天信息
        client_chat_reply(obj);
    else if(cmd == "get_group_member_reply")  //获取群成员
        client_get_group_member_reply(obj);
    else if(cmd == "group_chat")  //收到群消息
        client_group_chat_reply(obj);
    else if(cmd == "send_file_reply")  //发送文件异常回复
        client_send_file_reply(obj.value("result").toString());
    else if(cmd == "send_file_port_reply")  //可以发送文件
        client_send_file_port_reply(obj);
    else if(cmd == "recv_file_port_reply")  //收到接收文件回复
        client_recv_file_port_reply(obj);
    else if(cmd == "friend_offline")  //好友下线
        client_friend_offline(obj.value("friend").toString());
}

//登入提醒
void Chatlist::client_login_reply(QString fri)
{
    QString str = QString("%1好友上线").arg(fri);
                      QMessageBox::information(this, "好友上限提醒", str);
}

//点击添加好友按钮
void Chatlist::on_addFriendbut_clicked()
{
    Addfriend *addFriendWidget = new Addfriend(this->tcpsocket, this->userName);
    addFriendWidget->show();
}

//添加好友
void Chatlist::client_add_friend_reply(QJsonObject &obj)
{
    if(obj.value("result").toString() == "user_not_exist")
        QMessageBox::warning(this, "添加好友提醒", "好友不存在！");
    else if(obj.value("result").toString() == "already_friend")
        QMessageBox::warning(this, "添加好友提醒", "已经是好友！");
    else if(obj.value("result").toString() == "success")
    {
        QMessageBox::information(this, "添加好友提醒", "好友添加成功！");
        ui->friendList->addItem(obj.value("friend").toString());
    }
}

//被添加好友
void Chatlist::client_be_add_friend_reply(QString fri)
{
    QString str = QString("%1把你添加为好友！").arg(fri);
    QMessageBox::information(this, "添加好友提醒", str);
    ui->friendList->addItem(fri);

}

//点击创建群按钮
void Chatlist::on_createGroupbut_clicked()
{
    CreateGroup *createGroupWidget = new CreateGroup(this->tcpsocket, this->userName);
    createGroupWidget->show();
}

//创建群
void Chatlist::client_create_group_reply(QJsonObject &obj)
{
    if(obj.value("result").toString() == "group_exist")
        QMessageBox::warning(this, "创建群提示", "群已经存在！");
    else if(obj.value("result").toString() == "success")
        ui->groupList->addItem(obj.value("group").toString());
}

//点击添加群按钮
void Chatlist::on_addGroupbut_clicked()
{
    AddGroup *addGroupWidget = new AddGroup(this->tcpsocket, this->userName);
    addGroupWidget->show();
}

//添加群
void Chatlist::client_add_group_reply(QJsonObject &obj)
{
    if(obj.value("result").toString() == "group_not_exist")
        QMessageBox::warning(this, "添加群提示", "群不存在！");
    else if(obj.value("result").toString() == "user_in_group")
        QMessageBox::warning(this, "添加群提示", "已经在群里！");
    else if(obj.value("result").toString() == "success")
        ui->groupList->addItem(obj.value("group").toString());
}

//双击好友列表
void Chatlist::on_friendList_double_clicked()
{
    QString friendName = ui->friendList->currentItem()->text();
    PrivateChat *privateChatWidget = new PrivateChat(this->tcpsocket, this->userName, friendName, this, &chatWidgetList);
    privateChatWidget->setWindowTitle(friendName);
    privateChatWidget->show();

    ChatWidgetInfo c = {privateChatWidget, friendName};
    this->chatWidgetList.push_back(c);
}

//对方不在线
void Chatlist::client_private_chat_reply(QString res)
{
    if(res == "offline")
        QMessageBox::warning(this, "发送提醒", "对方不在线");
}

//收到消息
void Chatlist::client_chat_reply(QJsonObject &obj)
{
    int flag = 0;
    for(int i = 0; i < this->chatWidgetList.size(); ++i)
        if(this->chatWidgetList.at(i).name == obj.value("user_from").toString())
        {
            flag = 1;
            break;
        }

    if(flag == 0)
    {
        QString friendName = obj.value("user_from").toString();
        PrivateChat *privateChatWidget = new PrivateChat(this->tcpsocket, this->userName, friendName, this, &chatWidgetList);
        privateChatWidget->setWindowTitle(friendName);
        privateChatWidget->show();

        ChatWidgetInfo c = {privateChatWidget, friendName};
        this->chatWidgetList.push_back(c);
    }

    emit signal_to_sub_widget(obj);
}

//双击群列表
void Chatlist::on_groupList_double_clicked()
{
    QString groupName = ui->groupList->currentItem()->text();
    GroupChat *groupChatWidget = new GroupChat(this->tcpsocket, this->userName, groupName, this, &groupWidgetList);
    groupChatWidget->setWindowTitle(groupName);
    groupChatWidget->show();

    groupWidgetInfo g = {groupChatWidget, groupName};
    groupWidgetList.push_back(g);
}

//获取群成员
void Chatlist::client_get_group_member_reply(QJsonObject &obj)
{
    emit signal_to_sub_widget_member(obj);
}

//收到群消息
void Chatlist::client_group_chat_reply(QJsonObject &obj)
{
    int flag = 0;
    for(int i = 0; i <groupWidgetList.size(); ++i)
        if(groupWidgetList.at(i).name == obj.value("group").toString())
        {
            flag = 1;
            break;
        }

    if(flag == 0)
    {
        QString groupName = obj.value("group").toString();
        GroupChat *groupChatWidget = new GroupChat(this->tcpsocket, this->userName, groupName, this, &groupWidgetList);
        groupChatWidget->setWindowTitle(groupName);
        groupChatWidget->show();

        groupWidgetInfo g = {groupChatWidget, groupName};
        groupWidgetList.push_back(g);
    }

    emit signal_to_sub_widget_group(obj);
}

//传出文件异常回复
void Chatlist::client_send_file_reply(QString res)
{
    if(res == "offline")
        QMessageBox::warning(this, "发送文件提醒", "对方不在线！");
    else if(res == "timeout")
        QMessageBox::warning(this, "发送文件提醒", "连接超时");
}

//开始发送文件
void Chatlist::client_send_file_port_reply(QJsonObject &obj)
{
    SendThread *mysendthread = new SendThread(obj);
    mysendthread->start();

    connect(mysendthread, &SendThread::start_send, this, [=](){
        QMessageBox::information(this, "发送文件", "开始发送");
    });
    connect(mysendthread, &SendThread::end_send, this, [=](){
        QMessageBox::information(this, "发送文件", "发送文件结束");
    });
}

//开始接收文件
void Chatlist::client_recv_file_port_reply(QJsonObject &obj)
{
    QThread *subThread = new QThread;
    RecvFile *work = new RecvFile(obj);
    work->moveToThread(subThread);

    //让线程工作
    subThread->start();

    //通过信号让工作对象工作
    connect(this, &Chatlist::recv_file_working, work, &RecvFile::working);
    emit recv_file_working();

    //收到超时信号
    connect(work, &RecvFile::recv_connect_timeout, this, [=](){
        QMessageBox::warning(this, "连接提示", "连接服务器超时");

        subThread->quit();
        subThread->wait();
        subThread->deleteLater();
        work->deleteLater();
    });
    //收到开始接收信号
    connect(work, &RecvFile::start_recv, this, [=](){
        QMessageBox::information(this, "接收提示", "开始接收文件");
    });
    //收到接收完毕信号
    connect(work, &RecvFile::recv_done, this, [=](){
        QMessageBox::information(this, "接收提示", "文件接收完成");

        subThread->quit();
        subThread->wait();
        subThread->deleteLater();
        work->deleteLater();
    });
}

//好友下线
void Chatlist::client_friend_offline(QString fri)
{
    QString str = QString("%1下线").arg(fri);
    QMessageBox::information(this, "下线提醒", str);
}
