#ifndef LOGGINGOBSERVER_H
#define LOGGINGOBSERVER_H

#include <iostream>
#include <fstream>
#include <list>
#include <string>
using namespace std;

/**
 * @brief Interface for all classes that can produce a log message.
 *
 * Classes that inherit from ILoggable must implement stringToLog(),
 * returning a human-readable string describing the most recent event
 * that should appear in the game log.
 */
class ILoggable {
public:
    virtual ~ILoggable() = default;
    virtual string stringToLog() const = 0;
};

/**
 * @brief Abstract observer in the Observer design pattern.
 *
 * Observers register themselves to Subjects to receive loggable updates.
 */
class Observer {
public:
    virtual ~Observer() = default;

    /**
     * @brief Called by a Subject when an observable event occurs.
     * @param loggable The ILoggable object containing the log message.
     */
    virtual void Update(ILoggable* loggable) = 0;
};

/**
 * @brief Subject class implementing the Observable role.
 *
 * Maintains a dynamic list of observers and notifies them when a
 * loggable event occurs. Classes that generate log events
 * (Command, CommandProcessor, OrdersList, Order, GameEngine)
 * should inherit from Subject.
 */
class Subject {
private:
    list<Observer*>* observers; ///< List of attached observers.

public:
    Subject();
    Subject(const Subject& other);
    Subject& operator=(const Subject& other);
    virtual ~Subject();

    /**
     * @brief Registers an observer.
     * @param o The observer to attach.
     */
    void Attach(Observer* o);

    /**
     * @brief Unregisters an observer.
     * @param o The observer to detach.
     */
    void Detach(Observer* o);

    /**
     * @brief Notifies all attached observers of an event.
     * @param loggable The ILoggable object containing the log message.
     */
    void Notify(ILoggable* loggable) const;
};

/**
 * @brief Concrete Observer that writes log messages to a file.
 *
 * The LogObserver receives notifications from any Subject implementing
 * ILoggable and appends their messages to "gamelog.txt".
 */
class LogObserver : public Observer {
private:
    ofstream logfile; ///< Output file stream for the log file.

public:
    LogObserver();
    ~LogObserver();

    /**
     * @brief Writes a new log entry to gamelog.txt.
     * @param loggable The ILoggable source of the log message.
     */
    void Update(ILoggable* loggable) override;
};

#endif // LOGGINGOBSERVER_H


