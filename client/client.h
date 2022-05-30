#pragma once

// Based on : https://doc.qt.io/qt-5/qtnetwork-fortuneclient-example.html
// and : https://doc.qt.io/qt-5/qtcharts-barchart-example.html

#include <QDataStream>
#include <QDialog>
#include <QTcpSocket>
#include <QtCharts>
#include <unistd.h>

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

public slots:
    void request();
    void read();
    void enableGetDataButton();
    void generateRandomData();
    void displayData();
    void resetSeries();
    void startGettingData();

private:
    QComboBox *hostCombo = nullptr;
    QLineEdit *portLineEdit = nullptr;
    QLabel *statusLabel = nullptr;
    QPushButton *getDataButton = nullptr;
    QPushButton *refreshGraphButton = nullptr;
    QPushButton * startGettingDataButton = nullptr;

    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;
    Data datas[DATA_NUMBER];

    QLineSeries *setLuminosity = nullptr;
    QLineSeries *setRed = nullptr;
    QLineSeries *setBlue = nullptr;
    QLineSeries *setGreen = nullptr;

    QChart *chart = nullptr;

    QChartView *chartView;
};