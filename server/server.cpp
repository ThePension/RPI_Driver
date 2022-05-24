#include <QtNetwork>

#include "server.h"

Server::Server()
{
    this->initServer();

    connect(tcpServer, &QTcpServer::newConnection, this, &Server::send);
}

void Server::initServer()
{
    this->tcpServer = new QTcpServer(this);
    if (!tcpServer->listen()) {
        QMessageBox::critical(this, tr("Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    status = (tr("The server is running on\n\nIP: %1\nport: %2\n\n"
                            "Run the Fortune Client example now.")
                         .arg(ipAddress).arg(tcpServer->serverPort()));
}

void Server::retrieveData()
{
    static char receive[BUFFER_LENGTH];     // The receive buffer from the driver

    int ret, fd;

    fd = open("/dev/drvTest", O_RDWR);             // Open the device with read/write access
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return;
    }

    ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the driver
    if (ret < 0){
        perror("Failed to read the message from the device.");
        return;
    }

    // this->data.temperature = receive[]
}

void Server::send()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);

    out << this->data;

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected, clientConnection, &QObject::deleteLater);

    clientConnection->write(block);
    clientConnection->disconnectFromHost();
}