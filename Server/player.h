#ifndef PLAYER_H
#define PLAYER_H

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
#include <QByteArray>
#include "shared.h"

class Player : public QThread
{
    Q_OBJECT
private:
    QString player;
    QString file;
    QLocalSocket * mpv;
protected:
    void run();
public:
    explicit Player(QString serverName, QString fileName, QObject * parent = NULL);
    ~Player();
    void sendCommand(kCommand command, QJsonArray parameters);
};

#endif // PLAYER_H
