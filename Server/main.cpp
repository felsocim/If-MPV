#include "interface.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Interface w;
    w.show();

    Automate automate;
    //Connection entre l'automate et l'interace
    QObject::connect(&automate, SIGNAL(signalMachine(phase)), &w, SLOT(message(phase)));
    QObject::connect(&w, SIGNAL(signalClient(kCommand)), &automate, SLOT(messages(kCommand)));

    return a.exec();
}
