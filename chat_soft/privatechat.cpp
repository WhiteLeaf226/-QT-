#include "privatechat.h"
#include "ui_privatechat.h"

PrivateChat::PrivateChat(QTcpSocket *socket, QString u_name, QString f_name, Chatlist *c, QList<ChatWidgetInfo> *l, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
    this->tcpsocket = socket;
    this->userName = u_name;
    this->friendName = f_name;
    this->mainWifget = c;
    this->chatWidgetList = l;

    connect(mainWifget, &Chatlist::signal_to_sub_widget, this, &PrivateChat::show_text_slot);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}



//点击发送按钮
void PrivateChat::on_sendbut_clicked()
{
    QString text = ui->lineEdit->text();
    QJsonObject obj;
    obj.insert("cmd", "private_chat");
    obj.insert("user_from", this->userName);
    obj.insert("user_to", this->friendName);
    obj.insert("text", text);

    QByteArray ba = QJsonDocument(obj).toJson();
    this->tcpsocket->write(ba);

    ui->lineEdit->clear();
    ui->textEdit->append(text);
    ui->textEdit->append("\n");
}

//将收到的消息显示在对话窗口
void PrivateChat::show_text_slot(QJsonObject obj)
{
    if(obj.value("cmd").toString() == "private_chat")
    {
        if(obj.value("user_from").toString() == friendName)
        {
            if(this->isMinimized())
                this->showNormal();
            this->activateWindow();
            ui->textEdit->append(obj.value("text").toString());
            ui->textEdit->append("\n");
        }
    }
}

void PrivateChat::closeEvent(QCloseEvent *event)
{
    for(int i = 0; i < this->chatWidgetList->size(); ++i)
        if(this->chatWidgetList->at(i).name == friendName)
        {
            chatWidgetList->removeAt(i);
            break;
        }
    event->accept();
}


//点击传输文件按钮
void PrivateChat::on_filebut_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "发送文件", QCoreApplication::applicationFilePath());
    if(fileName.isEmpty())
    {
        QMessageBox::warning(this, "发送文件提示", "请选择一个文件！");
    }
    else
    {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QJsonObject obj;
        obj.insert("cmd", "send_file");
        obj.insert("from_user", this->userName);
        obj.insert("to_user", this->friendName);
        obj.insert("length", file.size());
        obj.insert("filename", fileName);
        QByteArray ba = QJsonDocument(obj).toJson();
        this->tcpsocket->write(ba);

    }
}

