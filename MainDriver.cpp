//#include "MapDriver.h" UNCOMMENT WHEN IMPLEMENTED
#include "PlayerDriver.h"
#include "OrdersDriver.h"
#include "CardsDriver.h"
//#include "GameEngineDriver.h" UNCOMMENT WHEN IMPLEMENTED

void testPlayers();
void testOrderLists();
void testCards();

int main() 
{
    //testLoadMaps(); UNCOMMENT WHEN IMPLEMENTED
    testPlayers();
	testOrderLists();
    testCards();
    //testGameStates(); UNCOMMENT WHEN IMPLEMENTED
    return 0;
}