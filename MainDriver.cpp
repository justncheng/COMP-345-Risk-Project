#include "MapDriver.h"
#include "PlayerDriver.h"
#include "OrdersDriver.h"
#include "CardsDriver.h"
#include "GameEngineDriver.h"
#include "LoggingObserverDriver.h"
#include "CommandProcessingDriver.h"
#include "PlayerStrategiesDriver.h"
#include "TournamentDriver.h"

int main(int argc, char* argv[]) 
{
    testPlayerStrategies();
    testTournament();
    return 0;
}
