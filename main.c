/*
 * main.c
 *
 *  Created on: Nov 11, 2014
 *      Author: giovanni
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "domino.c"
#include "domino.h"

//initial configuration definitions
#define MAX_NAME_LENGHT 25
#define MAX_PLAYERS 4

//name of simulators
#define SIM_1_NAME "McCartney"
#define SIM_2_NAME "Torvalds"
#define SIM_3_NAME "Newton"

//return of userPlay() and computerPlay()
#define WON_GAME 1
#define CONTINUE 0

//sleep time definitions
#define SLEEP_DEF_TIME 1
#define SLEEP_TURN_TIME 2

//return of playAgainScan()
#define DONT_PLAY_AGAIN 0
#define PLAY_AGAIN 1
#define PLAY_AGAIN_NEW_CONFIG 2

//global variables
int version, playersNumber, piecesPerPlayer, turns, idleTurns, currentPlayer,
		mode;
pieceArray buyHand, playersHand[MAX_PLAYERS], table, userBought;
char playersName[4][MAX_NAME_LENGHT];

//shows a welcome message
void welcome();
//scans user name to playersName[0]
void scanName();
//initialize playersName[] with SIM_X_NAME
void initializePlayersName();
//scan version which user want to play
void scanVersion();
//scan number of players
void scanPlayers();
//scan number of pieces per player
void scanPiecesPerPlayer();
//scan mode of game
void scanMode();
//initialize table and distribute pieces to players and the remaining to buy array
void initAndDistributePieces();
//make the first play of game
void firstPlay();
//play the game until it has a winner or game get locked
int playTheGame();
//returns the next player
int getNextPlayer();
//enable UI in user turn and handle user move
int userPlay();
//scan which side of table user want to move the piece
int scanMoveSide();
//algorithm to make simulated moves
int computerPlay();
//print table with label
void printTable();
//print turn number with label
void printTurn();
//print message of invalid input
void printInvalidInput();
//get char in event time
int getcha();
//initialize termios attributes
void initAttr();
//define input attributes back to normal
void closeAttr();
//scan if user want to play the game again with others or same config
int playAgainScan();
//scan answer for yes or not question by arrow input
int yesOrNotScan(char *message);

//stores termios and common attributes
struct termios old_attr, new_attr;

void main(int argc, char *argv[]) {
	//initializes random function with current time
	srand(time(NULL));
	//there's no arguments
	if (argc == 1) {
		welcome();
		scanName();
		//user is playing again
	} else if (argc > 1) {
		strcpy(playersName[0], argv[1]);
	}
	initializePlayersName();
	//first play or user want to play again with new config
	if (argc < 6) {
		scanVersion();
		scanPlayers();
		scanPiecesPerPlayer();
		scanMode();
		//user is playing again with same config
	} else if (argc >= 6) {
		version = atoi(argv[2]);
		playersNumber = atoi(argv[3]);
		piecesPerPlayer = atoi(argv[4]);
		mode = atoi(argv[5]);
	}
	initPieceArray(&userBought);
	initPieceArray(&buyHand);
	generatePieces(&buyHand, version);
	initAndDistributePieces();
	turns = 0;
	idleTurns = 0;
	initPieceArray(&table);
	firstPlay();
	//keeps in this function until someone wins or game get locked
	playTheGame();
	int playAgain = playAgainScan();
	switch (playAgain) {
	char cmd[50];
case (DONT_PLAY_AGAIN):
	//hank
	exit(0);
case (PLAY_AGAIN):
	sprintf(cmd, "./main %s %d %d %d %d", playersName[0], version,
			playersNumber, piecesPerPlayer, mode);
	system(cmd);
	exit(0);
case (PLAY_AGAIN_NEW_CONFIG):
	sprintf(cmd, "./main %s", playersName[0]);
	system(cmd);
	exit(0);
	}
	system("clear");
	exit(0);
}

void welcome() {
	system("clear");
	puts("Bem-vindo ao Dominó 1.0 !");
	sleep(SLEEP_DEF_TIME);
	system("clear");
}

void scanName() {
	system("clear");
	printf("Entre com seu nome: ");
	scanf("%s", playersName[0]);
	system("clear");
}

void initializePlayersName() {
	strcpy(playersName[1], SIM_1_NAME);
	strcpy(playersName[2], SIM_2_NAME);
	strcpy(playersName[3], SIM_3_NAME);
}

void scanVersion() {
	system("clear");
	printf("Entre com a versão do dominó que deseja jogar "
			"(6 -> duplo-seis, 9 -> duplo-nove, 12 -> duplo-doze, "
			"15 -> duplo-quinze ou 18 -> duplo-dezoito): ");
	scanf("%d", &version);
	if (!(version == 6 || version == 9 || version == 12 || version == 15
			|| version == 18)) {
		system("clear");
		printInvalidInput();
		sleep(SLEEP_DEF_TIME);
		scanVersion();
	}
	system("clear");
}

void scanPlayers() {
	system("clear");
	printf(
			"Entre com o número de jogadores, contando com você (min.: 2, máx.: %d): ",
			MAX_PLAYERS);
	scanf("%d", &playersNumber);
	if (playersNumber < 2 || playersNumber > MAX_PLAYERS) {
		system("clear");
		printInvalidInput();
		sleep(SLEEP_DEF_TIME);
		scanPlayers();
	}
	system("clear");
}

void scanMode() {
	system("clear");
	//cleans stdin and consume all chars before
	fflush(stdin);
	while (getchar() != '\n')
		;
	int index = EASY;
	puts("Selecione o modo de jogo:");
	highPrintf("[FÁCIL]\n");
	printf("[MÉDIO]\n[DIFÍCIL]");
	initAttr();
	char c;
	while ((c = getcha()) != '\n') {
		//^[[C is the code of right arrow
		if (c == 'A' && index > EASY) {
			index--;
		} else if (c == 'B' && index < HARD) {
			index++;
		}
		system("clear");
		puts("Selecione o modo de jogo:");
		switch (index) {
		case EASY:
			highPrintf("[FÁCIL]\n");
			printf("[MÉDIO]\n[DIFÍCIL]\n");
			break;
		case MEDIUM:
			printf("[FÁCIL]\n");
			highPrintf("[MÉDIO]\n");
			printf("[DIFÍCIL]\n");
			break;
		case HARD:
			printf("[FÁCIL]\n[MÉDIO]\n");
			highPrintf("[DIFÍCIL]\n");
			break;
		}
	}
	closeAttr();
	mode = index;
	system("clear");
}

void scanPiecesPerPlayer() {
	system("clear");
	printf("Entre com o número de peças por jogador (6 ou 7): ");
	scanf("%d", &piecesPerPlayer);
	if (!(piecesPerPlayer == 6 || piecesPerPlayer == 7)) {
		system("clear");
		printInvalidInput();
		sleep(SLEEP_DEF_TIME);
		scanPiecesPerPlayer();
	}
	system("clear");
}

void initAndDistributePieces() {
	int i;
	for (i = 0; i < playersNumber; i++) {
		initPieceArray(&(playersHand[i]));
		distributePiece(&buyHand, &(playersHand[i]), piecesPerPlayer);
	}
}

void firstPlay() {
	system("clear");
	pieceResponse pR = getFirstPlayer(playersHand, playersNumber, version);
	printTurn();
	//if someone has a double
	if (pR.player != NONE) {
		currentPlayer = pR.player;
		printf("%s é o primeiro a jogar pois possui a maior peça dupla\n",
				playersName[pR.player]);
		addPiece(&table, END, getPiece(playersHand[pR.player], pR.index));
		removePiece(&(playersHand[pR.player]), pR.index);
		turns++;
		//if nobody has double
	} else {
		currentPlayer = 0;
		printf(
				"Como nenhum jogador possui peça dupla você é o primeiro a jogar\n"
						"Selecione através das setas a peça que deseja jogar\n");
		userPlay();
		turns++;
	}
	sleep(SLEEP_TURN_TIME);
	system("clear");
	printTable();
	sleep(SLEEP_DEF_TIME);
	system("clear");
}

int playTheGame() {
	while (1) {
		system("clear");
		currentPlayer = getNextPlayer();
		if (currentPlayer == 0) {
			printf("%s é a sua vez de jogar...\n", playersName[0]);
			sleep(SLEEP_DEF_TIME);
			if (userPlay() == WON_GAME) {
				//finish hand pieces
				puts("Parabéns, você ganhou o jogo!");
				break;
			}
		} else {
			if (computerPlay() == WON_GAME) {
				//finish hand pieces
				printf("%s ganhou o jogo! Mais sorte da próxima vez.\n",
						playersName[currentPlayer]);
				break;
			}
		}
		if (idleTurns == playersNumber) {
			//there's no more piece in buyHand and nobody moved for 1 turn
			system("clear");
			puts("O jogo está fechado!");
			puts("O vencedor será decidido pelo menor número peças...");
			sleep(SLEEP_DEF_TIME);
			int i, min = version, n = 0, mini;
			//checks who has less pieces
			for (i = 0; i < playersNumber; i++) {
				if (playersHand[i].size <= min) {
					min = playersHand[i].size;
					n++;
					mini = i;
				}
			}
			if (n == 1) {
				printf("O vencedor é %s com %d peças.", playersName[mini],
						playersHand[mini].size);
			} else {
				puts(
						"Como houve mais de um jogador com o mesmo número de peças, não houve um vencedor.");
			}
			break;
		}
		turns++;
		system("clear");
	}
	sleep(SLEEP_TURN_TIME);
	system("clear");
	return SUCESS;
}

int playAgainScan() {
	int result = DONT_PLAY_AGAIN;
	if (yesOrNotScan("Deseja jogar novamente ?")) {
		result = PLAY_AGAIN;
		if (yesOrNotScan("Deseja jogar com outra configuração de jogo ?")) {
			result = PLAY_AGAIN_NEW_CONFIG;
		}
	}
	return result;
}

int yesOrNotScan(char *message) {
	int i = TRUE;
	char c;
	system("clear");
	puts(message);
	highPrintf("[SIM]\n");
	printf("[NÃO]\n");
	initAttr();
	while ((c = getcha()) != '\n') {
		if (c == 'A' && i == FALSE) {
			i = TRUE;
		} else if (c == 'B' && i == TRUE) {
			i = FALSE;
		}
		system("clear");
		puts(message);
		if (i) {
			highPrintf("[SIM]\n");
			printf("[NÃO]\n");
		} else {
			printf("[SIM]\n");
			highPrintf("[NÃO]\n");
		}
	}
	closeAttr();
	return i;

}

int getNextPlayer() {
	int result;
	result = currentPlayer + 1;
	if (result == playersNumber) {
		result = 0;
	}
	return result;
}

int userPlay() {
	int index = 0, compatibility = NOT_COMPATIBLE, hasPiece;
	hasPiece = hasCompatiblePiece(table, playersHand[0]);
	system("clear");
	printTurn();
	printTable();
	printf("Sua mão:\n");
	printHighlightedPieceArray(playersHand[0], index);
	if (!hasPiece) {
		printf("[Comprar]\n");
	}
	char c;
	piece p;
	fflush(stdin);
	do {
		hasPiece = hasCompatiblePiece(table, playersHand[0]);
		initAttr();
		while ((c = getcha()) != '\n') {
			//^[[C is the code of right arrow
			if (c == 'C' && index < playersHand[0].size - 1) {
				index++;
				//^[[D is the code of left arrow
			} else if (c == 'D' && index > 0) {
				index--;
			} else if (c == 'A') {
				index = 0;
			} else if (c == 'B' && !hasPiece) {
				index = playersHand[0].size;
			}
			system("clear");
			printTurn();
			printTable();
			printf("Sua mão:\n");
			printHighlightedPieceArray(playersHand[0], index);
			if (!hasPiece && index != playersHand[0].size) {
				printf("[Comprar]\n");
			} else if (!hasPiece && index == playersHand[0].size) {
				highPrintf("[Comprar]\n");
			}
		}
		closeAttr();
		if (hasPiece) {
			compatibility = checkPieceCompatibility(table,
					getPiece(playersHand[0], index));
			//user clicked to buy piece
		} else if (!hasPiece && index == playersHand[0].size) {
			piece hasnt;
			hasnt.side[0] = table.piece[0].side[0];
			hasnt.side[1] = table.piece[table.size - 1].side[1];
			if (!containsPiece(userBought, hasnt)) {
				addPiece(&userBought, END, hasnt);
			}
			int buyResult = buyPiece(&buyHand, &(playersHand[0]));
			if (buyResult == SUCESS) {
				return userPlay();
			} else if (buyResult == CANNOT_BUY) {
				puts("Não há mais peças para serem compradas.");
				sleep(SLEEP_DEF_TIME);
				idleTurns++;
				return CONTINUE;
			}
		}
	} while (compatibility == NOT_COMPATIBLE);
	if (table.size == 0) {
		addPiece(&(playersHand[0]), END, playersHand[0].piece[index]);
	}
	if (compatibility == BOTH) {
		compatibility = scanMoveSide();
	}
	movePiece(&playersHand[0], &table, index, compatibility);
	idleTurns = 0;
	if (playersHand[0].size == 0) {
		return WON_GAME;
	}
	return CONTINUE;
}

int computerPlay() {
	system("clear");
	printf("%s está a jogar...\n", playersName[currentPlayer]);
	sleep(SLEEP_TURN_TIME);
	int hasPiece = hasCompatiblePiece(table, playersHand[currentPlayer]);
	int buyResult;
	while (!hasPiece) {
		buyResult = buyPiece(&buyHand, &playersHand[currentPlayer]);
		if (buyResult == CANNOT_BUY) {
			printf("O jogador não possui uma peça compatível e "
					"não há mais peças para serem compradas"
					", por isso passou seu turno.\n");
			sleep(SLEEP_TURN_TIME);
			idleTurns++;
			return CONTINUE;
		}
		hasPiece = hasCompatiblePiece(table, playersHand[currentPlayer]);
	}
	int index = getBestPieceIndexToMove(table, playersHand[currentPlayer],
			version, userBought, mode);
	int compatibility = checkPieceCompatibility(table,
			playersHand[currentPlayer].piece[index]);
	movePiece(&playersHand[currentPlayer], &table, index, compatibility);
	printf("%s jogou\n", playersName[currentPlayer]);
	printTable();
	sleep(SLEEP_DEF_TIME);
	system("clear");
	if (playersHand[currentPlayer].size == 0) {
		return WON_GAME;
	}
	return CONTINUE;
}

int scanMoveSide() {
	char c;
	puts(
			"Use as setas para indicar se deseja jogar a peça à direita ou à esquerda");
	initAttr();
	while ((c = getcha()) != '\n') {
		//^[[C is the code of right arrow
		if (c == 'C') {
			return END;
			//^[[D is the code of left arrow
		} else if (c == 'D') {
			return BEGIN;
		}
	}
//return config to default
	closeAttr();
	return ERROR;
}

void printTable() {
	printf("Mesa:\n");
	printPieceArray(table);
}

void printTurn() {
	printf("Rodada %d:\n", turns);
}

void printInvalidInput() {
	puts("Entrada inválida");
}

int getcha() {
	int c;
	new_attr.c_lflag &= ~ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
	c = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
	return c;
}

void initAttr(void) {
	tcgetattr(0, &old_attr);
	new_attr = old_attr;
	new_attr.c_lflag &= ~ICANON;
	new_attr.c_cc[VTIME] = 0;
	new_attr.c_cc[VMIN] = 1;
}

void closeAttr(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &old_attr);
}
