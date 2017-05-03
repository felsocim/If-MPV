#ifndef INTERFACE_H
#define INTERFACE_H

#include <QMainWindow>
#include <QDebug>
#include <QJsonArray>
#include <QFileDialog>
#include <QDirIterator>
#include <QStringList>
#include <QFileInfo>
#include <QLocalServer>
#include <QLocalSocket>
#include <QList>
#include "player.h"
#include "shared.h"

namespace Ui
{
    class Interface;
}

class Interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit Interface(QWidget *parent = 0);
    ~Interface();
    void launchServer();

private:
    Ui::Interface *ui;
    Player * player;
    bool nowPlaying;
    QString currentMusicFolder;
    QStringList * musicFileList;
    QStringList * playlists;
    QLocalServer * server;
    QLocalSocket * client;

private slots:
    void startPlayerInstance();
    void selectMusicFolder();
    void terminate();
    void musicDiscovery();
    void receiver(QJsonObject object);
    void propertyStatus(int property, QJsonValue value);
    void incomingConnection();
    void listen();
};

#endif // INTERFACE_H
