//Made by Michał Safuryn

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

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
- dodac sprawdanie czy mozna obstawic dan ilosc pieniedzy
- dodac wyglad zeby jakos wyglada
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
    int pTotal;

    int dIndex;
    int dTotal;

    int splited;
    int splitGame;
} Info;

void mainMenu(Card *pHand, Card *dHand, Info *info, Player *player);
void gameLogic(Card *pHand, Card *dHand, Info *info, Player *player);
void splitGameLogic(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK]);
void login(Info *info);

void mainGameLoop(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK], int endGame);
void hitLogic(Card *pHadn, Info *info, int aceIndex[][MAX_DECK], int *endGame);
void standLogic(Card *dHand, Info *info, int aceIndex[][MAX_DECK]);

void draw(Card *hand, int index, int aceIndex[][MAX_DECK], int *total, int player);
void FirstTwoDraws(Card *hand, int aceIndex[][MAX_DECK], int *total, int player);

void checkWinner(int total, int dTotal, Info *info);
void checkSplitWinners(int total, Info *info);
void checkIfBlackJack(int total, int dTotal, int *endGame, Info *info);
void checkBetMoney(Info *info);

void devInfo(int aceIndex[][MAX_DECK], Info *info, Card *pHand, Card *dHand);
void resetSplitValues(int aceIndex[][MAX_DECK], Card *pHand, Info *info, int copyOfFirst, int *endGame);
void structVariablesInit(Info *info);
void saveToFile(Info *info, int money);

void printHand(Card *hand, int cardOnHand);
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
    int aceIndex[2][MAX_DECK]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    structVariablesInit(info);

    printf("MONEY: %d\n", info->player.money);
    checkBetMoney(info);

    //First to cards for player and dealer
    FirstTwoDraws(pHand, aceIndex, &info->pTotal, PLAYER);
    FirstTwoDraws(dHand, aceIndex, &info->dTotal, DEALER);
    checkIfBlackJack(info->pTotal, info->dTotal, &endGame, info);

    mainGameLoop(pHand, dHand, info, aceIndex, endGame);

    printTable(pHand, dHand, info, DEALER);
    checkWinner(info->pTotal, info->dTotal, info);
}

void splitGameLogic(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK])
{
    system("clear");

    char c;
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

void login(Info *info)
{
    int logged = 1;
    int c;
    int moneyInFile;
    char username[30];
    int compareResult;
    FILE *file = fopen("players.txt", "a");
    FILE *r_file = fopen("results.txt", "r");

    while (logged)
    {
        printf("1 - Log in \n");
        printf("2 - Register \n");
        printf("3 - Exit \n");

        scanf(" %d", &c);

        switch(c)
        {
            system("clear");

            case 1:
                if (r_file == NULL)
                    printf("asdadasd");
                
                printf("Write your name (max 30 characters) no spaces \n");
                scanf("%s", info->player.name);

                while (fscanf(r_file, "%s %d", username, &moneyInFile) == 2)
                {
                    printf("%s %d \n", username, moneyInFile);
                    compareResult = strcmp(username, info->player.name);
                    if (compareResult == 0)
                    {
                        printf("You have logged in \n");
                        info->player.money = moneyInFile;
                        logged = 0;
                        break;
                    }
                }
                if (compareResult != 1)
                    printf("You are here for the first time, try to register \n");
                sleep(2);
                break;
            case 2:
                printf("Write your name (max 30 characters) no spaces \n");
                scanf("%29s", info->player.name);
                info->player.money = 1000;
                fprintf(file, "%s \n", info->player.name);
                logged = 0;
                break;
            case 3:
                exit(1);
                break;
        }
    }    
    fclose(r_file);
    fclose(file);
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
                if (info->splited == 1 && info->splitGame == 0)
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
                if ((pHand[0].face == pHand[1].face || pHand[0].value == pHand[1].value) && info->splited == 0)
                {
                    info->splited = 1;
                    splitGameLogic(pHand, dHand, info, aceIndex);
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
    info->splited = 1;
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
    if (info->pTotal > BLACKJACK)
    {
        printf("You Bust with %d points, dealer wins. You lose %d$\n", total, info->moneyBet);
        info->player.money -= info->moneyBet;
    }
    else if (total <= BLACKJACK && dTotal > BLACKJACK)
    {
        printf("You Win with %d points, you earn %d$ \n", total, info->moneyBet);
        info->player.money += info->moneyBet;
    }
    else if (total > dTotal)
    {
        printf("You Win with %d points, you earn %d$ \n", total, info->moneyBet);
        info->player.money += info->moneyBet;
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
    for (int i = 0; i < MAX_DECK; i++)
        aceIndex[PLAYER][i] = 0;

    if (pHand[0].face == face[0])
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
    info->splitGame = 0;
}

void saveToFile(Info *info, int money)
{
    FILE *file = fopen("results.txt", "a");

    info->player.money = 1000;
    fprintf(file, "%s %d\n", info->player.name, money);
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

    printf("\t\t\t h - hit \t s - stand \t \n");
    if ((pHand[0].face == pHand[1].face || pHand[0].value == pHand[1].value) && info->splited == 0)
        printf("\t\t\t\t p - split \n");

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
