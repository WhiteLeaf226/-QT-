#ifndef ADDGROUP_H
#define ADDGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class AddGroup;
}

class AddGroup : public QWidget
{
    Q_OBJECT

public:
    explicit AddGroup(QTcpSocket *, QString, QWidget *parent = nullptr);
    ~AddGroup();

private slots:
    void on_cancelbut_clicked();

    void on_addbut_clicked();

private:
    Ui::AddGroup *ui;
    QTcpSocket *tcpsocket;
    QString userName;
};

#endif // ADDGROUP_H
