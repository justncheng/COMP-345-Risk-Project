#include "LoggingObserver.h"

// ===================== Subject Implementation =====================

Subject::Subject() {
    observers = new list<Observer*>;
}

Subject::Subject(const Subject& other) {
    observers = new list<Observer*>(*other.observers);
}

Subject& Subject::operator=(const Subject& other) {
    if (this != &other) {
        delete observers;
        observers = new list<Observer*>(*other.observers);
    }
    return *this;
}

Subject::~Subject() {
    delete observers;
}

void Subject::Attach(Observer* o) {
    observers->push_back(o);
}

void Subject::Detach(Observer* o) {
    observers->remove(o);
}

void Subject::Notify(ILoggable* loggable) const {
    for (Observer* obs : *observers)
        obs->Update(loggable);
}

// ===================== LogObserver Implementation =====================

LogObserver::LogObserver() {
    logfile.open("gamelog.txt", ios::app);
    if (!logfile.is_open())
        cerr << "[LogObserver] Error: could not open gamelog.txt" << endl;
}

LogObserver::~LogObserver() {
    if (logfile.is_open())
        logfile.close();
}

void LogObserver::Update(ILoggable* loggable) {
    if (logfile.is_open() && loggable)
        logfile << loggable->stringToLog() << endl;
}
