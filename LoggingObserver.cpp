#include "LoggingObserver.h"
#include <iostream>

// ---------------- Subject ----------------

Subject::Subject() { observers = new std::list<Observer*>; }

Subject::Subject(const Subject& other) {
    observers = new std::list<Observer*>(*other.observers);
}

Subject& Subject::operator=(const Subject& other) {
    if (this != &other) {
        delete observers;
        observers = new std::list<Observer*>(*other.observers);
    }
    return *this;
}

Subject::~Subject() { delete observers; }

void Subject::Attach(Observer* o) { observers->push_back(o); }
void Subject::Detach(Observer* o) { observers->remove(o); }

void Subject::Notify(ILoggable* loggable) const {
    for (Observer* obs : *observers)
        obs->Update(loggable);
}

// ---------------- LogObserver ----------------

LogObserver::LogObserver() {
    logfile.open("gamelog.txt", std::ios::app);
    if (!logfile.is_open())
        std::cerr << "Error: could not open gamelog.txt\n";
}

LogObserver::~LogObserver() {
    if (logfile.is_open())
        logfile.close();
}

void LogObserver::Update(ILoggable* loggable) {
    if (logfile.is_open() && loggable)
        logfile << loggable->stringToLog() << std::endl;
}

