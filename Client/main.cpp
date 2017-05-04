/**
  *Marek Felsoci, Arnaud Pinsun
  *
  **/
#include "remotecontroller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RemoteController w;
    w.show();

    return a.exec();
}
