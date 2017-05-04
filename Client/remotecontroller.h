#ifndef REMOTECONTROLLER_H
#define REMOTECONTROLLER_H

#include <iostream>
#include <QMainWindow>
#include <QLocalSocket>
#include <QtConcurrent/QtConcurrent>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <QDataStream>
#include <QByteArray>
#include <QFileDialog>
#include <QStringList>
#include <QModelIndex>
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
    void majAutomate(phase p);
    void setNomFichier(QString s);
    void setNomPlaylist(QString s);
    QString nomFichier;
    QString nomPlaylist;
    bool isPlaylist;
    bool isPlaying;
    bool isShuffle;
    bool isMute;
private slots:
    void listener();
    void onPlayPause();
    void showNowPlaying();
    void showAllMusic();
    void showPlaylists();
    void showPreferences();
    void onSongSelection(QModelIndex item);
    void onPlaylistSelection(QModelIndex item);
    void onPrevious();
    void onNext();
    void onShuffle();
    void onMute();
    void onVolumeChange(int val);
    void onPositionChange(int val);
    void onConnectSynchronize();
    void onCurrentSave();
    void showCurrentPlaylist();
};

#endif // REMOTECONTROLLER_H
