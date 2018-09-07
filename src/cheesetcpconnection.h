#ifndef CHEESE_TCP_CONNECTION_H_
#define CHEESE_TCP_CONNECTION_H_

#include <QtCore/QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtWidgets/QProgressDialog>
#include <QtCore/QTimer>
#include <cheese.h>

class CheeseTcpConnection : public QObject
{
  Q_OBJECT

public:
  explicit CheeseTcpConnection(QObject *parent = nullptr);
  ~CheeseTcpConnection();
  bool initTcpServer(uint16_t port);
  bool initTcpClient(const QString &serverAddress, uint16_t port);

signals:
  void recvChanged(std::array<std::array<Cheese *, 9>, 10> changedCheese, CheeseColor currentColor);
  void recvChanged(CheesePoint startCheesePoint, CheesePoint endCheesePoint);
  void recvChanged(CheeseColor);

public slots:
  void send(const std::array<std::array<Cheese *, 9>, 10> &changedCheese, CheeseColor currentColor);
  void send(CheesePoint startCheesePoint, CheesePoint endCheesePoint);
  void send(CheeseColor winColor);
  void send(QByteArray byteArray);

private slots:
  void serverAcceptConnection();
  void recv();

private:
  QTcpServer *server = nullptr;
  QTcpSocket *socket = nullptr;
};

class ServerWaitProgress : public QObject
{
  Q_OBJECT

public:
  explicit ServerWaitProgress(QProgressDialog *dialog, QObject *parent = nullptr);
  bool progressIsEnded() { return this->progressEnded; }

public slots:
  void serverWaitProgressContinue();
  void serverWaitProgressEnd();

private:
  QProgressDialog *dialog;
  int progress = 0;
  bool progressEnded = false;
};

#endif // CHEESE_TCP_CONNECTION_H_