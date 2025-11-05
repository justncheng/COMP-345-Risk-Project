#include "MapDriver.h"
#include "PlayerDriver.h"
#include "OrdersDriver.h"
#include "CardsDriver.h"
#include "GameEngineDriver.h"

int main() 
{
    testLoadMaps();
    testPlayers();
	testOrderExecution();
    testCards();
    testGameStates(); 
    return 0;
}
