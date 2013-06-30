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

}

Dialog::~Dialog()
{
    delete ui;
    delete(this->iSerialPort);
    delete(this->iTimer);
    delete(this->iFile);
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
