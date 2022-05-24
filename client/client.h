#pragma once

// Based on : https://doc.qt.io/qt-5/qtnetwork-fortuneclient-example.html

#include <QDataStream>
#include <QDialog>
#include <QTcpSocket>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = nullptr);

private slots:
    void request();
    void read();
    void displayError(QAbstractSocket::SocketError socketError);
    void enableGetDataButton();

private:
    QComboBox *hostCombo = nullptr;
    QLineEdit *portLineEdit = nullptr;
    QLabel *statusLabel = nullptr;
    QPushButton *getDataButton = nullptr;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;
    QString currentData;
};