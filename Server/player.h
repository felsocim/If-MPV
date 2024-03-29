/**
  *Marek Felsoci, Arnaud Pinsun
  *
  **/
#ifndef PLAYER_H
#define PLAYER_H
#define GET_PERCET_POS_PROPERTY_REQUEST_ID 100

#include <iostream>
#include <string>
#include <QObject>
#include <QThread>
#include <QProcess>
#include <QStringList>
#include <QLocalSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <QDataStream>
#include <QByteArray>
#include <QtConcurrent/QtConcurrent>
#include "shared.h"

class Player : public QThread
{
    Q_OBJECT
private:
    QString player;
    QLocalSocket * mpv;
    void listener();
    bool running;
    QFuture<void> listening;
protected:
    void run();
public:
    explicit Player(QString serverName, QObject * parent = NULL);
    ~Player();
    void sendCommand(kCommand command, QJsonArray parameters);
    void observeProperty(int property);
    void setRunning(bool value);
    bool getRunning();
signals:
    void response(QJsonObject object);
    void observer(int property, QJsonValue value);
};

#endif // PLAYER_H
