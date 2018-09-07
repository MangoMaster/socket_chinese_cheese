#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_2->setAlignment(Qt::AlignHCenter);
    QObject::connect(this->ui->action_Exit, SIGNAL(triggered(bool)), this, SLOT(close()));
    QObject::connect(&(this->t), SIGNAL(timeout()), this, SLOT(timerSingalTimeOut()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerStart()
{
    this->end = false;
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

void MainWindow::setCurrentColor(CheeseColor currentColor)
{
    switch (currentColor)
    {
    case CheeseColor::red:
        this->ui->label_2->setPixmap(QPixmap(":/icons/red_shuai.png"));
        break;
    case CheeseColor::black:
        this->ui->label_2->setPixmap(QPixmap(":/icons/black_shuai.png"));
        break;
    }
}