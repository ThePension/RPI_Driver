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

    setTemperature = new QBarSet("Température");
    setHumidity = new QBarSet("Humidité");
    setPressure = new QBarSet("Pression");

    *setTemperature << 1 << 2 << 3 << 4 << 5 << 6;
    *setHumidity << 5 << 0 << 0 << 4 << 0 << 7;
    *setPressure << 3 << 5 << 8 << 13 << 8 << 5;

    series = new QBarSeries();

    series->append(setTemperature);
    series->append(setHumidity);
    series->append(setPressure);

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
    statusLabel->setText("Requesting...");
    tcpSocket->abort();
    tcpSocket->connectToHost(hostCombo->currentText(), portLineEdit->text().toInt());
}

void Client::read()
{
    getDataButton->setEnabled(false);

    in.startTransaction();

    Data nextData;
    in >> nextData;

    if (!in.commitTransaction())
        return;

    /*if (nextData == currentData) {
        QTimer::singleShot(0, this, &Client::request);
        return;
    }*/

    currentData = nextData;
    statusLabel->setText("Temperature : " + QString::number(currentData.temperature));
    getDataButton->setEnabled(true);
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