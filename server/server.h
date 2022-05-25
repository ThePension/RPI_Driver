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

#define DATA_NUMBER 100
#define BUFFER_LENGTH (4 * DATA_NUMBER + 1) // The buffer length

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

    Data datas[DATA_NUMBER];
    QString status = nullptr;
    QTcpServer *tcpServer = nullptr;
};