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

    refreshGraphButton = new QPushButton(tr("Generate random data"));
    startGettingDataButton = new QPushButton(tr("Start getting data"));

    connect(refreshGraphButton, &QPushButton::clicked, this, &Client::generateRandomData);

    statusLabel = new QLabel();

    portLineEdit->setValidator(new QIntValidator(1, 65535, this));

    auto hostLabel = new QLabel(tr("&Server name:"));
    hostLabel->setBuddy(hostCombo);
    auto portLabel = new QLabel(tr("S&erver port:"));
    portLabel->setBuddy(portLineEdit);

    getDataButton->setDefault(true);

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(getDataButton, QDialogButtonBox::ActionRole);

    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    connect(hostCombo, &QComboBox::editTextChanged, this, &Client::enableGetDataButton);
    connect(portLineEdit, &QLineEdit::textChanged, this, &Client::enableGetDataButton);
    connect(getDataButton, &QAbstractButton::clicked, this, &Client::request);
    connect(tcpSocket, &QIODevice::readyRead, this, &Client::read);
    connect(startGettingDataButton, &QPushButton::clicked, this, &Client::startGettingData);

    QGridLayout *mainLayout = nullptr;
    if (QGuiApplication::styleHints()->showIsFullScreen() || QGuiApplication::styleHints()->showIsMaximized()) 
    {
        auto outerVerticalLayout = new QVBoxLayout(this);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        auto outerHorizontalLayout = new QHBoxLayout;
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        auto groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        mainLayout = new QGridLayout(groupBox);
        outerHorizontalLayout->addWidget(groupBox);
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        outerVerticalLayout->addLayout(outerHorizontalLayout);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    } 
    else 
    {
        mainLayout = new QGridLayout(this);
    }
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostCombo, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);
    mainLayout->addWidget(startGettingDataButton, 4, 0);

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
    chart->setTitle("Simple line chart example");

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

void Client::startGettingData()
{
    /*while(true)
    {
        this->request();
        sleep(500);
    }*/
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

void Client::enableGetDataButton()
{
    getDataButton->setEnabled(!hostCombo->currentText().isEmpty() &&
                                 !portLineEdit->text().isEmpty());

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