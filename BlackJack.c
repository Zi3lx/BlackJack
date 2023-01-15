//Michał Safuryn

#include "functions.h"

int main()
{
    srand(time(NULL));

    Player player;
    Info info;
    Card pHand[MAX_DECK];  
    Card dHand[MAX_DECK];

    system("clear");
    mainMenu(pHand, dHand, &info, &player);

    return 0;
}