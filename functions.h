#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define BLACKJACK 21
#define MAX_DECK 22
#define MAX_LEN 30
#define PLAYER 0
#define DEALER 1


static char *cardFace[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

typedef struct 
{
    const char *face;
    int value;
} Card;

typedef struct 
{
    char name[MAX_LEN];
    int money;
    int games;
    int wonGames;
} Player;

typedef struct 
{
    Player player;

    int moneyBet;

    int pIndex;
    int pTotal;
    
    int dIndex;
    int dTotal;

    int checkNormalWin;
    int splited;
    int splitGame;
    int blackJack;
    int doubledDown;
} Info;

void mainMenu(Card *pHand, Card *dHand, Info *info, Player *player);
void gameLogic(Card *pHand, Card *dHand, Info *info, Player *player);
void splitGameLogic(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK]);

void mainLoginFunction(Info *info);
void registerLogic(Info *info, int *logged);
void login(Info *info, int *logged);

void mainGameLoop(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK], int endGame);
void hitLogic(Card *pHadn, Info *info, int aceIndex[][MAX_DECK], int *endGame);
void standLogic(Card *dHand, Info *info, int aceIndex[][MAX_DECK]);
void doubleDown(Card *pHand, Card *dHand, Info *info, int aceIndex[][MAX_DECK]);

void draw(Card *hand, int index, int aceIndex[][MAX_DECK], int *total, int player);
void FirstTwoDraws(Card *hand, int aceIndex[][MAX_DECK], int *total, int player);

void checkWinner(int total, int dTotal, Info *info);
void checkSplitWinners(int total, Info *info);
void checkIfBlackJack(int total, int dTotal, int *endGame, Info *info);
void checkBetMoney(Info *info);

void devInfo(int aceIndex[][MAX_DECK], Info *info, Card *pHand, Card *dHand);

void resetSplitValues(int aceIndex[][MAX_DECK], Card *pHand, Info *info, int copyOfFirst, int *endGame);
void structVariablesInit(Info *info);
void saveToFile(Info *info);

void showAllPlayers();
void printHand(Card *hand, int cardOnHand);
void printTable(Card *pHand, Card *dHand, Info *info, int player);

char getch();
