#include "MapDriver.h"
#include "PlayerDriver.h"
#include "OrdersDriver.h"
#include "CardsDriver.h"
#include "GameEngineDriver.h"
#include "LoggingObserverDriver.h"
#include "CommandProcessingDriver.h"

int main(int argc, char* argv[]) 
{
    testCommandProcessor(argc, argv);
    testStartupPhase();
    testMainGameLoop();
    testOrderExecution();
    testLoggingObserver();
    return 0;
}
