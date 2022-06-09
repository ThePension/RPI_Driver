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

    this->isRunning = (true);

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
}

void Client::request()
{
    tcpSocket->abort();
    tcpSocket->connectToHost(hostCombo->currentText(), portLineEdit->text().toInt());
}

void Client::getData()
{
    this->isRunning = true;
    this->getDataButton->setEnabled(false);
    this->stopButton->setEnabled(true);
    this->getDataThread = std::thread(&Client::run, this, "Start getting data");
}

void Client::run(std::string msg)
{
    while(this->isRunning)
    {
        this->request();
        // this->generateRandomData();
        // sleep(1);
        int k = 0;
        for(int i = 0; i < 1000000000; i++) { k = sqrt(i); int l = i%5; int o = (int)(sqrt(i)) % 18; }
        // std::this_thread::sleep_for (std::chrono::seconds(1));
    }
}

void Client::stop()
{
    this->isRunning = false;
    this->getDataThread.join();
    this->getDataButton->setEnabled(true);
    this->stopButton->setEnabled(false);
}

void Client::read()
{
    resetSeries();

    getDataButton->setEnabled(false);

    in.startTransaction();

    for(int i = 0; i < DATA_NUMBER; i++)
    {
        Data nextData;
        in >> nextData;
        datas[i] = nextData;
    }   

    if (!in.commitTransaction()) return;

    statusLabel->setText("Datas received");
    getDataButton->setEnabled(true);

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