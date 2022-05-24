#pragma once

// Based on : https://doc.qt.io/qt-5/qtnetwork-fortuneserver-example.html

#include <QtWidgets>

class QTcpServer;

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server();

private slots:
    void send();

private:
    void initServer();

    QString status = nullptr;
    QTcpServer *tcpServer = nullptr;
};