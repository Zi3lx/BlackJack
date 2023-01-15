#include "functions.h"

void mainMenu(Card *pHand, Card *dHand, Info *info, Player *player)
{
    int end = 1;
    char k;

    mainLoginFunction(info);
    sleep(1);
    while (end)
    {   
        printf("1 - Play BlackJack \n");
        printf("2 - Players stats \n");
        printf("3 - Exit \n");

        k = getch();
        system("clear");
        switch (k)
        {
            case '1':
                gameLogic(pHand, dHand, info, player);
                break;
            case '2':
                showAllPlayers();
                break;
            case '3':
                end = 0;
                break;
            default:
                system("clear");
                break;
        }
    }
}
void gameLogic(Card *pHand, Card *dHand, Info *info, Player *player)
{
    //Variables initialization
    int endGame = 1;
    int aceIndex[2][MAX_DECK]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    structVariablesInit(info);

    printf("MONEY: %d\n", info->player.money);
    checkBetMoney(info);

    //First to cards for player and dealer
    FirstTwoDraws(pHand, aceIndex, &info->pTotal, PLAYER);
    FirstTwoDraws(dHand, aceIndex, &info->dTotal, DEALER);

    printTable(pHand, dHand, info, PLAYER);
    checkIfBlackJack(info->pTotal, info->dTotal, &endGame, info);

    if (info->gotBlackJack == 0)
    {
        mainGameLoop(pHand, dHand, info, aceIndex, endGame);

        printTable(pHand, dHand, info, DEALER);
        if (info->splited == 0)
            checkWinner(info->pTotal, info->dTotal, info);
    }
    info->player.games = info->player.games + 1;
    saveToFile(info);
}
void splitGameLogic(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK])
{
    system("clear");

    int helpVar;
    int endGame;
    int copyOfFirst = pHand[1].value;

    //First Split Game
    resetSplitValues(aceIndex, pHand, info, copyOfFirst, &endGame);
    printTable(pHand, dHand, info, PLAYER);

    mainGameLoop(pHand, dHand, info, aceIndex, endGame);
    
    int total = info->pTotal;
    printTable(pHand, dHand, info, DEALER);

    //Second Split Game
    resetSplitValues(aceIndex, pHand, info, copyOfFirst, &endGame);
    printTable(pHand, dHand, info, PLAYER);

    mainGameLoop(pHand, dHand, info, aceIndex, endGame);

    printTable(pHand, dHand, info, DEALER);
    checkSplitWinners(total, info);
}

void mainLoginFunction(Info *info)
{
    int logged = 1;
    int c;

    while (logged)
    {
        system("clear");
        printf("1 - Log in \n");
        printf("2 - Register \n");
        printf("3 - Exit \n");

        c = getch();

        switch(c)
        {
            case '1':
                login(info, &logged);
                break;
            case '2':
                registerLogic(info, &logged);
                break;
            case '3':
                exit(1);
                break;
        }
    }    
}
void registerLogic(Info *info, int *logged)
{
    int reg = 0;
    int moneyInFile, gamesPlayed, wonGames;
    char username[MAX_LEN];
    FILE *r_file = fopen("results.txt", "r");

    if (r_file == NULL) 
    {
        printf("Error opening file, please make sure the file exists\n");
        exit(1);
    }

    printf("Write your name (max 30 characters min 3) no spaces \n");
    scanf("%s", info->player.name);

    //Checking if input (name) is correct no psacess and 3 to 30 char
    if (strlen(info->player.name) < 3 || strlen(info->player.name) > MAX_LEN)
    {
        printf("Invalid name length\n");
        return;
    }
    else if (strchr(info->player.name, ' ') != NULL)
    {
        printf("Name shouldn't contain any spaces\n");
        return;
    }
    else 
    {
        while (fscanf(r_file, "%s %d %d %d", username, &moneyInFile, &gamesPlayed, &wonGames) == 4)
        {
            if (strcmp(info->player.name, username) == 0)
            {
                printf("Name already in data base. Use diffrent name \n");
                reg = 1;
                break;
            }
        }
    }

    if (reg == 0)
    {
        printf("Register successful\n");
        sleep(1);
        info->player.money = 1000;
        info->player.games = 0;
        info->player.wonGames = 0;
        saveToFile(info);
        *logged = 0;
    }
    fclose(r_file);
}
void login(Info *info, int *logged)
{
    int moneyInFile, gamesPlayed, wonGames;
    char username[MAX_LEN];
    int loggedIn = 0;
    FILE *r_file = fopen("results.txt", "r");

    if (r_file == NULL) 
    {
        printf("Error opening file, please make sure the file exists\n");
        exit(1);
    }

    printf("Write your name to login\n");
    scanf("%29s", info->player.name);

    //Check if username is in file if yes you can login
    while (fscanf(r_file, "%s %d %d %d", username, &moneyInFile, &gamesPlayed, &wonGames) == 4)
    {
        if (strcmp(info->player.name, username) == 0)
        {
            printf("You have logged in \n");
            sleep(1);
            info->player.money = moneyInFile;
            info->player.games = gamesPlayed;
            info->player.wonGames = wonGames;
            fclose(r_file);
            loggedIn = 1;
            *logged = 0;
        }
    }
    if(loggedIn == 0)
        printf("You are here for the first time, try to register \n");
    fclose(r_file);
}

void mainGameLoop(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK], int endGame)
{
    char c;
    printTable(pHand, dHand, info, PLAYER);

    while (endGame)
    {
        c = getch();

        switch(c)
        {
            case 'h':
                hitLogic(pHand, info, aceIndex, &endGame);
                printTable(pHand, dHand, info, PLAYER);
                break;
            case 's':
                //If is for split and noral game else is for first split game so the dealer won't show his cards after first game 
                if (info->splitGame == 0)
                {
                    standLogic(dHand, info, aceIndex);
                    endGame = 0;
                }
                else 
                {
                    info->splitGame = 1;
                    endGame = 0;
                }
                break;
            case 'p':
                //Check if faces are equal or vaues and if game was already spliteds
                if ((pHand[0].face == pHand[1].face || pHand[0].value == pHand[1].value) && info->splited == 0)
                {
                    info->splited = 1;
                    splitGameLogic(pHand, dHand, info, aceIndex);
                    endGame = 0;
                }
                break;
            case 'd':
                if (info->splited == 0 && info->pIndex == 2)
                {
                    doubleDown(pHand, dHand, info, aceIndex);
                    if (info->pIndex == 3)
                        endGame = 0;
                }
                break;
            default:
                printTable(pHand, dHand, info, PLAYER);
                break;
        }
        //devInfo(aceIndex, info, pHand, dHand);
    }
}
void hitLogic(Card *pHand, Info *info, int aceIndex[][MAX_DECK], int *endGame)
{
    draw(pHand, info->pIndex, aceIndex, &info->pTotal, PLAYER);
    info->pIndex++;
    if (info->pTotal > BLACKJACK)
        *endGame = 0;
}
void standLogic(Card *dHand, Info *info, int aceIndex[][MAX_DECK])
{
    while (info->dTotal < 17)
    {
        draw(dHand, info->dIndex, aceIndex, &info->dTotal, DEALER);
        info->dIndex++;
    }
}
void doubleDown(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK])
{
    //Check if you can double 
    if (info->moneyBet * 2 > info->player.money)
    {
        printf("Not enough money to double \n");
        return;
    } 
    else
    {
        draw(pHand, info->pIndex, aceIndex, &info->pTotal, PLAYER);
        info->pIndex++;
        info->doubledDown = 1;
    }
}

void draw(Card *hand, int index, int aceIndex[][MAX_DECK] ,int *total, int player)
{
    //Ran = 0 is Ace ran > 10 cards with 10 points
    int ran = rand() % 13;

    hand[index].face = cardFace[ran];
    if(ran == 0)
    {
        hand[index].value = 11;
        //Remember the ace position
        aceIndex[player][index] = 1;
    }
    else if (ran >= 10)
        hand[index].value = 10;
    else 
        hand[index].value = ran + 1;    

    *total += hand[index].value;

    //Checks if ace is in hand if so checks the total value is greater than 21 if so change ace value to 1
    if (*total > BLACKJACK)
    {
        for (int i = 0; i < MAX_DECK; i++)
            if (aceIndex[player][i] == 1)
            {
                aceIndex[player][i] = 0;
                hand[i].value = 1;
                *total = 0;

                for (int j = 0; j < index + 1; j++)
                    *total += hand[j].value;   

                if(*total < BLACKJACK)
                    break;
            }
    }   
}
void FirstTwoDraws(Card *hand,int aceIndex[][MAX_DECK], int *total, int player)
{
    for (int i = 0; i < 2; i++)
        draw(hand, i, aceIndex, total, player);
}

void checkWinner(int total, int dTotal, Info *info)
{
    if (info->doubledDown == 1)
        info->moneyBet *= 2;
    //Checks all game options to win or lose
    if (total > BLACKJACK)
    {
        printf("You Bust with %d points, dealer wins. You lose %d$\n", total, info->moneyBet);
        info->player.money -= info->moneyBet;
    }
    else if (total <= BLACKJACK && dTotal > BLACKJACK)
    {
        printf("You Win with %d points, you earn %d$ \n", total, info->moneyBet);
        info->player.money += info->moneyBet;
        info->player.wonGames = info->player.wonGames + 1;
    }
    else if (total > dTotal)
    {
        printf("You Win with %d points, you earn %d$ \n", total, info->moneyBet);
        info->player.money += info->moneyBet;
        info->player.wonGames = info->player.wonGames + 1;
    }
    else if (total < dTotal)
    {   
        printf("You Lost with %d points, you lose %d$ \n", total, info->moneyBet);
        info->player.money -= info->moneyBet;
    }
    else if (total == dTotal)
    {
        printf("You Draw with %d points, you earn 0$ \n", total);
    }
}
void checkSplitWinners(int total, Info *info)
{
    printf("Dealer Points: %d\n", total);
    printf("First Game: \n");   
    checkWinner(total, info->dTotal, info);
    printf("Second Game: \n");   
    checkWinner(info->pTotal, info->dTotal, info);
    sleep(3);
}
void checkIfBlackJack(int pTotal, int dTotal, int *endGame, Info *info)
{
    if (pTotal == BLACKJACK && dTotal == BLACKJACK)
    {
        printf("Draw, Dealer and you have BLACKJACK. You earn 0$\n");
        sleep(3);
        *endGame = 0;
    }
    else if (pTotal == BLACKJACK)
    {
        printf("You Win, BLACKJACK. You earn %d\n", (int)(1.5 * info->moneyBet));
        info->player.money += (int)(1.5 * info->moneyBet);
        info->player.wonGames = info->player.wonGames + 1;
        info->gotBlackJack = 1;
        sleep(3);
        *endGame = 0;
    }
    else if (dTotal == BLACKJACK)
    {
        printf("You Lose, Dealer has BLACKJACK \n");
        info->player.money -= info->moneyBet;
        info->gotBlackJack = 1;
        sleep(3);
        *endGame = 0;
    }
}
void checkBetMoney(Info *info)
{
    while (1)
    {
        printf("How much do you wanna bet?\n");
        scanf("%d", &info->moneyBet);
        
        if (info->player.money <=0)
        {
            printf("You dont have money\n");
            exit(1);
        }
        else if (info->moneyBet <= info->player.money && info->moneyBet != 0 && info->moneyBet > 0)
            break;
        system("clear");
        printf("Bet right amount?\n");
    }
}

void devInfo(int aceIndex[][MAX_DECK], Info *info, Card *pHand, Card *dHand)
{
    for (int i = 0; i < 22; i++)
    {
        printf("%d", aceIndex[0][i]);
    }
    printf("\n");
    for (int i = 0; i < info->pIndex; i++)
    {
        printf("%d ", pHand[i].value);
    }
    printf("\n");   
    for (int i = 0; i < info->pIndex; i++)
    {
        printf("%s ", pHand[i].face);
    }
    printf("\n");

    for (int i = 0; i < 22; i++)
    {
        printf("%d", aceIndex[1][i]);
    }
    for (int i = 0; i < info->dIndex; i++)
    {
        printf("%s", dHand[i].face);
    }
    printf("\n %d p    d %d \n", info->pTotal, info->dTotal);
    printf("\n \n \n");
}

void resetSplitValues(int aceIndex[][MAX_DECK], Card *pHand, Info *info, int copyOfFirst, int *endGame)
{
    //This function prepare values for split game 
    for (int i = 0; i < MAX_DECK; i++)
        aceIndex[PLAYER][i] = 0;

    if (pHand[0].face == cardFace[0])
    {
        aceIndex[PLAYER][0] = 1;
        aceIndex[PLAYER][1] = 0;
    }

    *endGame = 1;
    info->pIndex = 1;
    info->pTotal = copyOfFirst;
    pHand[0].value = copyOfFirst;
}
void structVariablesInit(Info *info)
{
    info->dIndex = 2;
    info->pIndex = 2;
    info->pTotal = 0;
    info->dTotal = 0;
    info->splited = 0;
    info->doubledDown = 0;
    info->checkNormalWin = 0;
    info->gotBlackJack = 0;
    info->splitGame = 0;
}
void saveToFile(Info *info)
{
    int moneyInFile, gamesPlayed, wonGames;
    char username[MAX_LEN];
    int playerExist = 0;
    FILE *file = fopen("results.txt", "r");
    FILE *tempFile = fopen("temp.txt", "w");

    //Checking if username and given file are in file if yes copies it to temp 
    //file then remove the result.txt and changes the name of temp file to result 
    while (fscanf(file, "%s %d %d %d", username, &moneyInFile, &gamesPlayed, &wonGames) == 4) 
    {
        if (strcmp(username, info->player.name) == 0) 
        {
            moneyInFile = info->player.money;
            gamesPlayed = info->player.games;
            wonGames = info->player.wonGames;
            playerExist = 1;
        }
        fprintf(tempFile, "%s %d %d %d\n", username, moneyInFile, gamesPlayed, wonGames);
    }
    if (!playerExist) 
    {
        fprintf(tempFile, "%s %d %d %d\n", info->player.name, info->player.money, info->player.games, info->player.wonGames);
    }
    
    fclose(file);
    fclose(tempFile);
    remove("results.txt");
    rename("temp.txt", "results.txt");
}

void showAllPlayers()
{
    FILE *file = fopen("results.txt", "r");
    char username[MAX_LEN], k;
    int cash, index = 1, gamesPlayed, wonGames;
 
    system("clear");
    printf("Press E to exit \n");
    printf("  %-20s %-8s %-12s %-8s\n", "PlayerName", "Money", "GamesPlayed", "WonGames");    
    while (fscanf(file, "%s %d %d %d", username, &cash, &gamesPlayed, &wonGames) == 4)
    {
        printf("%d. %-20s %-8d %-12d %-8d\n", index, username, cash, gamesPlayed, wonGames);
        index++;
    }
    while (k != 'e')
        k = getch();
    
    system("clear");
    fclose(file);
}
void printHand(Card *hand, int cardOnHand)
{
    for (int i = 0; i < cardOnHand; i++)
        printf("%s ", hand[i].face);
    printf("\n");

}
void printTable(Card *pHand, Card *dHand, Info *info, int player)
{
    system("clear");
    printf("------------------------ \u2660\u2665 BLACKJACK \u2663\u2666 ------------------------ \n");
    printf("Dealer Cards On Hand: ");
    //Depending on the current state of the game printing half of dealers hand or all of them
    if (player == PLAYER)
    {
        printf("%s # \n", dHand[0].face);
        printf("Total Dealer Points: %d \n", (info->dTotal - dHand[1].value));
    }
    else 
    {
        printHand(dHand, info->dIndex);
        printf("Total Dealer Points: %d \n", info->dTotal);
    }
    printf("\n \n \n");

    printf("\t h - hit \t s - stand");
    if (info->splited == 0)
        printf("\t d - double \n");
    //Split options shows only if face of first card = face of second card or value of first = value of second and the game wasn't splited
    if ((pHand[0].face == pHand[1].face || pHand[0].value == pHand[1].value) && info->splited == 0)
        printf("\t\t\t p - split \n");

    printf("\n \n \n");
    printf("Total Player Points: %d \n", info->pTotal);
    printf("Your Cards On Hand: ");
    printHand(pHand, info->pIndex);
    printf("\n");
    printf("Money: %d                                         Money Bet: %d   \n", info->player.money, info->moneyBet);
    printf("----------------------------------------------------------------- \n");
} 

char getch()
{
    system ("/bin/stty raw");  
    char ret = getchar();
    system ("/bin/stty cooked");
    return ret;
}