#include "dialog.h"
#include "ui_dialog.h"

#include "qextserialenumerator.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //Get available COM ports
    this->iTimer = new QTimer(this);
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    foreach(QextPortInfo info, ports) {
        ui->comboBox->addItem(info.portName);
    }

    PortSettings settings = {BAUD115200, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};   // default settings to
    this->iSerialPort = new QextSerialPort(ui->comboBox->currentText(), settings, QextSerialPort::EventDriven);

    // Connect slots
    connect(this->iSerialPort, SIGNAL(readyRead()), SLOT(onReadyRead()));   // Serial slot onReadyRead
    connect(this->iTimer, SIGNAL(timeout()), this, SLOT(onTimer()));    // Timer slot onTimer

    this->xAxis = new QVector<double>(101);
    this->yAxis = new QVector<double>(101);

    this->counter = 0;

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(*this->xAxis, *this->yAxis);
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    ui->customPlot->xAxis->setRange(-1, 101);
    ui->customPlot->yAxis->setRange(-32768, +32767 );

    ui->customPlot->replot();

}

Dialog::~Dialog()
{
    delete ui;
    delete(this->iSerialPort);
    delete(this->iTimer);
    delete(this->iFile);
}

double Dialog::fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


void Dialog::onTimer()
{
    this->iSerialPort->write("GetGyro\r");
}

void Dialog::onReadyRead()
{
    if (this->iSerialPort->bytesAvailable() >= 6) {
        QByteArray temp;
        temp = this->iSerialPort->read(6);
        qint16 GyroX = (temp[0] << 8) | (temp[1] << 0);
        qint16 GyroY = (temp[2] << 8) | (temp[3] << 0);
        qint16 GyroZ = (temp[4] << 8) | (temp[5] << 0);
        QString tempString = QString::number(GyroX) + "," + QString::number(GyroY) + "," + QString::number(GyroZ) + "\n";  // CSV formating of gyro data
        if (this->iFile->open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(this->iFile);
            out << tempString;
            this->iFile->flush();
            this->iFile->close();
        }
        this->counter++;
        this->xAxis->push_front(counter);
        this->yAxis->push_front(GyroX);
        ui->customPlot->graph(0)->setData(*xAxis, *yAxis);
        ui->customPlot->xAxis->setRange(counter-100, counter+50);
        ui->customPlot->replot();
        this->xAxis->pop_back();
        this->yAxis->pop_back();
    }
}

void Dialog::on_pushButton_clicked()
{
    if (!iSerialPort->isOpen()) {
           QDateTime dateTime;
           qint64 timeSinceEpoch = dateTime.currentMSecsSinceEpoch();
           QString fileName;
           fileName = QString::number(timeSinceEpoch);
           this->iFile = new QFile(fileName + ".csv");
           this->iSerialPort->setPortName(ui->comboBox->currentText());
           this->iSerialPort->open(QIODevice::ReadWrite);
           this->iTimer->start(DEFAULT_TIMER_TICK_TIME);   // defined in dialog.h
           this->ui->pushButton->setText("disconnect");
       }
       else {
           this->iSerialPort->close();
           this->iTimer->stop();
           this->ui->pushButton->setText("connect");
    }
}
