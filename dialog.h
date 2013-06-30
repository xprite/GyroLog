#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <qextserialport.h>
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QDateTime>

#define DEFAULT_TIMER_TICK_TIME 100

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    QextSerialPort *iSerialPort;
    QTimer *iTimer;
    QFile *iFile;

public slots:
    void onTimer();
    void onReadyRead();

};

#endif // DIALOG_H
