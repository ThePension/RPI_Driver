#pragma once

// Based on : https://doc.qt.io/qt-5/qtnetwork-fortuneclient-example.html
// and : https://doc.qt.io/qt-5/qtcharts-barchart-example.html

#include <QDataStream>
#include <QDialog>
#include <QTcpSocket>
#include <QtCharts>

#include "../common/data.h"

#define DATA_NUMBER 100 

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
    Data datas[DATA_NUMBER];

    QBarSet *setLuminosity = nullptr;
    QBarSet *setRed = nullptr;
    QBarSet *setBlue = nullptr;
    QBarSet *setGreen = nullptr;

    QBarSeries *series = nullptr;

    QChart *chart = nullptr;

    QChartView *chartView;

    void displayData();
};