#ifndef LOGGINGOBSERVER_H
#define LOGGINGOBSERVER_H

#include <iostream>
#include <fstream>
#include <list>
#include <string>

/**
 * @brief Interface for classes that can produce log messages.
 */
class ILoggable {
public:
    virtual ~ILoggable() = default;
    virtual std::string stringToLog() const = 0;
};

/**
 * @brief Abstract observer in the Observer design pattern.
 */
class Observer {
public:
    virtual ~Observer() = default;
    virtual void Update(ILoggable* loggable) = 0;
};

/**
 * @brief Subject (observable) base class.
 */
class Subject {
private:
    std::list<Observer*>* observers;
public:
    Subject();
    Subject(const Subject& other);
    Subject& operator=(const Subject& other);
    virtual ~Subject();

    void Attach(Observer* o);
    void Detach(Observer* o);
    void Notify(ILoggable* loggable) const;
};

/**
 * @brief Concrete observer that writes log entries to gamelog.txt.
 */
class LogObserver : public Observer {
private:
    std::ofstream logfile;
public:
    LogObserver();
    ~LogObserver();
    void Update(ILoggable* loggable) override;
};

#endif




