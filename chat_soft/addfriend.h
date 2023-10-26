#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

namespace Ui {
class Addfriend;
}

class Addfriend : public QWidget
{
    Q_OBJECT

public:
    explicit Addfriend(QTcpSocket *, QString, QWidget *parent = nullptr);
    ~Addfriend();

private slots:
    void on_cancelbut_clicked();

    void on_addbut_clicked();

private:
    Ui::Addfriend *ui;
    QTcpSocket * tcpsocket;
    QString userName;
};

#endif // ADDFRIEND_H
