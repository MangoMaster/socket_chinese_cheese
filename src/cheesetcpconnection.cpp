#include "cheesetcpconnection.h"
#include <QtWidgets/QMessageBox>
#include <cassert>
#include <QtCore/QDebug>

CheeseTcpConnection::CheeseTcpConnection(QObject *parent)
    : QObject(parent)
{
}

CheeseTcpConnection::~CheeseTcpConnection()
{
    delete server;
    delete socket;
}

bool CheeseTcpConnection::initTcpServer(uint16_t port)
{
    this->server = new QTcpServer(this);
    if (!this->server->listen(QHostAddress::Any, port))
    {
        QMessageBox::warning(nullptr, "Error", "This port has been used.");
        return false;
    }
    QObject::connect(this->server, SIGNAL(newConnection()), this, SLOT(serverAcceptConnection()));

    // wait for new connection for 30s
    QProgressDialog dialog("Waiting for connection for 30s ...", "Cancel", 0, 30);
    ServerWaitProgress progress(&dialog);
    QTimer t;
    QObject::connect(&t, SIGNAL(timeout()), &progress, SLOT(serverWaitProgressContinue()));            // continue;
    QObject::connect(this->server, SIGNAL(newConnection()), &progress, SLOT(serverWaitProgressEnd())); // end
    t.start(1000);
    dialog.exec(); // modal progress dialog
    t.stop();
    if (dialog.wasCanceled())
        return false;
    if (!progress.progressIsEnded())
    {
        QMessageBox::warning(nullptr, "Error", "Waiting time out.");
        return false;
    }
    // QMessageBox::information(nullptr, "Info", "Connection succeed.");
    return true;
}

bool CheeseTcpConnection::initTcpClient(const QString &serverAddress, uint16_t port)
{
    this->socket = new QTcpSocket(this);
    this->socket->connectToHost(serverAddress, port);
    QObject::connect(this->socket, SIGNAL(readyRead()), this, SLOT(recv()));

    // wait for new connection for 5s
    QProgressDialog dialog("Waiting for connection for 5s ...", "Cancel", 0, 5);
    ServerWaitProgress progress(&dialog);
    QTimer t;
    QObject::connect(&t, SIGNAL(timeout()), &progress, SLOT(serverWaitProgressContinue()));        // continue;
    QObject::connect(this->socket, SIGNAL(connected()), &progress, SLOT(serverWaitProgressEnd())); // end
    t.start(1000);
    dialog.exec(); // modal progress dialog
    t.stop();
    if (dialog.wasCanceled())
        return false;
    if (!progress.progressIsEnded())
    {
        QMessageBox::warning(nullptr, "Error", "Waiting time out.");
        return false;
    }
    // QMessageBox::information(nullptr, "Info", "Connection succeed.");
    return true;
}

void CheeseTcpConnection::serverAcceptConnection()
{
    this->socket = this->server->nextPendingConnection();
    QObject::connect(this->socket, SIGNAL(readyRead()), this, SLOT(recv()));
}

void CheeseTcpConnection::send(const std::array<std::array<Cheese *, 9>, 10> &changedCheese, CheeseColor currentColor)
{
    QByteArray byteArray;
    byteArray.append('n'); // new
    byteArray.reserve(2 + 9 * 10 * (sizeof(CheeseColor) + sizeof(CheeseKind)));
    for (const auto &cheeseArray : changedCheese)
        for (auto cheese : cheeseArray)
        {
            if (cheese == nullptr)
            {
                byteArray.append(static_cast<char>(-1));
                byteArray.append(static_cast<char>(-1));
            }
            else
            {
                byteArray.append(static_cast<char>(cheese->color));
                byteArray.append(static_cast<char>(cheese->kind));
            }
        }
    byteArray.append(static_cast<char>(currentColor));
    this->send(byteArray);
}

void CheeseTcpConnection::send(CheesePoint startCheesePoint, CheesePoint endCheesePoint)
{
    QByteArray byteArray;
    byteArray.append('g'); // go
    byteArray.append(static_cast<char>(startCheesePoint.row));
    byteArray.append(static_cast<char>(startCheesePoint.column));
    byteArray.append(static_cast<char>(endCheesePoint.row));
    byteArray.append(static_cast<char>(endCheesePoint.column));
    this->send(byteArray);
}

void CheeseTcpConnection::send(QByteArray byteArray)
{
    this->socket->write(byteArray);
    this->socket->flush();
}

void CheeseTcpConnection::recv()
{
    QByteArray byteArray = this->socket->readAll();
    switch (byteArray[0])
    {
    case 'n': // new
    {
        std::array<std::array<Cheese *, 9>, 10> changedCheese;
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 9; ++j)
            {
                if (byteArray[1 + 2 * (j + 9 * i)] == static_cast<char>(-1))
                    changedCheese[i][j] = nullptr;
                else
                {
                    CheeseColor cheeseColor;
                    CheeseKind cheeseKind;
                    switch (byteArray[1 + 2 * (j + 9 * i)])
                    {
                    case static_cast<char>(CheeseColor::red):
                        cheeseColor = CheeseColor::red;
                        break;
                    case static_cast<char>(CheeseColor::black):
                        cheeseColor = CheeseColor::black;
                        break;
                    }
                    switch (byteArray[2 + 2 * (j + 9 * i)])
                    {
                    case static_cast<char>(CheeseKind::che):
                        cheeseKind = CheeseKind::che;
                        break;
                    case static_cast<char>(CheeseKind::ma):
                        cheeseKind = CheeseKind::ma;
                        break;
                    case static_cast<char>(CheeseKind::pao):
                        cheeseKind = CheeseKind::pao;
                        break;
                    case static_cast<char>(CheeseKind::xiang):
                        cheeseKind = CheeseKind::xiang;
                        break;
                    case static_cast<char>(CheeseKind::shi):
                        cheeseKind = CheeseKind::shi;
                        break;
                    case static_cast<char>(CheeseKind::shuai):
                        cheeseKind = CheeseKind::shuai;
                        break;
                    case static_cast<char>(CheeseKind::bing):
                        cheeseKind = CheeseKind::bing;
                        break;
                    }
                    changedCheese[i][j] = new Cheese(cheeseColor, cheeseKind, i, j);
                }
            }

        CheeseColor currentColor;
        switch (byteArray[byteArray.size() - 1])
        {
        case static_cast<char>(CheeseColor::red):
            currentColor = CheeseColor::red;
            break;
        case static_cast<char>(CheeseColor::black):
            currentColor = CheeseColor::black;
            break;
        }

        emit recvChanged(changedCheese, currentColor);
    }
    break;
    case 'g': // go
    {
        CheesePoint startCheesePoint(static_cast<int>(byteArray[1]), static_cast<int>(byteArray[2]));
        CheesePoint endCheesePoint(static_cast<int>(byteArray[3]), static_cast<int>(byteArray[4]));
        emit recvChanged(startCheesePoint, endCheesePoint);
    }
    break;
    }
}

ServerWaitProgress::ServerWaitProgress(QProgressDialog *dialog, QObject *parent)
    : QObject(parent), dialog(dialog)
{
}

void ServerWaitProgress::serverWaitProgressContinue()
{
    ++(this->progress);
    dialog->setValue(this->progress);
}

void ServerWaitProgress::serverWaitProgressEnd()
{
    dialog->setValue(dialog->maximum());
    this->progressEnded = true;
}