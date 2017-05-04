#ifndef REMOTECONTROLLER_H
#define REMOTECONTROLLER_H

#include <iostream>
#include <QMainWindow>
#include <QLocalSocket>
#include <QtConcurrent/QtConcurrent>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <QDataStream>
#include <QByteArray>
#include <QStringList>
#include "../Server/shared.h"

namespace Ui {
class RemoteController;
}

class RemoteController : public QMainWindow
{
    Q_OBJECT

public:
    explicit RemoteController(QWidget *parent = 0);
    ~RemoteController();

private:
    Ui::RemoteController *ui;
    QLocalSocket * me;
    void joinReceiver();
    void sendCommand(kCommand command, QJsonArray parameters);
private slots:
    void listener();
    void onPlayPause();
    void showNowPlaying();
    void showAllMusic();
};

#endif // REMOTECONTROLLER_H
