#include "cheesemodel.h"
#include "modedialog.h"
#include "ui_modedialog.h"
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtNetwork/QNetworkInterface>
#include <cassert>
#include <QtCore/QDebug>

const CheesePoint CheeseModel::MA_POINT_DELTA[8] = {CheesePoint(-2, 1), CheesePoint(-1, 2), CheesePoint(1, 2), CheesePoint(2, 1), CheesePoint(2, -1), CheesePoint(1, -2), CheesePoint(-1, -2), CheesePoint(-2, -1)};
const CheesePoint CheeseModel::MA_BIE_POINT_DELTA[8] = {CheesePoint(-1, 0), CheesePoint(0, 1), CheesePoint(0, 1), CheesePoint(1, 0), CheesePoint(1, 0), CheesePoint(0, -1), CheesePoint(0, -1), CheesePoint(-1, 0)};
const CheesePoint CheeseModel::XIANG_POINT_DELTA[4] = {CheesePoint(-2, 2), CheesePoint(2, 2), CheesePoint(2, -2), CheesePoint(-2, -2)};
const CheesePoint CheeseModel::XIANG_BIE_POINT_DELTA[4] = {CheesePoint(-1, 1), CheesePoint(1, 1), CheesePoint(1, -1), CheesePoint(-1, -1)};
const CheesePoint CheeseModel::SHI_POINT_DELTA[4] = {CheesePoint(-1, 1), CheesePoint(1, 1), CheesePoint(1, -1), CheesePoint(-1, -1)};
const CheesePoint CheeseModel::SHUAI_POINT_DELTA[4] = {CheesePoint(-1, 0), CheesePoint(1, 0), CheesePoint(0, -1), CheesePoint(0, 1)};
const CheesePoint CheeseModel::BING_RED_POINT_DELTA[3] = {CheesePoint(-1, 0), CheesePoint(0, -1), CheesePoint(0, 1)};
const CheesePoint CheeseModel::BING_BLACK_POINT_DELTA[3] = {CheesePoint(1, 0), CheesePoint(0, -1), CheesePoint(0, 1)};

CheeseModel::CheeseModel(QObject *parent)
    : QObject(parent), cheeseChosenPoint(-1, -1)
{
    for (auto &cheeseArray : this->cheeseTable)
        for (auto &cheese : cheeseArray)
            cheese = nullptr;

    jiangJunPlayer = new QMediaPlayer(this);
    jiangJunPlayer->setMedia(QUrl("file:///home/lizhi/Documents/CodeProject/network/chinese_cheese/sounds/jiangjun.mp3"));
    jiangJunPlayer->setVolume(100);
}

CheeseModel::~CheeseModel()
{
    for (auto &cheeseArray : this->cheeseTable)
        for (auto &cheese : cheeseArray)
            delete cheese;

    delete jiangJunPlayer;
}

void CheeseModel::setNewModel()
{
    // set mode
    QStringList modes;
    modes << tr("单人模式") << tr("单机对战模式") << tr("联机对战模式");
    bool ok;
    this->mode = QInputDialog::getItem(nullptr, tr("对战模式"), tr("选择对战模式"), modes, 1, false, &ok);
    if (!ok)
        return;

    // set networkd
    if (this->mode == tr("单机对战模式"))
    {
        this->mode = tr("对战模式");
        ModeDialog dialog;
        dialog.ui->spinBox_2->setValue(127);
        dialog.ui->spinBox_2->setReadOnly(true);
        dialog.ui->spinBox_3->setValue(0);
        dialog.ui->spinBox_3->setReadOnly(true);
        dialog.ui->spinBox_4->setValue(0);
        dialog.ui->spinBox_4->setReadOnly(true);
        dialog.ui->spinBox_5->setValue(1);
        dialog.ui->spinBox_5->setReadOnly(true);
        if (dialog.exec() == QDialog::Accepted)
        {
            delete this->connection;
            this->connection = nullptr;
            this->connection = new CheeseTcpConnection();
            bool success = this->connection->initTcpServer(static_cast<uint16_t>(dialog.ui->spinBox->value()));
            if (!success)
            {
                delete this->connection;
                this->connection = nullptr;
                return;
            }
            this->connectToConnection();
        }
        else
            return;
    }
    else if (this->mode == tr("联机对战模式"))
    {
        this->mode = tr("对战模式");
        QString address;
        for (QHostAddress add : QNetworkInterface::allAddresses())
            if (add.protocol() == QAbstractSocket::IPv4Protocol)
                address = add.toString();
        QStringList addressParts = address.split('.');

        ModeDialog dialog;
        dialog.ui->spinBox_2->setValue(addressParts[0].toInt());
        dialog.ui->spinBox_2->setReadOnly(true);
        dialog.ui->spinBox_3->setValue(addressParts[1].toInt());
        dialog.ui->spinBox_3->setReadOnly(true);
        dialog.ui->spinBox_4->setValue(addressParts[2].toInt());
        dialog.ui->spinBox_4->setReadOnly(true);
        dialog.ui->spinBox_5->setValue(addressParts[3].toInt());
        dialog.ui->spinBox_5->setReadOnly(true);
        if (dialog.exec() == QDialog::Accepted)
        {
            delete this->connection;
            this->connection = nullptr;
            this->connection = new CheeseTcpConnection();
            bool success = this->connection->initTcpServer(static_cast<uint16_t>(dialog.ui->spinBox->value()));
            if (!success)
            {
                delete this->connection;
                this->connection = nullptr;
                return;
            }
            this->connectToConnection();
        }
        else
            return;
    }

    // set gaming
    this->gaming = true;

    // set color
    this->myCheeseColor = CheeseColor::red;
    this->currentStepColor = CheeseColor::red;

    // set shuai point
    this->redShuaiPoint = CheesePoint(9, 4);
    this->blackShuaiPoint = CheesePoint(0, 4);

    // set cheese
    for (auto &cheeseArray : this->cheeseTable)
        for (auto &cheese : cheeseArray)
        {
            delete cheese;
            cheese = nullptr;
        }

    // black
    cheeseTable[0][0] = new Cheese(CheeseColor::black, CheeseKind::che, 0, 0);
    cheeseTable[0][1] = new Cheese(CheeseColor::black, CheeseKind::ma, 0, 1);
    cheeseTable[0][2] = new Cheese(CheeseColor::black, CheeseKind::xiang, 0, 2);
    cheeseTable[0][3] = new Cheese(CheeseColor::black, CheeseKind::shi, 0, 3);
    cheeseTable[0][4] = new Cheese(CheeseColor::black, CheeseKind::shuai, 0, 4);
    cheeseTable[0][5] = new Cheese(CheeseColor::black, CheeseKind::shi, 0, 5);
    cheeseTable[0][6] = new Cheese(CheeseColor::black, CheeseKind::xiang, 0, 6);
    cheeseTable[0][7] = new Cheese(CheeseColor::black, CheeseKind::ma, 0, 7);
    cheeseTable[0][8] = new Cheese(CheeseColor::black, CheeseKind::che, 0, 8);
    cheeseTable[2][1] = new Cheese(CheeseColor::black, CheeseKind::pao, 2, 1);
    cheeseTable[2][7] = new Cheese(CheeseColor::black, CheeseKind::pao, 2, 7);
    cheeseTable[3][0] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 0);
    cheeseTable[3][2] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 2);
    cheeseTable[3][4] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 4);
    cheeseTable[3][6] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 6);
    cheeseTable[3][8] = new Cheese(CheeseColor::black, CheeseKind::bing, 3, 8);

    // red
    cheeseTable[9][0] = new Cheese(CheeseColor::red, CheeseKind::che, 9, 0);
    cheeseTable[9][1] = new Cheese(CheeseColor::red, CheeseKind::ma, 9, 1);
    cheeseTable[9][2] = new Cheese(CheeseColor::red, CheeseKind::xiang, 9, 2);
    cheeseTable[9][3] = new Cheese(CheeseColor::red, CheeseKind::shi, 9, 3);
    cheeseTable[9][4] = new Cheese(CheeseColor::red, CheeseKind::shuai, 9, 4);
    cheeseTable[9][5] = new Cheese(CheeseColor::red, CheeseKind::shi, 9, 5);
    cheeseTable[9][6] = new Cheese(CheeseColor::red, CheeseKind::xiang, 9, 6);
    cheeseTable[9][7] = new Cheese(CheeseColor::red, CheeseKind::ma, 9, 7);
    cheeseTable[9][8] = new Cheese(CheeseColor::red, CheeseKind::che, 9, 8);
    cheeseTable[7][1] = new Cheese(CheeseColor::red, CheeseKind::pao, 7, 1);
    cheeseTable[7][7] = new Cheese(CheeseColor::red, CheeseKind::pao, 7, 7);
    cheeseTable[6][0] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 0);
    cheeseTable[6][2] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 2);
    cheeseTable[6][4] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 4);
    cheeseTable[6][6] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 6);
    cheeseTable[6][8] = new Cheese(CheeseColor::red, CheeseKind::bing, 6, 8);

    emit modelChanged(this->cheeseTable, this->myCheeseColor);
    emit readySend(this->cheeseTable, this->currentStepColor);
    emit colorChanged(this->currentStepColor);
}

void CheeseModel::setPiecesModel()
{
    // set cheese
    for (auto &cheeseArray : this->cheeseTable)
        for (auto &cheese : cheeseArray)
        {
            delete cheese;
            cheese = nullptr;
        }

    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("打开残局文件"), ".", tr("文本文件 (*.txt)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot open file: ") + file.errorString());
        return;
    }

    // coodinate transform & cheesKind order transform
    QTextStream in(&file);
    QString colorString = in.readLine();
    if (colorString == "red") // 红方先手
    {
        for (int i = 0; i < 7; ++i)
        {
            QString tempBuffer = in.readLine();
            int cheeseNumber = QString(tempBuffer[0]).toInt();
            int x, y;
            for (int j = 0; j < cheeseNumber; ++j)
            {
                x = QString(tempBuffer[3 + 6 * j]).toInt();
                y = QString(tempBuffer[5 + 6 * j]).toInt();
                switch (i)
                {
                case 0:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::shuai, 9 - y, x);
                    this->redShuaiPoint = CheesePoint(9 - y, x);
                    break;
                case 1:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::shi, 9 - y, x);
                    break;
                case 2:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::xiang, 9 - y, x);
                    break;
                case 3:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::ma, 9 - y, x);
                    break;
                case 4:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::che, 9 - y, x);
                    break;
                case 5:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::pao, 9 - y, x);
                    break;
                case 6:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::bing, 9 - y, x);
                    break;
                }
            }
        }

        in.readLine(); // waste
        for (int i = 7; i < 14; ++i)
        {
            QString tempBuffer = in.readLine();
            int cheeseNumber = QString(tempBuffer[0]).toInt();
            int x, y;
            for (int j = 0; j < cheeseNumber; ++j)
            {
                x = QString(tempBuffer[3 + 6 * j]).toInt();
                y = QString(tempBuffer[5 + 6 * j]).toInt();
                switch (i)
                {
                case 7:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::shuai, 9 - y, x);
                    this->blackShuaiPoint = CheesePoint(9 - y, x);
                    break;
                case 8:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::shi, 9 - y, x);
                    break;
                case 9:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::xiang, 9 - y, x);
                    break;
                case 10:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::ma, 9 - y, x);
                    break;
                case 11:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::che, 9 - y, x);
                    break;
                case 12:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::pao, 9 - y, x);
                    break;
                case 13:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::bing, 9 - y, x);
                    break;
                }
            }
        }
    }
    else // 黑方先手
    {
        for (int i = 0; i < 7; ++i)
        {
            QString tempBuffer = in.readLine();
            int cheeseNumber = QString(tempBuffer[0]).toInt();
            int x, y;
            for (int j = 0; j < cheeseNumber; ++j)
            {
                x = QString(tempBuffer[3 + 6 * j]).toInt();
                y = QString(tempBuffer[5 + 6 * j]).toInt();
                switch (i)
                {
                case 0:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::shuai, 9 - y, x);
                    this->blackShuaiPoint = CheesePoint(9 - y, x);
                    break;
                case 1:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::shi, 9 - y, x);
                    break;
                case 2:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::xiang, 9 - y, x);
                    break;
                case 3:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::ma, 9 - y, x);
                    break;
                case 4:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::che, 9 - y, x);
                    break;
                case 5:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::pao, 9 - y, x);
                    break;
                case 6:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::black, CheeseKind::bing, 9 - y, x);
                    break;
                }
            }
        }

        in.readLine(); // waste
        for (int i = 7; i < 14; ++i)
        {
            QString tempBuffer = in.readLine();
            int cheeseNumber = QString(tempBuffer[0]).toInt();
            int x, y;
            for (int j = 0; j < cheeseNumber; ++j)
            {
                x = QString(tempBuffer[3 + 6 * j]).toInt();
                y = QString(tempBuffer[5 + 6 * j]).toInt();
                switch (i)
                {
                case 7:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::shuai, 9 - y, x);
                    this->redShuaiPoint = CheesePoint(9 - y, x);
                    break;
                case 8:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::shi, 9 - y, x);
                    break;
                case 9:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::xiang, 9 - y, x);
                    break;
                case 10:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::ma, 9 - y, x);
                    break;
                case 11:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::che, 9 - y, x);
                    break;
                case 12:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::pao, 9 - y, x);
                    break;
                case 13:
                    this->cheeseTable[9 - y][x] = new Cheese(CheeseColor::red, CheeseKind::bing, 9 - y, x);
                    break;
                }
            }
        }
    }

    file.close();

    // set mode
    QStringList modes;
    modes << tr("单人模式") << tr("单机对战模式") << tr("联机对战模式");
    bool ok;
    this->mode = QInputDialog::getItem(nullptr, tr("对战模式"), tr("选择对战模式"), modes, 1, false, &ok);
    if (!ok)
        return;

    // set network
    if (this->mode == tr("单机对战模式"))
    {
        this->mode = tr("对战模式");
        ModeDialog dialog;
        dialog.ui->spinBox_2->setValue(127);
        dialog.ui->spinBox_2->setReadOnly(true);
        dialog.ui->spinBox_3->setValue(0);
        dialog.ui->spinBox_3->setReadOnly(true);
        dialog.ui->spinBox_4->setValue(0);
        dialog.ui->spinBox_4->setReadOnly(true);
        dialog.ui->spinBox_5->setValue(1);
        dialog.ui->spinBox_5->setReadOnly(true);
        if (dialog.exec() == QDialog::Accepted)
        {
            delete this->connection;
            this->connection = nullptr;
            this->connection = new CheeseTcpConnection();
            bool success = this->connection->initTcpServer(static_cast<uint16_t>(dialog.ui->spinBox->value()));
            if (!success)
            {
                delete this->connection;
                this->connection = nullptr;
                return;
            }
            this->connectToConnection();
        }
        else
            return;
    }
    else if (this->mode == tr("联机对战模式"))
    {
        this->mode = tr("对战模式");
        QString address;
        for (QHostAddress add : QNetworkInterface::allAddresses())
            if (add.protocol() == QAbstractSocket::IPv4Protocol)
                address = add.toString();
        QStringList addressParts = address.split('.');

        ModeDialog dialog;
        dialog.ui->spinBox_2->setValue(addressParts[0].toInt());
        dialog.ui->spinBox_2->setReadOnly(true);
        dialog.ui->spinBox_3->setValue(addressParts[1].toInt());
        dialog.ui->spinBox_3->setReadOnly(true);
        dialog.ui->spinBox_4->setValue(addressParts[2].toInt());
        dialog.ui->spinBox_4->setReadOnly(true);
        dialog.ui->spinBox_5->setValue(addressParts[3].toInt());
        dialog.ui->spinBox_5->setReadOnly(true);
        if (dialog.exec() == QDialog::Accepted)
        {
            delete this->connection;
            this->connection = nullptr;
            this->connection = new CheeseTcpConnection();
            bool success = this->connection->initTcpServer(static_cast<uint16_t>(dialog.ui->spinBox->value()));
            if (!success)
            {
                delete this->connection;
                this->connection = nullptr;
                return;
            }
            this->connectToConnection();
        }
        else
            return;
    }

    // set gaming
    this->gaming = true;

    // set color
    this->myCheeseColor = CheeseColor::red;
    if (colorString == "red")
        this->currentStepColor = CheeseColor::red;
    else
        this->currentStepColor = CheeseColor::black;

    emit modelChanged(this->cheeseTable, this->myCheeseColor);
    emit readySend(this->cheeseTable, this->currentStepColor);
    emit colorChanged(this->currentStepColor);
}

void CheeseModel::setJoinModel()
{
    // set mode
    this->mode = tr("对战模式");

    // set network
    ModeDialog dialog;
    dialog.ui->spinBox_2->setValue(127);
    dialog.ui->spinBox_3->setValue(0);
    dialog.ui->spinBox_4->setValue(0);
    dialog.ui->spinBox_5->setValue(1);
    dialog.ui->spinBox->setValue(50000);
    if (dialog.exec() == QDialog::Accepted)
    {
        delete this->connection;
        this->connection = nullptr;
        this->connection = new CheeseTcpConnection();
        QString address = QString::number(dialog.ui->spinBox_2->value()) + '.' + QString::number(dialog.ui->spinBox_3->value()) + '.' + QString::number(dialog.ui->spinBox_4->value()) + '.' + QString::number(dialog.ui->spinBox_5->value());
        bool success = this->connection->initTcpClient(address, static_cast<uint16_t>(dialog.ui->spinBox->value()));
        if (!success)
        {
            delete this->connection;
            this->connection = nullptr;
            return;
        }
        this->connectToConnection();
    }
    else
        return;

    // set gaming
    this->gaming = true;

    // set color
    this->myCheeseColor = CheeseColor::black;
}

void CheeseModel::saveModel()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, tr("保存残局文件"), ".", tr("文本文件 (*.txt)"));
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(nullptr, "Error", "Cannot save file: " + file.errorString());
        return;
    }

    std::array<std::vector<CheesePoint>, 14> fileTemplate;
    // coodinate transform & cheesKind order transform
    switch (this->currentStepColor)
    {
    case CheeseColor::red: // 红方先手
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 9; ++j)
            {
                if (this->cheeseTable[i][j] == nullptr)
                    continue;
                switch (this->cheeseTable[i][j]->color)
                {
                case CheeseColor::red:
                    switch (this->cheeseTable[i][j]->kind)
                    {
                    case CheeseKind::shuai:
                        fileTemplate[0].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::shi:
                        fileTemplate[1].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::xiang:
                        fileTemplate[2].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::ma:
                        fileTemplate[3].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::che:
                        fileTemplate[4].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::pao:
                        fileTemplate[5].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::bing:
                        fileTemplate[6].push_back(CheesePoint(j, 9 - i));
                        break;
                    }
                    break;
                case CheeseColor::black:
                    switch (this->cheeseTable[i][j]->kind)
                    {
                    case CheeseKind::shuai:
                        fileTemplate[7].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::shi:
                        fileTemplate[8].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::xiang:
                        fileTemplate[9].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::ma:
                        fileTemplate[10].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::che:
                        fileTemplate[11].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::pao:
                        fileTemplate[12].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::bing:
                        fileTemplate[13].push_back(CheesePoint(j, 9 - i));
                        break;
                    }
                    break;
                }
            }
        break;
    case CheeseColor::black: // 黑方先手
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 9; ++j)
            {
                if (this->cheeseTable[i][j] == nullptr)
                    continue;
                switch (this->cheeseTable[i][j]->color)
                {
                case CheeseColor::red:
                    switch (this->cheeseTable[i][j]->kind)
                    {
                    case CheeseKind::shuai:
                        fileTemplate[7].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::shi:
                        fileTemplate[8].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::xiang:
                        fileTemplate[9].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::ma:
                        fileTemplate[10].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::che:
                        fileTemplate[11].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::pao:
                        fileTemplate[12].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::bing:
                        fileTemplate[13].push_back(CheesePoint(j, 9 - i));
                        break;
                    }
                    break;
                case CheeseColor::black:
                    switch (this->cheeseTable[i][j]->kind)
                    {
                    case CheeseKind::shuai:
                        fileTemplate[0].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::shi:
                        fileTemplate[1].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::xiang:
                        fileTemplate[2].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::ma:
                        fileTemplate[3].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::che:
                        fileTemplate[4].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::pao:
                        fileTemplate[5].push_back(CheesePoint(j, 9 - i));
                        break;
                    case CheeseKind::bing:
                        fileTemplate[6].push_back(CheesePoint(j, 9 - i));
                        break;
                    }
                    break;
                }
            }
        break;
    }

    // save pieces file
    QTextStream out(&file);
    switch (this->currentStepColor)
    {
    case CheeseColor::red:
        out << "red\n";
        for (int i = 0; i < 7; ++i)
        {
            out << fileTemplate[i].size() << " ";
            for (const auto &cheesePoint : fileTemplate[i])
                out << "<" << cheesePoint.row << "," << cheesePoint.column << ">"
                    << " ";
            out << "\n";
        }
        out << "black\n";
        for (int i = 7; i < 14; ++i)
        {
            out << fileTemplate[i].size() << " ";
            for (const auto &cheesePoint : fileTemplate[i])
                out << "<" << cheesePoint.row << "," << cheesePoint.column << ">"
                    << " ";
            out << "\n";
        }
        break;
    case CheeseColor::black:
        out << "black\n";
        for (int i = 0; i < 7; ++i)
        {
            out << fileTemplate[i].size() << " ";
            for (const auto &cheesePoint : fileTemplate[i])
                out << "<" << cheesePoint.row << "," << cheesePoint.column << ">"
                    << " ";
            out << "\n";
        }
        out << "red\n";
        for (int i = 7; i < 14; ++i)
        {
            out << fileTemplate[i].size() << " ";
            for (const auto &cheesePoint : fileTemplate[i])
                out << "<" << cheesePoint.row << "," << cheesePoint.column << ">"
                    << " ";
            out << "\n";
        }
        break;
    }

    file.close();
}

void CheeseModel::setLoseGame()
{
    if (!this->gaming)
        return;
    if (this->mode == "单人模式")
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            this->endGame(CheeseColor::black);
            break;
        case CheeseColor::black:
            this->endGame(CheeseColor::red);
            break;
        }
    else // 多人模式
        switch (this->myCheeseColor)
        {
        case CheeseColor::red:
            emit readySend(CheeseColor::black);
            this->endGame(CheeseColor::black);
            break;
        case CheeseColor::black:
            emit readySend(CheeseColor::red);
            this->endGame(CheeseColor::red);
            break;
        }
}

void CheeseModel::setEndGame()
{
    if (!this->gaming)
        return;

    if (this->mode == "单人模式")
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            break;
        case CheeseColor::black:
            break;
        }
    else // 多人模式
    {
        if (this->myCheeseColor == this->currentStepColor)
            switch (this->myCheeseColor)
            {
            case CheeseColor::red:
                emit readySend(CheeseColor::black);
                this->endGame(CheeseColor::black);
                break;
            case CheeseColor::black:
                emit readySend(CheeseColor::red);
                this->endGame(CheeseColor::red);
                break;
            }
    }
}

void CheeseModel::receiveMousePress(CheesePoint cheesePoint)
{
    assert(cheesePoint.row >= 0 && cheesePoint.row <= 9 && cheesePoint.column >= 0 && cheesePoint.column <= 8);
    if (this->mode == tr("对战模式"))
        if (myCheeseColor != currentStepColor)
            return;
    if (this->cheeseChosenPoint.row != -1)         // There has been a chosen cheese
        if (cheeseNextPoint.contains(cheesePoint)) // Can go to cheesePoint
        {
            // Chosen cheese go to cheesePoint
            this->goCheese(this->cheeseChosenPoint, cheesePoint);
            emit readySend(this->cheeseChosenPoint, cheesePoint);
            // next step
            this->enterNextStep();
            // Clear chosen cheese
            this->clearChosenCheese();
        }
        else // Cannot go to cheesePoint
        {
            if (cheeseTable[cheesePoint.row][cheesePoint.column] != nullptr)                           // There is a cheese at cheesePoint
                if (cheeseTable[cheesePoint.row][cheesePoint.column]->color == this->currentStepColor) // Proper cheese color at cheesePoint
                    this->chooseCheese(cheesePoint);
                else // Not proper cheese color at cheesePoint
                    this->clearChosenCheese();
            else // There is not a cheese at cheesePoint
                this->clearChosenCheese();
        }
    else // There has not been a chosen cheese
    {
        if (cheeseTable[cheesePoint.row][cheesePoint.column] != nullptr)                           // There is a cheese at cheesePoint
            if (cheeseTable[cheesePoint.row][cheesePoint.column]->color == this->currentStepColor) // Proper cheese color at cheesePoint
                this->chooseCheese(cheesePoint);
    }
}

void CheeseModel::receiveMousePress() // clear chosen point
{
    // cheesePoint not OK
    this->clearChosenCheese();
}

void CheeseModel::receiveRecv(std::array<std::array<Cheese *, 9>, 10> changedCheese, CheeseColor currentColor)
{
    // set cheese
    this->cheeseTable = changedCheese;

    // set color
    this->currentStepColor = currentColor;

    // set shuai
    for (int i = 0; i <= 2; ++i)
        for (int j = 3; j <= 5; ++j)
        {
            if (changedCheese[i][j] == nullptr)
                continue;
            if (changedCheese[i][j]->kind == CheeseKind::shuai)
                if (changedCheese[i][j]->color == CheeseColor::black)
                    this->blackShuaiPoint = CheesePoint(i, j);
        }
    for (int i = 7; i <= 9; ++i)
        for (int j = 3; j <= 5; ++j)
        {
            if (changedCheese[i][j] == nullptr)
                continue;
            if (changedCheese[i][j]->kind == CheeseKind::shuai)
                if (changedCheese[i][j]->color == CheeseColor::red)
                    this->redShuaiPoint = CheesePoint(i, j);
        }
    emit modelChanged(this->cheeseTable, this->myCheeseColor);
    emit colorChanged(this->currentStepColor);
}

void CheeseModel::receiveRecv(CheesePoint startCheesePoint, CheesePoint endCheesePoint)
{
    // go cheese
    this->goCheese(startCheesePoint, endCheesePoint);
    // next step
    this->enterNextStep();
}

void CheeseModel::receiveRecv(CheeseColor winColor)
{
    this->endGame(winColor);
}

void CheeseModel::chooseCheese(const CheesePoint &cheesePoint)
{
    // Chose cheese at cheesePoint
    this->cheeseChosenPoint = cheesePoint;
    this->chooseCheeseNextPoint(cheesePoint);
}

void CheeseModel::clearChosenCheese()
{
    // Clear chosen cheese
    this->cheeseChosenPoint.row = this->cheeseChosenPoint.column = -1;
    this->cheeseNextPoint.clear();
}

void CheeseModel::chooseCheeseNextPoint(const CheesePoint &cheesePoint)
{
    int row = cheesePoint.row;
    int column = cheesePoint.column;
    switch (cheeseTable[row][column]->kind)
    {
    case CheeseKind::che:
        for (int i = row - 1; i >= 0; --i) // upward
        {
            if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
            {
                if (cheeseTable[i][column]->color != this->currentStepColor) // Can eat cheese in the way
                    this->cheeseNextPoint.insert(CheesePoint(i, column));
                break;
            }
            this->cheeseNextPoint.insert(CheesePoint(i, column));
        }
        for (int i = row + 1; i <= 9; ++i) // downward
        {
            if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
            {
                if (cheeseTable[i][column]->color != this->currentStepColor) // Can eat cheese in the way
                    this->cheeseNextPoint.insert(CheesePoint(i, column));
                break;
            }
            this->cheeseNextPoint.insert(CheesePoint(i, column));
        }
        for (int j = column - 1; j >= 0; --j) // leftward
        {
            if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
            {
                if (cheeseTable[row][j]->color != this->currentStepColor) // Can eat cheese in the way
                    this->cheeseNextPoint.insert(CheesePoint(row, j));
                break;
            }
            this->cheeseNextPoint.insert(CheesePoint(row, j));
        }
        for (int j = column + 1; j <= 8; ++j) // rightward
        {
            if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
            {
                if (cheeseTable[row][j]->color != this->currentStepColor) // Can eat cheese in the way
                    this->cheeseNextPoint.insert(CheesePoint(row, j));
                break;
            }
            this->cheeseNextPoint.insert(CheesePoint(row, j));
        }
        break;
    case CheeseKind::ma:
        for (int i = 0; i < 8; ++i)
        {
            int newRow = row + MA_POINT_DELTA[i].row;
            int newColumn = column + MA_POINT_DELTA[i].column;
            if (newRow >= 0 && newRow <= 9 && newColumn >= 0 && newColumn <= 8)                                                   // inside border
                if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                    if (cheeseTable[row + MA_BIE_POINT_DELTA[i].row][column + MA_BIE_POINT_DELTA[i].column] == nullptr)           // 不别马腿
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
        }
        break;
    case CheeseKind::pao:
        for (int i = row - 1, inTheWay = 0; i >= 0; --i) // upward
            if (inTheWay == 0)
            {
                if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
                {
                    inTheWay = 1;
                    continue;
                }
                this->cheeseNextPoint.insert(CheesePoint(i, column));
            }
            else
            {
                if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
                {
                    if (cheeseTable[i][column]->color != this->currentStepColor) // Can eat cheese in the way
                        this->cheeseNextPoint.insert(CheesePoint(i, column));
                    break;
                }
            }
        for (int i = row + 1, inTheWay = 0; i <= 9; ++i) // downward
            if (inTheWay == 0)
            {
                if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
                {
                    inTheWay = 1;
                    continue;
                }
                this->cheeseNextPoint.insert(CheesePoint(i, column));
            }
            else
            {
                if (cheeseTable[i][column] != nullptr) // There is a cheese in the way
                {
                    if (cheeseTable[i][column]->color != this->currentStepColor) // Can eat cheese in the way
                        this->cheeseNextPoint.insert(CheesePoint(i, column));
                    break;
                }
            }
        for (int j = column - 1, inTheWay = 0; j >= 0; --j) // leftward
            if (inTheWay == 0)
            {
                if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
                {
                    inTheWay = 1;
                    continue;
                }
                this->cheeseNextPoint.insert(CheesePoint(row, j));
            }
            else
            {
                if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
                {
                    if (cheeseTable[row][j]->color != this->currentStepColor) // Can eat cheese in the way
                        this->cheeseNextPoint.insert(CheesePoint(row, j));
                    break;
                }
            }
        for (int j = column + 1, inTheWay = 0; j <= 8; ++j) // rightward
            if (inTheWay == 0)
            {
                if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
                {
                    inTheWay = 1;
                    continue;
                }
                this->cheeseNextPoint.insert(CheesePoint(row, j));
            }
            else
            {
                if (cheeseTable[row][j] != nullptr) // There is a cheese in the way
                {
                    if (cheeseTable[row][j]->color != this->currentStepColor) // Can eat cheese in the way
                        this->cheeseNextPoint.insert(CheesePoint(row, j));
                    break;
                }
            }
        break;
    case CheeseKind::xiang:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + XIANG_POINT_DELTA[i].row;
                int newColumn = column + XIANG_POINT_DELTA[i].column;
                if (newRow >= 5 && newRow <= 9 && newColumn >= 0 && newColumn <= 8)                                                   // inside red xiang border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        if (cheeseTable[row + XIANG_BIE_POINT_DELTA[i].row][column + XIANG_BIE_POINT_DELTA[i].column] == nullptr)     // 不塞象眼
                            this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        case CheeseColor::black:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + XIANG_POINT_DELTA[i].row;
                int newColumn = column + XIANG_POINT_DELTA[i].column;
                if (newRow >= 0 && newRow <= 4 && newColumn >= 0 && newColumn <= 8)                                                   // inside black xiang border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        if (cheeseTable[row + XIANG_BIE_POINT_DELTA[i].row][column + XIANG_BIE_POINT_DELTA[i].column] == nullptr)     // 不塞象眼
                            this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        }
        break;
    case CheeseKind::shi:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + SHI_POINT_DELTA[i].row;
                int newColumn = column + SHI_POINT_DELTA[i].column;
                if (newRow >= 7 && newRow <= 9 && newColumn >= 3 && newColumn <= 5)                                                   // inside red shi border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        case CheeseColor::black:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + SHI_POINT_DELTA[i].row;
                int newColumn = column + SHI_POINT_DELTA[i].column;
                if (newRow >= 0 && newRow <= 2 && newColumn >= 3 && newColumn <= 5)                                                   // inside black shi border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            break;
        }
        break;
    case CheeseKind::shuai:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + SHUAI_POINT_DELTA[i].row;
                int newColumn = column + SHUAI_POINT_DELTA[i].column;
                if (newRow >= 7 && newRow <= 9 && newColumn >= 3 && newColumn <= 5)                                                   // inside red shuai border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            if (column == this->blackShuaiPoint.column)
            {
                bool shuaiConflict = true;
                for (int i = this->blackShuaiPoint.row + 1; i < row; ++i)
                    if (this->cheeseTable[i][column] != nullptr)
                    {
                        shuaiConflict = false;
                        break;
                    }
                if (shuaiConflict)
                    this->cheeseNextPoint.insert(this->blackShuaiPoint);
            }
            break;
        case CheeseColor::black:
            for (int i = 0; i < 4; ++i)
            {
                int newRow = row + SHUAI_POINT_DELTA[i].row;
                int newColumn = column + SHUAI_POINT_DELTA[i].column;
                if (newRow >= 0 && newRow <= 2 && newColumn >= 3 && newColumn <= 5)                                                   // inside black shuai border
                    if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                        this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
            }
            if (column == this->redShuaiPoint.column)
            {
                bool shuaiConflict = true;
                for (int i = row + 1; i < this->redShuaiPoint.row; ++i)
                    if (this->cheeseTable[i][column] != nullptr)
                    {
                        shuaiConflict = false;
                        break;
                    }
                if (shuaiConflict)
                    this->cheeseNextPoint.insert(this->redShuaiPoint);
            }
            break;
        }
        break;
    case CheeseKind::bing:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            if (row >= 5) // 未过河
            {
                int newRow = row - 1;
                if (cheeseTable[newRow][column] == nullptr || cheeseTable[newRow][column]->color != this->currentStepColor) // Can go there
                    this->cheeseNextPoint.insert(CheesePoint(newRow, column));
            }
            else // 已过河
            {
                for (int i = 0; i < 3; ++i)
                {
                    int newRow = row + BING_RED_POINT_DELTA[i].row;
                    int newColumn = column + BING_RED_POINT_DELTA[i].column;
                    if (newRow >= 0 && newColumn >= 0 && newColumn <= 8)                                                                  // inside border
                        if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                            this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
                }
            }
            break;
        case CheeseColor::black:
            if (row <= 4) // 未过河
            {
                int newRow = row + 1;
                if (cheeseTable[newRow][column] == nullptr || cheeseTable[newRow][column]->color != this->currentStepColor) // Can go there
                    this->cheeseNextPoint.insert(CheesePoint(newRow, column));
            }
            else // 已过河
            {
                for (int i = 0; i < 3; ++i)
                {
                    int newRow = row + BING_BLACK_POINT_DELTA[i].row;
                    int newColumn = column + BING_BLACK_POINT_DELTA[i].column;
                    if (newRow <= 9 && newColumn >= 0 && newColumn <= 8)                                                                  // inside border
                        if (cheeseTable[newRow][newColumn] == nullptr || cheeseTable[newRow][newColumn]->color != this->currentStepColor) // Can go there
                            this->cheeseNextPoint.insert(CheesePoint(newRow, newColumn));
                }
            }
            break;
        }
        break;
    }
}

void CheeseModel::goCheese(CheesePoint startCheesePoint, CheesePoint endCheesePoint)
{
    // specific: shuai
    // go shuai
    if (this->cheeseTable[startCheesePoint.row][startCheesePoint.column]->kind == CheeseKind::shuai)
        switch (this->cheeseTable[startCheesePoint.row][startCheesePoint.column]->color)
        {
        case CheeseColor::red:
            this->redShuaiPoint = endCheesePoint;
            break;
        case CheeseColor::black:
            this->blackShuaiPoint = endCheesePoint;
            break;
        }

    // eat shuai
    bool end = false;
    CheeseColor winColor;
    if (this->cheeseTable[endCheesePoint.row][endCheesePoint.column] != nullptr)
        if (this->cheeseTable[endCheesePoint.row][endCheesePoint.column]->kind == CheeseKind::shuai)
        {
            end = true;
            switch (this->cheeseTable[endCheesePoint.row][endCheesePoint.column]->color)
            {
            case CheeseColor::red:
                winColor = CheeseColor::black;
                break;
            case CheeseColor::black:
                winColor = CheeseColor::red;
                break;
            }
        }

    // go
    delete cheeseTable[endCheesePoint.row][endCheesePoint.column];
    cheeseTable[endCheesePoint.row][endCheesePoint.column] = cheeseTable[startCheesePoint.row][startCheesePoint.column];
    cheeseTable[startCheesePoint.row][startCheesePoint.column] = nullptr;
    emit modelChanged(startCheesePoint, endCheesePoint);

    // going to eat shuai
    this->checkJiangJun();

    // game end
    if (end)
    {
        emit readySend(winColor);
        this->endGame(winColor);
    }
}

void CheeseModel::enterNextStep()
{
    switch (this->currentStepColor)
    {
    case CheeseColor::red:
        this->currentStepColor = CheeseColor::black;
        break;
    case CheeseColor::black:
        this->currentStepColor = CheeseColor::red;
        break;
    }
    emit colorChanged(this->currentStepColor);
}

void CheeseModel::checkJiangJun()
{
    bool jiangJun = false;
    switch (this->currentStepColor)
    {
    case CheeseColor::red:
        for (int i = 0; i < 10 && !jiangJun; ++i)
            for (int j = 0; j < 9 && !jiangJun; ++j)
                if (this->cheeseTable[i][j] != nullptr && this->cheeseTable[i][j]->color == CheeseColor::red)
                    if (this->checkGoable(CheesePoint(i, j), this->blackShuaiPoint))
                        jiangJun = true;
        break;
    case CheeseColor::black:
        for (int i = 0; i < 10 && !jiangJun; ++i)
            for (int j = 0; j < 9 && !jiangJun; ++j)
                if (this->cheeseTable[i][j] != nullptr && this->cheeseTable[i][j]->color == CheeseColor::black)
                    if (this->checkGoable(CheesePoint(i, j), this->redShuaiPoint))
                        jiangJun = true;
        break;
    }

    if (jiangJun) // sound
        jiangJunPlayer->play();
}

void CheeseModel::endGame(CheeseColor winColor)
{
    if (this->mode == "单人模式")
        switch (winColor)
        {
        case CheeseColor::red:
            QMessageBox::information(nullptr, "Game End", "Red Win!");
            break;
        case CheeseColor::black:
            QMessageBox::information(nullptr, "Game End", "Black Win!");
            break;
        }
    else // 多人模式
        switch (winColor)
        {
        case CheeseColor::red:
            if (winColor == this->myCheeseColor)
                QMessageBox::information(nullptr, "Game End", "You Red Win!");
            else
                QMessageBox::information(nullptr, "Game End", "You Black Lose!");
            break;
        case CheeseColor::black:
            if (winColor == this->myCheeseColor)
                QMessageBox::information(nullptr, "Game End", "You Black Win!");
            else
                QMessageBox::information(nullptr, "Game End", "You Red Lose!");
            break;
        }

    // set mode
    this->mode = QString();

    // set network
    delete this->connection;

    // set gaming
    this->gaming = false;

    // set cheese
    for (auto &cheeseArray : this->cheeseTable)
        for (auto &cheese : cheeseArray)
        {
            delete cheese;
            cheese = nullptr;
        }

    // set shuai point
    this->redShuaiPoint = CheesePoint();
    this->blackShuaiPoint = CheesePoint();

    emit gameEnded();
}

void CheeseModel::connectToConnection()
{
    // model & connection
    QObject::connect(this, SIGNAL(readySend(const std::array<std::array<Cheese *, 9>, 10> &, CheeseColor)),
                     this->connection, SLOT(send(const std::array<std::array<Cheese *, 9>, 10> &, CheeseColor)));
    QObject::connect(this, SIGNAL(readySend(CheesePoint, CheesePoint)), this->connection, SLOT(send(CheesePoint, CheesePoint)));
    QObject::connect(this, SIGNAL(readySend(CheeseColor)), this->connection, SLOT(send(CheeseColor)));
    QObject::connect(this->connection, SIGNAL(recvChanged(std::array<std::array<Cheese *, 9>, 10>, CheeseColor)),
                     this, SLOT(receiveRecv(std::array<std::array<Cheese *, 9>, 10>, CheeseColor)));
    QObject::connect(this->connection, SIGNAL(recvChanged(CheesePoint, CheesePoint)), this, SLOT(receiveRecv(CheesePoint, CheesePoint)));
    QObject::connect(this->connection, SIGNAL(recvChanged(CheeseColor)), this, SLOT(receiveRecv(CheeseColor)));
}

bool CheeseModel::checkGoable(const CheesePoint &startCheesePoint, const CheesePoint &endCheesePoint)
{
    int startRow = startCheesePoint.row;
    int startColumn = startCheesePoint.column;
    int endRow = endCheesePoint.row;
    int endColumn = endCheesePoint.column;
    int deltaRow = endRow - startRow;
    int deltaColumn = endColumn - startColumn;

    if (cheeseTable[endRow][endColumn] != nullptr && cheeseTable[endRow][endColumn]->color == this->currentStepColor) // Cannot eat cheese in the way
        return false;

    switch (cheeseTable[startRow][startColumn]->kind)
    {
    case CheeseKind::che:
        if (deltaRow == 0)
            if (deltaColumn > 0)
            {
                for (int j = startColumn + 1; j < endColumn; ++j)
                    if (cheeseTable[startRow][j] != nullptr)
                        return false;
            }
            else if (deltaColumn < 0)
            {
                for (int j = endColumn + 1; j < startColumn; ++j)
                    if (cheeseTable[startRow][j] != nullptr)
                        return false;
                return true;
            }
            else
                return false;
        else if (deltaColumn == 0)
            if (deltaRow > 0)
            {
                for (int i = startRow + 1; i < endRow; ++i)
                    if (cheeseTable[i][startColumn] != nullptr)
                        return false;
                return true;
            }
            else if (deltaRow < 0)
            {
                for (int i = endRow + 1; i < startRow; ++i)
                    if (cheeseTable[i][startColumn] != nullptr)
                        return false;
                return true;
            }
            else
                return false;
        else
            return false;
        break;
    case CheeseKind::ma:
        switch (deltaRow)
        {
        case 2:
            switch (deltaColumn)
            {
            case 1:
            case -1:
                if (cheeseTable[startRow + 1][startColumn] == nullptr) // 不别马腿
                    return true;
                else
                    return false;
                break;
            default:
                return false;
                break;
            }
            break;
        case -2:
            switch (deltaColumn)
            {
            case 1:
            case -1:
                if (cheeseTable[startRow - 1][startColumn] == nullptr) // 不别马腿
                    return true;
                else
                    return false;
                break;
            default:
                return false;
                break;
            }
            break;
        case 1:
        case -1:
            switch (deltaColumn)
            {
            case 2:
                if (cheeseTable[startRow][startColumn + 1] == nullptr) // 不别马腿
                    return true;
                else
                    return false;
                break;
            case -2:
                if (cheeseTable[startRow][startColumn - 1] == nullptr) // 不别马腿
                    return true;
                else
                    return false;
                break;
            default:
                return false;
                break;
            }
            break;
        default:
            return false;
            break;
        }
        break;
    case CheeseKind::pao:
        if (deltaRow == 0)
            if (deltaColumn > 0)
            {
                int barrier = 0;
                for (int j = startColumn + 1; j < endColumn; ++j)
                    if (cheeseTable[startRow][j] != nullptr)
                        ++barrier;
                if (barrier == 0 && cheeseTable[endRow][endColumn] == nullptr)
                    return true;
                else if (barrier == 1)
                    return true;
                else
                    return false;
            }
            else if (deltaColumn < 0)
            {
                int barrier = 0;
                for (int j = endColumn + 1; j < startColumn; ++j)
                    if (cheeseTable[startRow][j] != nullptr)
                        ++barrier;
                if (barrier == 0 && cheeseTable[endRow][endColumn] == nullptr)
                    return true;
                else if (barrier == 1)
                    return true;
                else
                    return false;
            }
            else
                return false;
        else if (deltaColumn == 0)
            if (deltaRow > 0)
            {
                int barrier = 0;
                for (int i = startRow + 1; i < endRow; ++i)
                    if (cheeseTable[i][startColumn] != nullptr)
                        ++barrier;
                if (barrier == 0 && cheeseTable[endRow][endColumn] == nullptr)
                    return true;
                else if (barrier == 1)
                    return true;
                else
                    return false;
            }
            else if (deltaRow < 0)
            {
                int barrier = 0;
                for (int i = endRow + 1; i < startRow; ++i)
                    if (cheeseTable[i][startColumn] != nullptr)
                        ++barrier;
                if (barrier == 0 && cheeseTable[endRow][endColumn] == nullptr)
                    return true;
                else if (barrier == 1)
                    return true;
                else
                    return false;
            }
            else
                return false;
        else
            return false;
        break;
    case CheeseKind::xiang:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            if (endRow <= 4) // outside red xiang border
                return false;
            break;
        case CheeseColor::black:
            if (endRow >= 5) // outside black xiang border
                return false;
            break;
        }
        switch (deltaRow)
        {
        case 2:
            switch (deltaColumn)
            {
            case 2:
                if (cheeseTable[startRow + 1][startColumn + 1] == nullptr) // 不塞象眼
                    return true;
                else
                    return false;
                break;
            case -2:
                if (cheeseTable[startRow + 1][startColumn - 1] == nullptr) // 不塞象眼
                    return true;
                else
                    return false;
                break;
            default:
                return false;
                break;
            }
            break;
        case -2:
            switch (deltaColumn)
            {
            case 2:
                if (cheeseTable[startRow - 1][startColumn + 1] == nullptr) // 不塞象眼
                    return true;
                else
                    return false;
                break;
            case -2:
                if (cheeseTable[startRow - 1][startColumn - 1] == nullptr) // 不塞象眼
                    return true;
                else
                    return false;
                break;
            default:
                return false;
                break;
            }
            break;
        default:
            return false;
            break;
        }
        break;
    case CheeseKind::shi:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            if (endRow <= 6 || endColumn <= 2 || endColumn >= 6) // outside red shi border
                return false;
            break;
        case CheeseColor::black:
            if (endRow >= 3 || endColumn <= 2 || endColumn >= 6) // outside black shi border
                return false;
            break;
        }
        switch (deltaRow)
        {
        case 1:
        case -1:
            switch (deltaColumn)
            {
            case 1:
            case -1:
                return true;
                break;
            default:
                return false;
                break;
            }
            break;
        default:
            return false;
            break;
        }
        break;
    case CheeseKind::shuai:
        // no need to consider shuai conflict
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            if (endRow <= 6 || endColumn <= 2 || endColumn >= 6) // outside red shuai border
                return false;
            break;
        case CheeseColor::black:
            if (endRow >= 3 || endColumn <= 2 || endColumn >= 6) // outside black shuai border
                return false;
            break;
        }
        switch (deltaRow)
        {
        case 1:
        case -1:
            switch (deltaColumn)
            {
            case 0:
                return true;
                break;
            default:
                return false;
                break;
            }
        case 0:
            switch (deltaColumn)
            {
            case 1:
            case -1:
                return true;
                break;
            default:
                return false;
                break;
            }
        default:
            return false;
            break;
        }
        break;
    case CheeseKind::bing:
        switch (this->currentStepColor)
        {
        case CheeseColor::red:
            if (startRow >= 5) // 未过河
                if (deltaRow == -1 && deltaColumn == 0)
                    return true;
                else
                    return false;
            else // 已过河
            {
                switch (deltaRow)
                {
                case -1:
                    switch (deltaColumn)
                    {
                    case 0:
                        return true;
                        break;
                    default:
                        return false;
                        break;
                    }
                    break;
                case 0:
                    switch (deltaColumn)
                    {
                    case 1:
                    case -1:
                        return true;
                        break;
                    default:
                        return false;
                        break;
                    }
                    break;
                default:
                    return false;
                    break;
                }
            }
            break;
        case CheeseColor::black:
            if (startRow <= 4) // 未过河
            {
                if (deltaRow == 1 && deltaColumn == 0)
                    return true;
                else
                    return false;
            }
            else // 已过河
            {
                switch (deltaRow)
                {
                case 1:
                    switch (deltaColumn)
                    {
                    case 0:
                        return true;
                        break;
                    default:
                        return false;
                        break;
                    }
                    break;
                case 0:
                    switch (deltaColumn)
                    {
                    case 1:
                    case -1:
                        return true;
                        break;
                    default:
                        return false;
                        break;
                    }
                    break;
                default:
                    return false;
                    break;
                }
            }
            break;
        }
        break;
    }
    return false;
}
