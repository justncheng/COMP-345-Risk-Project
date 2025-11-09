#include "MapDriver.h"
#include "PlayerDriver.h"
#include "OrdersDriver.h"
#include "CardsDriver.h"
#include "GameEngineDriver.h"
#include "LoggingObserverDriver.h"

int main() 
{
    testStartupPhase();
    testOrderExecution();
    testLoggingObserver();
    return 0;
}
