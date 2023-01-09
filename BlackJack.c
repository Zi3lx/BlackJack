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
- stworzyc opcje split i double down
- ala logowanie informacje w pliku i informacje o kasie itp w nim
- as przy splicie zostaje z wartoscia 1 zmienic
- uporzadkowac dunkcje i napsiac funckje na gre 

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
    int pIndex;
    int dIndex;
    int pTotal;
    int dTotal;
    int splited;
} Info;

typedef struct 
{
    char name[30];
    int money;
} Player;

void mainMenu(Card *pHand, Card *dHand, Info *info, int *aceIndex);
void gameLogic(Card *pHand, Card *dHand, Info *info, int *aceIndex);
void splitGameLogic(Card *pHand, Card *dHand, Info *info, int *aceIndex);
void draw(Card *hand, int index, int *aceIndex, int *total);
void FirstTwoDraws(Card *hand, int *aceIndex, int *total);

void checkWinner(int total, int kTotal, int *endGame);
void checkIfBlackJack(int *total, int *kTotal, int *endGame);
void printHand(Card *hand, int cardAmount);
void printTable(Card *pHand, Card *dHand, int total, int kTotal, int cAmount, int kAmount, int player, int splited);

char getch();


int main()
{
    srand(time(NULL));

    Player player;

    Info info;

    info.dIndex = 2;
    info.dIndex = 2;
    info.pTotal = 0;
    info.dTotal = 0;
    info.splited = 0;

    Card pHand[MAX_DECK];  
    Card dHand[MAX_DECK];

    int aceIndex[MAX_DECK]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Zainicjować dane struktury i zamiast podawani 4 arguentow przekazac wskaźniki na strukture !!!!!!!!!!

    int i = 2, j = 2; 
    int total = 0, kTotal = 0;

    system("clear");
    mainMenu(pHand, dHand, i, j, total, kTotal, aceIndex);

    return 0;
}

void mainMenu(Card *pHand, Card *dHand, int i, int j, int total, int kTotal, int *aceIndex)
{
    int end = 1;
    char k;
    printf("1 - Play BlackJack \n");
    printf("2 - Players stats \n");
    printf("3 - Exit \n");

    while (end)
    {   
        k = getchar();
        printf("1 - Play BlackJack \n");
        printf("2 - Players stats \n");
        printf("3 - Exit \n");

        switch (k)
        {
            case '1':
                system("clear");
                gameLogic(pHand, dHand, i, j, total, kTotal, aceIndex);
                break;
            case '3':
                end = 0;
                system("clear");
                break;
        }
    }
}

void gameLogic(Card *pHand, Card *dHand, int i, int j, int total, int kTotal, int *aceIndex)
{
    int endGame = 1;
    int splited = 0;
    char c;

    FirstTwoDraws(pHand, aceIndex, &total);
    FirstTwoDraws(dHand, aceIndex, &kTotal);

    printTable(pHand, dHand, total, kTotal - dHand[1].value, i, j, PLAYER, splited);
    checkIfBlackJack(&total, &kTotal, &endGame);

    while (endGame)
    {
        scanf("%c", &c);

        switch(c)
        {
            case 'h':
                draw(pHand, i, aceIndex, &total);
                printTable(pHand, dHand, total, kTotal - dHand[1].value, i+1, j+1, PLAYER, splited);
                i++;
                splited = 1;
                if (total > BLACKJACK)
                {
                    printTable(pHand, dHand, total, kTotal, i, j, DEALER, splited);
                    printf("You Lost \n");
                    endGame = 0;
                    break;
                }
                break;
            case 's':
                while (kTotal < 17)
                {
                    draw(dHand, j, aceIndex, &kTotal);
                    printTable(pHand, dHand, total, kTotal, i, j, DEALER, splited);
                    j++;
                }
                if (splited == 0)
                    printTable(pHand, dHand, total, kTotal, i, j, DEALER, splited);
                checkWinner(total, kTotal, &endGame);
                break;
            case 'p':
                if (pHand[0].face == pHand[1].face && splited == 0)
                {
                    splited = 1;
                    splitGameLogic(pHand, dHand, j, i, total, kTotal, aceIndex, splited);
                    endGame = 0;
                }
                break;
        }
    }
}

void splitGameLogic(Card *pHand, Card *dHand, int i, int j, int total, int kTotal, int *aceIndex, int splited)
{
    Card cSHandOne[MAX_DECK];
    Card cSHandTwo[MAX_DECK];
    char c;
    i = 1;
    int totalOne = pHand[0].value;
    int endGameOne = 1;
    cSHandOne[0].face = pHand[0].face;
    cSHandOne[0].value = pHand[0].value;

    int totalTwo = pHand[1].value;
    int endGameTwo = 1;
    cSHandTwo[0].face = pHand[1].face;
    cSHandTwo[0].value = pHand[1].value;

    printTable(cSHandOne, dHand, totalOne, kTotal - dHand[1].value, i, j, PLAYER, splited);

    while (endGameOne)
    {
        scanf("%c", &c);

        switch(c)
        {
            case 'h':
                draw(cSHandOne, i, aceIndex, &totalOne);
                printTable(cSHandOne, dHand, totalOne, kTotal - dHand[1].value, i+1, j+1, PLAYER, splited);
                i++;
                if (total > BLACKJACK)
                {
                    printTable(cSHandOne, dHand, totalOne, kTotal, i, j, DEALER, splited);
                    printf("You Lost \n");
                    endGameOne = 0;
                }
                break;
            case 's':
                endGameOne = 0;
                break;
        }
    }
    i = 1;

    printTable(cSHandTwo, dHand, totalTwo, kTotal - dHand[1].value, i, j, PLAYER, splited);

    while (endGameTwo)
    {
        scanf("%c", &c);

        switch(c)
        {
            case 'h':
                draw(cSHandTwo, i, aceIndex, &totalTwo);
                printTable(cSHandTwo, dHand, totalTwo, kTotal - dHand[1].value, i+1, j+1, PLAYER, splited);
                i++;
                if (totalTwo > BLACKJACK)
                {
                    printTable(cSHandTwo, dHand, totalTwo, kTotal, i, j, DEALER, splited);
                    printf("You Lost \n");
                    endGameTwo = 0;
                }
                break;
            case 's':
                while (kTotal < 17)
                {
                    draw(dHand, j, aceIndex, &kTotal);
                    printTable(cSHandTwo, dHand, totalTwo, kTotal, i, j, DEALER, splited);
                    j++;
                }
                break;
        }
    }
    printTable(cSHandTwo, dHand, totalTwo, kTotal, i, j, DEALER, splited);
    printf("First Game: \n");   
    checkWinner(totalOne, kTotal, &endGameOne);
    printf("Second Game: \n");   
    checkWinner(totalTwo, kTotal, &endGameTwo);
}

void draw(Card *hand, int index, int *aceIndex ,int *total)
{
    // ran = 0 is Ace ran > 10 cards with 10 points
    int ran = rand() % 13;
    int aceInDeck = 0;
    hand[index].face = face[ran];
    if(ran == 0)
    {
        hand[index].value = 11;
        aceIndex[index] = 1;
        aceInDeck = 1;
    }
    else if (ran >= 10)
        hand[index].value = 10;
    else 
        hand[index].value = ran + 1;    

    *total += hand[index].value;

    //Check if ace is in hand
    if (*total > BLACKJACK && aceInDeck == 1)
    {
        for (int i = 0; i < MAX_DECK; i++)
            if (aceIndex[i] == 1)
            {
                aceIndex[i] = 0;
                hand[i].value = 1;
                *total -= 10;
                if(*total < BLACKJACK)
                    break;
            }   
    }   
}

void FirstTwoDraws(Card *hand,int *aceIndex, int *total)
{
    for (int i = 0; i < 2; i++)
        draw(hand, i, aceIndex, total);
}

void printHand(Card *hand, int cardAmount)
{
    for (int i = 0; i < cardAmount; i++)
        printf("%s ", hand[i].face);
    printf("\n");

}

void checkWinner(int total, int kTotal, int *endGame)
{
    if (total > BLACKJACK)
        printf("You Bust, dealer wins \n");
    else if (total <= BLACKJACK && kTotal > BLACKJACK)
        printf("You win \n");
    else if (total > kTotal)
        printf("You Win \n");
    else if (total < kTotal)
        printf("You Lost \n");
    else if (total == kTotal)
        printf("You Draw \n");
    *endGame = 0;
}

void checkIfBlackJack(int *total, int *kTotal, int *endGame)
{
    if (*total == BLACKJACK && *kTotal == BLACKJACK)
    {
        printf("Draw, Dealer and you have BLACKJACK \n");
        *endGame = 0;
    }
    else if (*total == BLACKJACK)
    {
        printf("You Win, BLACKJACK \n");
        *endGame = 0;
    }
    else if (*kTotal == BLACKJACK)
    {
        printf("You Lose, Dealer has BLACKJACK \n");
        *endGame = 0;
    }
}

void printTable(Card *pHand, Card *dHand, int total, int kTotal, int cAmount, int kAmount, int player, int splited)
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

char getch()
{
    system ("/bin/stty raw");  
    char ret = getchar();
    system ("/bin/stty cooked");
    return ret;
}