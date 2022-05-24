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
        qDebug() << "Unable to start the server : " << tcpServer->errorString();
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
    qDebug() << "The server is running on";
    qDebug() << (tr("IP: %1 and port: %2")).arg(ipAddress).arg(tcpServer->serverPort());
}

void Server::retrieveData()
{
    static char receive[BUFFER_LENGTH];     // The receive buffer from the driver

    int ret, fd;

    fd = open("/dev/drvI2C", O_RDWR);             // Open the device with read/write access
    if (fd < 0)
    {
        perror("Failed to open the device...1");
        return;
    }

    ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the driver
    if (ret < 0){
        perror("Failed to read the message from the device.");
        return;
    }

    printf("The received message is: [%s]\n", receive);

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