#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(this->ui->action_Exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(&(this->t), SIGNAL(timeout()), this, SLOT(timerSingalTimeOut()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerStop()
{
    t.stop();
    this->end = true;
}

void MainWindow::timerZero()
{
    if (this->end)
        return;
    this->time = 60;
    this->ui->lcdNumber->display(this->time);
    t.start(1000);
}

void MainWindow::timerSingalTimeOut()
{
    --(this->time);
    this->ui->lcdNumber->display(this->time);
    if (this->time == 0)
    {
        this->timerStop();
        emit timerTimeout();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    this->ui->pushButton->click();
    QMainWindow::closeEvent(event);
}