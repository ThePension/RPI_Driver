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

    if (!tcpServer->listen()) 
    {
        qDebug() << "Unable to start the server : " << tcpServer->errorString();
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) 
    {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) 
        {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty()) ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    qDebug() << "The server is running on " << (tr("IP: %1 and port: %2")).arg(ipAddress).arg(tcpServer->serverPort());
}

void Server::retrieveData()
{
    char receiveChar[BUFFER_LENGTH]; // The receive buffer from the driver

    int * receiveInt = new int[DATA_NUMBER * 4];
    receiveInt = (int*)receiveChar;

    int ret, fd;

    try
    {
        fd = open("/dev/drvI2C", O_RDWR); // Open the device with read/write access
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }

    if (fd < 0)
    {
        perror("Failed to open the device...");
        return;
    }

    try
    {
        ret = read(fd, receiveChar, BUFFER_LENGTH); // Read the response from the driver
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }

    if (ret < 0)
    {
        perror("Failed to read the message from the device.");
        return;
    }

    printf("The received message is: \n");
    /*for(int i = 0; i < 400; i++)
    {
        printf("%d : %d\n", i, receiveInt[i]);
    }*/

    for(int i = 0, y = 0; i < DATA_NUMBER * 4; i += 4, y++)
    {
        try
        {
            this->datas[y].luminosity = receiveInt[i];
            this->datas[y].red = receiveInt[i + 1];
            this->datas[y].green = receiveInt[i + 2];
            this->datas[y].blue = receiveInt[i + 3];
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    close(fd);
}

int Server::convertToInt(std::string str)
{
    int num = 0;
    try
    {
        num = std::stoi(str);
    }
    catch(const std::exception& e)
    {
        qDebug() << "Impossible to convert the string : '" << QString::fromStdString(str) << "' to int";
        num = 255; 
    }
    return num;
}

void Server::send()
{
    retrieveData();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);

    for(int i = 0; i < DATA_NUMBER; i++)
    {
        out << this->datas[i];
    }  

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected, clientConnection, &QObject::deleteLater);

    clientConnection->write(block);
    clientConnection->disconnectFromHost();
}