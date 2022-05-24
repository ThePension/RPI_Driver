#pragma once

// Based on : https://doc.qt.io/qt-5/qtnetwork-fortuneserver-example.html

#include <QtCore>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "../common/data.h"

#define BUFFER_LENGTH 256 // The buffer length 

class QTcpServer;

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server();
    void retrieveData();

private slots:
    void send();

private:
    void initServer();

    Data data;
    QString status = nullptr;
    QTcpServer *tcpServer = nullptr;
};