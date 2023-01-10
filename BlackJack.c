//Made by Michał Safuryn

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/*
X warunek ze dealer przegrywa jak ma więcej niż 21
X opcja rozpoczęcia kolejnej tury bez wychodzenia z programu
X dziala wtedy kiedy zczytywanie bez enterow
    punkty nie powinny się pokazywać na początku - 
    a) jak klikam H po raz pierwszy to punkty zostają takie same
    b) po nacisnieciu stand i tak jest dodawan jeszcze jedna karta do gracza
X przegrana powinna być automatyczna jak będę miał więcej niż 21
X funkcje dla gameMenu 
X naprawic Asa by był 1 i 11
X naprawic funkcje sum dla asa
X/O stworzyc opcje split i double down
- ala logowanie informacje w pliku i informacje o kasie itp w nim
- as przy splicie zostaje z wartoscia 1 zmienic
X uporzadkowac dunkcje i napsiac funckje na gre 

X/O uporzadkowac kod 
X stworzyc wyglad dla gry jak ponizej 

----------------------------------------
A5                    
11            
  
    h-hit       s-stand     p-split

56                    
11
----------------------------------------
*/

#define BLACKJACK 21
#define MAX_DECK 22
#define PLAYER 0
#define DEALER 1

const char *face[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

typedef struct 
{
    const char *face;
    int value;
} Card;

typedef struct 
{
    char name[30];
    int money;
} Player;

typedef struct 
{
    Player player;

    int moneyBet;
    int pIndex;
    int dIndex;
    int pTotal;
    int dTotal;
    int splited;
} Info;

void mainMenu(Card *pHand, Card *dHand, Info *info, Player *player);
void gameLogic(Card *pHand, Card *dHand, Info *info, Player *player);
void splitGameLogic(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK]);
void login(Info *info);

void hitLogic(Card *pHadn, Info *info, int aceIndex[][MAX_DECK], int *endGame);
void standLogic(Card *dHand, Info *info, int aceIndex[][MAX_DECK]);

void draw(Card *hand, int index, int aceIndex[][MAX_DECK], int *total, int player);
void FirstTwoDraws(Card *hand, int aceIndex[][MAX_DECK], int *total, int player);

void checkWinner(int total, int dTotal, Info *info);
void checkSplitWinners(int total, Info *info);
void checkIfBlackJack(int total, int dTotal, int *endGame, Info *info);

void adminInfo(int aceIndex[][MAX_DECK], Info *info, Card *pHand, Card *dHand);
void structVariablesInit(Info *info);
void resetSplitValues(int aceIndex[][MAX_DECK], Card *pHand, Info *info, int *copyOfFirst, int *endGame);

void saveToFile(Info *info, int money)
{
    FILE *file = fopen("results.txt", "a");

    info->player.money = 1000;
    fprintf(file, "%s %d\n", info->player.name, money);
    fclose(file);
}

void printHand(Card *hand, int cardAmount);
void printTable(Card *pHand, Card *dHand, Info *info, int player);

char getch();


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

void mainMenu(Card *pHand, Card *dHand, Info *info, Player *player)
{
    int end = 1;
    char k;

    login(info);

    while (end)
    {   
        printf("1 - Play BlackJack \n");
        printf("2 - Players stats \n");
        printf("3 - Exit \n");

        k = getch();

        switch (k)
        {
            case '1':
                system("clear");
                gameLogic(pHand, dHand, info, player);
                break;
            case '3':
                saveToFile(info, info->player.money);
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
    char c;
    int aceIndex[2][MAX_DECK]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    structVariablesInit(info);


    printf("How much do you wanna bet?\n");
    scanf("%d", &info->moneyBet);

    FirstTwoDraws(pHand, aceIndex, &info->pTotal, PLAYER);
    FirstTwoDraws(dHand, aceIndex, &info->dTotal, DEALER);

    checkIfBlackJack(info->pTotal, info->dTotal, &endGame, info);

    for (int i = 0; i < info->pIndex; i++)
        printf("%s ", pHand[i].face);

    while (endGame)
    {
        printf("MONEY: %d\n", info->player.money);
        c = getch();
        switch(c)
        {
            case 'h':
                hitLogic(pHand, info, aceIndex, &endGame);
                break;
            case 's':
                standLogic(dHand, info, aceIndex);
                endGame = 0;
                break;
            case 'p':
                if ((pHand[0].face == pHand[1].face || pHand[0].value == pHand[1].value) && info->splited == 0)
                {
                    info->splited = 1;
                    splitGameLogic(pHand, dHand, info, aceIndex);
                    endGame = 0;
                }
                break;
        }
        adminInfo(aceIndex, info, pHand, dHand);
    }
    printf("MONEY: %d\n", info->player.money);
}

void splitGameLogic(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK])
{
    system("clear");

    char c;
    int helpVar;
    int endGame;

    resetSplitValues(aceIndex, pHand, info, &helpVar, &endGame);
    printf("%s ", pHand[0].face);
    while (endGame)
    {
        scanf(" %c", &c);

        switch(c)
        {
            case 'h':
                hitLogic(pHand, info, aceIndex, &endGame);
                break;
            case 's':
                endGame = 0;
                break;
        }
        adminInfo(aceIndex, info, pHand, dHand);
    }
    
    int total = info->pTotal;

    resetSplitValues(aceIndex, pHand, info, &helpVar, &endGame);
    printf("%s ", pHand[0].face);
    while (endGame)
    {
        scanf(" %c", &c);

        switch(c)
        {
            case 'h':
                hitLogic(pHand, info, aceIndex, &endGame);
                break;
            case 's':
                standLogic(dHand, info, aceIndex);
                endGame = 0;
                break;
        }
        adminInfo(aceIndex, info, pHand, dHand);
    }
    checkSplitWinners(total, info);
}

void login(Info *info)
{
    int logged = 1;
    FILE *file = fopen("players.txt", "a");
    int c;
    
    while (logged)
    {
        printf("1 - Log in \n");
        printf("2 - Register \n");
        printf("3 - Exit \n");

        scanf(" %d", &c);

        switch(c)
        {
            case 1:
                printf("%s", info->player.name);
                logged = 0;
                break;
            case 2:
                printf("Write your name (max 30 characters)\n");
                scanf("%s", info->player.name);
                info->player.money = 1000;
                fprintf(file, "%s \n", info->player.name);
                logged = 0;
                break;
            case 3:
                exit(1);
                break;
        }
    }    
    fclose(file);
}

void hitLogic(Card *pHand, Info *info, int aceIndex[][MAX_DECK], int *endGame)
{
    draw(pHand, info->pIndex, aceIndex, &info->pTotal, PLAYER);
    info->pIndex++;
    info->splited = 1;
    if (info->pTotal > BLACKJACK)
    {
        printf("You Bust, dealer wins. You lose %d$\n", info->moneyBet);
        info->player.money -= info->moneyBet;
        *endGame = 0;
    }
}

void standLogic(Card *dHand, Info *info, int aceIndex[][MAX_DECK])
{
    while (info->dTotal < 17)
    {
        draw(dHand, info->dIndex, aceIndex, &info->dTotal, DEALER);
        info->dIndex++;
    }
    checkWinner(info->pTotal, info->dTotal, info);
}


void draw(Card *hand, int index, int aceIndex[][MAX_DECK] ,int *total, int player)
{
    // ran = 0 is Ace ran > 10 cards with 10 points
    int ran = rand() % 4;

    hand[index].face = face[ran];
    if(ran == 0)
    {
        hand[index].value = 11;
        aceIndex[player][index] = 1;
    }
    else if (ran >= 10)
        hand[index].value = 10;
    else 
        hand[index].value = ran + 1;    

    *total += hand[index].value;

    //Check if ace is in hand
    // Napisac funkcje na to 
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
    if (total <= BLACKJACK && dTotal > BLACKJACK)
    {
        printf("You Win, you earn %d$\n", info->moneyBet);
        info->player.money += info->moneyBet;
    }
    else if (total > dTotal)
    {
        printf("You Win, you earn %d$ \n", info->moneyBet);
        info->player.money += info->moneyBet;
    }
    else if (total < dTotal)
    {   
        printf("You Lost, you lose %d$ \n", info->moneyBet);
        info->player.money -= info->moneyBet;
    }
    else if (total == dTotal)
    {
        printf("You Draw, you earn 0$ \n");
    }
}

void checkSplitWinners(int total, Info *info)
{
    system("clear");
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
        *endGame = 0;
    }
    else if (pTotal == BLACKJACK)
    {
        printf("You Win, BLACKJACK. You earn %d\n", (int)(1.5 * info->moneyBet));
        info->player.money += (int)(1.5 * info->moneyBet);
        *endGame = 0;
    }
    else if (dTotal == BLACKJACK)
    {
        printf("You Lose, Dealer has BLACKJACK \n");
        info->player.money -= info->moneyBet;
        *endGame = 0;
    }
}


void adminInfo(int aceIndex[][MAX_DECK], Info *info, Card *pHand, Card *dHand)
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

void resetSplitValues(int aceIndex[][MAX_DECK], Card *pHand, Info *info, int *copyOfFirst, int *endGame)
{
    for (int i = 0; i < MAX_DECK; i++)
        aceIndex[PLAYER][i] = 0;

    if (pHand[0].face == face[0])
    {
        aceIndex[PLAYER][0] = 1;
        aceIndex[PLAYER][1] = 0;
    }

    *endGame = 1;
    copyOfFirst = &pHand[1].value;
    info->pIndex = 1;
    info->pTotal = *copyOfFirst;
    pHand[0].value = *copyOfFirst;
}

void structVariablesInit(Info *info)
{
    info->dIndex = 2;
    info->pIndex = 2;
    info->pTotal = 0;
    info->dTotal = 0;
    info->splited = 0;
}


/*
void printHand(Card *hand, int cardAmount)
{
    for (int i = 0; i < cardAmount; i++)
        printf("%s ", hand[i].face);
    printf("\n");

}*/

/*

void printTable(Card *pHand, Card *dHand, Info *info, int player, int splited)
{
    system("clear");
    printf("------------------------------------------------------------------ \n");
    printf("Dealer Cards On Hand: ");
    if (player == PLAYER)
        printf("%s # \n", dHand[0].face);
    else 
        printHand(dHand, kAmount);
    printf("Total Dealer Points: %d \n", kTotal);
    printf("\n \n \n");

    printf("\t h - hit \t s - stand \t d - double \n");
    if (pHand[0].face == pHand[1].face && splited == 0)
        printf("\t\t\t p - split \n");

    printf("\n \n \n");
    printf("Total Player Points: %d \n", total);
    printf("Your Cards On Hand: ");
    printHand(pHand, cAmount);
    printf("\n");
    printf("------------------------------------------------------------------- \n");
} 
*/
char getch()
{
    system ("/bin/stty raw");  
    char ret = getchar();
    system ("/bin/stty cooked");
    return ret;
}
