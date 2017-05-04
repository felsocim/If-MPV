/**
  *Marek Felsoci, Arnaud Pinsun
  *
  **/
#ifndef AUTOMATE_H
#define AUTOMATE_H

#include <QObject>
#include <QStateMachine>
#include <QState>
#include <QHistoryState>
#include <QFinalState>
#include <QSignalTransition>
#include <QTimer>
#include <QDebug>
#include <QPushButton>
#include "shared.h"

/*
enum boutton {
    kButtonMuet,
    kButtonPause,
    kButtonAleat
};*/

class Automate : public QObject
{
    Q_OBJECT
public:
    explicit Automate(QObject *parent = 0);
    void setupMessages();
private:

    // notre automate
    QStateMachine* machine;

    // Les differents etats de notre automate
    QState* etatInitial;
    QState* etatMuet;
    QState* etatAleat;
    QState* etatPause;
    QState* etatMuetAleat;
    QState* etatMuetPause;
    QState* etatAleatPause;
    QState* etatMuetAleatPause;
signals:
    // Pour communiquer avec l'UI
    void signalMachine(phase);

    //Pour la communication interne
    void signalMuet();
    void signalPause();
    void signalAleat();
public slots:
    void messages(kCommand);
};

#endif // AUTOMATE_H
