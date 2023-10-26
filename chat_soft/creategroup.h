#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class CreateGroup;
}

class CreateGroup : public QWidget
{
    Q_OBJECT

public:
    explicit CreateGroup(QTcpSocket *, QString, QWidget *parent = nullptr);
    ~CreateGroup();

private slots:
    void on_cancelbut_clicked();

    void on_createbut_clicked();

private:
    Ui::CreateGroup *ui;
    QTcpSocket *tcpsocket;
    QString userName;
};

#endif // CREATEGROUP_H
