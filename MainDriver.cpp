#include "MapDriver.h"
#include "PlayerDriver.h"
#include "OrdersDriver.h"
#include "CardsDriver.h"
#include "GameEngineDriver.h"

void testLoggingObserver();

int main() 
{
    testStartupPhase();
    testOrderExecution();
    testLoggingObserver();
    return 0;
}
