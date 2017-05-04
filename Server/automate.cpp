#include "automate.h"
#include "shared.h"
  // assignProperty permet d'assigner une prpriété au moment de l'entrée dans la phase.
 // history state permet en gros de gader un historique des osus-états d'un état...
// Qvariant permet de stocker un grand nombre de type (pas plusieurs a la fois mais differnt pdt le temps)
Automate::Automate(QObject *parent) : QObject(parent)
{
    machine = new QStateMachine(this);

    etatInitial= new QState(machine);
    etatMuet=new QState(machine);
    etatAleat=new QState(machine);
    etatPause=new QState(machine);
    etatMuetAleat=new QState(machine);
    etatMuetPause=new QState(machine);
    etatAleatPause=new QState(machine);
    etatMuetAleatPause=new QState(machine);

    //etat initial
    etatInitial->addTransition(this,SIGNAL(signalAleat()),etatAleat);
    etatInitial->addTransition(this, SIGNAL(signalPause()),etatPause);
    etatInitial->addTransition(this,SIGNAL(signalMuet()), etatMuet);

    //etat muet
    etatMuet->addTransition(this,SIGNAL(signalAleat()),etatMuetAleat);
    etatMuet->addTransition(this,SIGNAL(signalPause()),etatMuetPause);
    etatMuet->addTransition(this,SIGNAL(signalMuet()),etatInitial);

    //etat aleat
    etatAleat->addTransition(this,SIGNAL(signalAleat()),etatInitial);
    etatAleat->addTransition(this,SIGNAL(signalPause()),etatAleatPause);
    etatAleat->addTransition(this,SIGNAL(signalMuet()),etatMuetAleat);

    //etat pause
    etatPause->addTransition(this,SIGNAL(signalAleat()),etatAleatPause);
    etatPause->addTransition(this,SIGNAL(signalPause()),etatInitial);
    etatPause->addTransition(this,SIGNAL(signalMuet()),etatMuetPause);

    //etat muet et aleat
    etatMuetAleat->addTransition(this,SIGNAL(signalAleat()),etatMuet);
    etatMuetAleat->addTransition(this,SIGNAL(signalPause()),etatMuetAleatPause);
    etatMuetAleat->addTransition(this,SIGNAL(signalMuet()),etatAleat);

    //etat muet et pause
    etatMuetPause->addTransition(this,SIGNAL(signalAleat()),etatMuetAleatPause);
    etatMuetPause->addTransition(this,SIGNAL(signalPause()),etatMuet);
    etatMuetPause->addTransition(this,SIGNAL(signalMuet()),etatPause);

    //etat aleat et pause
    etatAleatPause->addTransition(this,SIGNAL(signalAleat()),etatPause);
    etatAleatPause->addTransition(this,SIGNAL(signalPause()),etatAleat);
    etatAleatPause->addTransition(this,SIGNAL(signalMuet()),etatMuetAleatPause);

    //etat muet aleat pause
    etatMuetAleatPause->addTransition(this,SIGNAL(signalAleat()),etatMuetPause);
    etatMuetAleatPause->addTransition(this,SIGNAL(signalPause()),etatMuetAleat);
    etatMuetAleatPause->addTransition(this,SIGNAL(signalMuet()),etatAleatPause);

    machine->setInitialState(etatInitial);
    machine->start();
    setupMessages();
    /*
    QStateMachine machine;
    QState *s1 = new QState();
    s1->assignProperty(&button, "text", "Click me");

    QFinalState *s2 = new QFinalState();
    s1->addTransition(&button, SIGNAL(clicked()), s2);

    machine.addState(s1);
    machine.addState(s2);
    machine.setInitialState(s1);
    machine.start();
    */
}

void Automate::messages(kCommand b){
    switch (b) {
    case kButtonMuet:
        emit signalMuet();
        break;
    case kButtonAleat:
        emit signalAleat();
        break;
    case kButtonPause:
        emit signalPause();
        break;
    default:
        break;
    }
}

void Automate::setupMessages(){
    // &QState::exited pour le signal de sortie d'état
  QObject::connect(etatInitial, &QState::entered, [this](){
      emit signalMachine(kPhaseInitial);
    });
  QObject::connect(etatMuet, &QState::entered, [this](){
      emit signalMachine(kPhaseMuet);
    });
  QObject::connect(etatAleat, &QState::entered, [this](){
      emit signalMachine(kPhaseAleat);
    });
  QObject::connect(etatPause, &QState::entered, [this](){
      emit signalMachine(kPhasePause);
    });
  QObject::connect(etatMuetAleat, &QState::entered, [this](){
      emit signalMachine(kPhaseMuetAleat);
    });
  QObject::connect(etatMuetPause, &QState::entered, [this](){
      emit signalMachine(kPhaseMuetPause);
    });
  QObject::connect(etatAleatPause, &QState::entered, [this](){
      emit signalMachine(kPhaseAleatPause);
    });
  QObject::connect(etatMuetAleatPause, &QState::entered, [this](){
      emit signalMachine(kPhaseMuetAleatPause);
    });
}
