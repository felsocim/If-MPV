#ifndef INTERFACE_H
#define INTERFACE_H

#include <QMainWindow>
#include <QDebug>
#include <QJsonArray>
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

private:
    Ui::Interface *ui;
    Player * player;

private slots:
    void startPlayerInstance(bool clicked);
    void on_volTest_clicked();
};

#endif // INTERFACE_H
