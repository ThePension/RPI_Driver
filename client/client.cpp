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
    // connect(tcpSocket, &QAbstractSocket::errorOccurred, this, &Client::displayError);

    QGridLayout *mainLayout = nullptr;
    if (QGuiApplication::styleHints()->showIsFullScreen() || QGuiApplication::styleHints()->showIsMaximized()) {
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
    } else {
        mainLayout = new QGridLayout(this);
    }
    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(hostCombo, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(statusLabel, 2, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 3, 0, 1, 2);

    setWindowTitle(QGuiApplication::applicationDisplayName());
    portLineEdit->setFocus();

    setLuminosity = new QBarSet("Luminosity");
    setRed = new QBarSet("Red");
    setBlue = new QBarSet("Blue");
    setGreen = new QBarSet("Green");

    series = new QBarSeries();

    series->append(setLuminosity);
    series->append(setRed);
    series->append(setBlue);
    series->append(setGreen);

    chart = new QChart();

    chart->addSeries(series);
    chart->setTitle("Simple barchart example");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    mainLayout->addWidget(chartView, 4, 1);
}

void Client::request()
{
    tcpSocket->abort();
    tcpSocket->connectToHost(hostCombo->currentText(), portLineEdit->text().toInt());
}

void Client::read()
{
    getDataButton->setEnabled(false);

    in.startTransaction();

    for(int i = 0; i < DATA_NUMBER; i++)
    {
        Data nextData;
        in >> nextData;
        datas[i] = nextData;
    }   

    if (!in.commitTransaction())
        return;

    statusLabel->setText("Datas received" + QString::number(datas[0].red));
    getDataButton->setEnabled(true);

    displayData();
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, 
                                    tr("Client"),
                                    tr("The host was not found. Please check the host name and port settings."));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, 
                                    tr("Client"),
                                    tr("The connection was refused by the peer. Make sure the server is running, and check that the host name and port settings are correct."));
            break;
        default:
            QMessageBox::information(this, 
                                    tr("Client"),
                                    tr("The following error occurred: %1.")
                                        .arg(tcpSocket->errorString()));
    }

    getDataButton->setEnabled(true);
}

void Client::enableGetDataButton()
{
    getDataButton->setEnabled(!hostCombo->currentText().isEmpty() &&
                                 !portLineEdit->text().isEmpty());

}

void Client::displayData()
{
    for(int i = 0; i < DATA_NUMBER; i++)
    {
        *setLuminosity << datas[i].luminosity;
        *setRed << datas[i].red;
        *setBlue << datas[i].blue;
        *setGreen << datas[i].green;
    }
    this->update();
    chart->update();
    chartView->update();
}