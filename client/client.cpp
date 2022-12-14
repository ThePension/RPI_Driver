#include <QtWidgets>
#include <QtNetwork>

#include "client.h"

using namespace QtCharts;

Client::Client(QWidget *parent)
    : QWidget(parent)
    , hostCombo(new QComboBox)
    , portLineEdit(new QLineEdit)
    , getDataButton(new QPushButton(tr("Get Data")))
    , tcpSocket(new QTcpSocket(this))
{
    hostCombo->setEditable(true);
    hostCombo->addItem(QString("157.26.91.84"));

    stopButton = new QPushButton(tr("Stop"));

    statusLabel = new QLabel();

    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    auto hostLabel = new QLabel(tr("&Server name:"));
    hostLabel->setBuddy(hostCombo);
    auto portLabel = new QLabel(tr("S&erver port:"));
    portLabel->setBuddy(portLineEdit);

    getDataButton->setDefault(true);

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getDataButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(stopButton, QDialogButtonBox::ActionRole);

    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    connect(getDataButton, &QAbstractButton::clicked, this, &Client::getData);
    connect(tcpSocket, &QIODevice::readyRead, this, &Client::read);
    connect(stopButton, &QPushButton::clicked, this, &Client::stop);

    QGridLayout *mainLayout = new QGridLayout(this);

    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostCombo, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    portLineEdit->setFocus();

    setLuminosity = new QLineSeries();
    setRed = new QLineSeries();
    setBlue = new QLineSeries();
    setGreen = new QLineSeries();

    QPen peni(QRgb(0x000000));
    peni.setWidth(3);
    setLuminosity->setPen(peni);

    QPen penr(QRgb(0xff0000));
    penr.setWidth(3);
    setRed->setPen(penr);
    
    QPen peng(QRgb(0x00ff00));
    peng.setWidth(3);
    setGreen->setPen(peng);

    QPen penb(QRgb(0x0000ff));
    penb.setWidth(3);
    setBlue->setPen(penb);
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 255);
    axisY->setTickCount(10);
    axisY->setLabelFormat("%.2f");

    chart = new QChart();

    chart->addSeries(setLuminosity);
    chart->addSeries(setRed);
    chart->addSeries(setBlue);
    chart->addSeries(setGreen);
    chart->setTitle("Color chart");

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->setAxisY(axisY, setLuminosity);

    mainLayout->addWidget(chartView, 4, 1);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Client::run);
}

void Client::request()
{
    tcpSocket->abort();
    tcpSocket->connectToHost(hostCombo->currentText(), portLineEdit->text().toInt());
}

void Client::getData()
{
    this->getDataButton->setEnabled(false);
    this->stopButton->setEnabled(true);
    timer->start(1000);
}

void Client::run()
{
    this->request();
}

void Client::stop()
{
    timer->stop();
    this->getDataButton->setEnabled(true);
    this->stopButton->setEnabled(false);
}

void Client::read()
{
    resetSeries();

    in.startTransaction();

    for(int i = 0; i < DATA_NUMBER; i++)
    {
        Data nextData;
        in >> nextData;
        datas[i] = nextData;
    }   

    if (!in.commitTransaction()) return;

    displayData();
}

void Client::generateRandomData()
{
    resetSeries();

    for(int i = 0; i < DATA_NUMBER; i++)
    {
        datas[i].luminosity = QRandomGenerator::global()->generateDouble() * 255;
        datas[i].red = QRandomGenerator::global()->generateDouble() * 255;
        datas[i].blue = QRandomGenerator::global()->generateDouble() * 255;
        datas[i].green = QRandomGenerator::global()->generateDouble() * 255;
    }

    displayData();
}

void Client::displayData()
{
    for(int i = 0; i < DATA_NUMBER; i++)
    {
        *setLuminosity << QPointF(i, datas[i].luminosity);
        *setRed << QPointF(i, datas[i].red);
        *setBlue << QPointF(i, datas[i].blue);
        *setGreen << QPointF(i, datas[i].green);
    }
    
    chart->removeSeries(setLuminosity);
    chart->addSeries(setLuminosity); 
    chart->removeSeries(setRed);
    chart->addSeries(setRed); 
    chart->removeSeries(setBlue);
    chart->addSeries(setBlue); 
    chart->removeSeries(setGreen);
    chart->addSeries(setGreen); 

    chartView->update();
    chartView->repaint();
    this->update();
}

void Client::resetSeries()
{
    setLuminosity->clear();
    setRed->clear();
    setBlue->clear();
    setGreen->clear();
}