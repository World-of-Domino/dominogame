/*
 * domino.c
 *
 *  Created on: Nov 11, 2014
 *      Author: giovanni
 */
#include "domino.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

//ANSI colors code
#define COLOR_RESET "\x1B[0m"
#define COLOR_GREEN "\x1B[32m"

unsigned long long int typedef bign;

//header of private functions
//creates a pA with intersection of pieces user bought and compatibles pieces
pieceArray getPiecesUserBought(pieceArray playerHand, pieceArray userBought,
		piece matchingPiece);
//creates a pA with intersection of doubles and compatible pieces
pieceArray getDoubles(pieceArray pA);
//search a piece inside a pieceArray and return index or NONE if hasn't
int getPieceIndex(pieceArray pA, piece p);
//checks if pieces equals with permutation
int pieceEquals(piece p, piece q);
//creates a pA with pieces that appears  more times
pieceArray getMostRepeatedSides(pieceArray pA, int version);
//returns max times of side appears inside a pieceArray
int getMaxTimesOfRepetition(pieceArray pA, int version);
//get repetition times of certain side inside a pieceArray
int getRepetitionTimes(pieceArray pA, int s);
//get piece that matches the table
piece getMatchingPiece(pieceArray table);
//returns an index between 0 and n-1
int randomIndexNumber(int n);
//returns the number of possible combinations of n different elements taken two to two
bign combination(int n, int p);
//function factorial defined in R+
bign factorial(int n);

void initPiece(piece *p, int a, int b) {
	p->side[0] = a;
	p->side[1] = b;
}

void initPieceArray(pieceArray *pA) {
	pA->piece = NULL;
	pA->size = 0;
}

piece getPiece(pieceArray pA, int index) {
	if (index < 0 || index >= pA.size) {
		piece ret;
		ret.side[0] = ERROR;
		ret.side[1] = ERROR;
		return ret;
	} else {
		return pA.piece[index];
	}
}

int setPiece(pieceArray *pA, int index, piece piece) {
	if (index < 0 || index >= pA->size) {
		return ERROR;
	} else {
		pA->piece[index] = piece;
		return SUCESS;
	}
}

int addPiece(pieceArray *pA, int where, piece piece) {
	if (where == BOTH) {
		where = END;
	}
	if (!(where == BEGIN || where == END)) {
		return ERROR;
	} else {
		if (pA->size != 0 && pA->piece[pA->size - 1].side[1] != piece.side[0]) {
			permutPiece(&piece);
		}
		if ((pA->piece = realloc(pA->piece, sizeof(piece) * (pA->size + 1)))
				== NULL) {
			return ERROR;
		}
		pA->size++;
	}
	if (where == BEGIN) {
		//increase all piece index
		if (pA->size != 0 && pA->piece[0].side[0] != piece.side[1]) {
			permutPiece(&piece);
		}
		int i;
		for (i = pA->size - 1; i > 0; i--) {
			pA->piece[i] = pA->piece[i - 1];
		}
		//insert piece first index
		pA->piece[0] = piece;
	} else if (where == END) {
		//insert piece last index
		pA->piece[pA->size - 1] = piece;
	}
	return SUCESS;
}

void permutPiece(piece *piece) {
	int aux = piece->side[0];
	piece->side[0] = piece->side[1];
	piece->side[1] = aux;
}

int removePiece(pieceArray *pA, int index) {
	if (index < 0 || index >= pA->size) {
		return ERROR;
	} else {
		//decreases all pieces index from removed to the end
		int i;
		for (i = index; i < pA->size; i++) {
			pA->piece[i] = pA->piece[i + 1];
		}
		if (pA->size - 1 == 0) {
			free(pA->piece);
			pA->piece = NULL;
			pA->size--;
			return SUCESS;
		}
		if ((pA->piece = realloc(pA->piece, sizeof(piece) * (pA->size - 1)))
				== NULL) {
			return ERROR;
		} else {
			pA->size--;
			return SUCESS;
		}
	}
}

int generatePieces(pieceArray *pA, int version) {
	//adds 1 to version because elements can make combination to itself
	bign n = combination(version + 1, 2);
	if (n == ERROR) {
		return ERROR;
	} else {
		int i, j;
		for (i = 0; i <= version; i++) {
			for (j = i; j <= version; j++) {
				piece p;
				initPiece(&p, i, j);
				addPiece(pA, END, p);
			}
		}
		return SUCESS;
	}
}

int distributePiece(pieceArray *from, pieceArray *to, int piecesPerPlayer) {
	int i;
	for (i = 0; i < piecesPerPlayer; i++) {
		if (from->size == 0) {
			return ERROR;
		}
		int rand = randomIndexNumber(from->size);
		piece p = from->piece[rand];
		movePiece(from, to, rand, END);
	}
	return SUCESS;
}

pieceResponse getFirstPlayer(pieceArray playersHand[], int playersNumber,
		int version) {
	pieceResponse result;
	int i, j, k;
	//start checking from bigger possible piece to the smaller one
	for (i = version; i >= 0; i--) {
		//search all players hands
		for (j = 0; j < playersNumber; j++) {
			pieceArray player = playersHand[j];
			//search all players pieces
			for (k = 0; k < player.size; k++) {
				piece piece = player.piece[k];
				if (piece.side[0] == i && piece.side[1] == i) {
					result.player = j;
					result.index = k;
					return result;
				}
			}
		}
	}
	result.player = NONE;
	result.index = NONE;
	return result;
}

int checkPieceCompatibility(pieceArray pA, piece p) {
	if (pA.size == 0) {
		return END;
	} else {
		int aux = 0;
		if (pA.piece[0].side[0] == p.side[0]
				|| pA.piece[0].side[0] == p.side[1]) {
			aux++;
		}
		if (pA.piece[pA.size - 1].side[1] == p.side[0]
				|| pA.piece[pA.size - 1].side[1] == p.side[1]) {
			aux = aux + 2;
		}
		switch (aux) {
		case 0:
			return NOT_COMPATIBLE;
		case 1:
			return BEGIN;
		case 2:
			return END;
		case 3:
			return BOTH;
		}
	}
}

int hasCompatiblePiece(pieceArray table, pieceArray hand) {
	int i;
	for (i = 0; i < hand.size; i++) {
		if (checkPieceCompatibility(table, hand.piece[i]) != NOT_COMPATIBLE) {
			return TRUE;
		}
	}
	return FALSE;
}

int buyPiece(pieceArray *from, pieceArray *to) {
	//there's no more pieces to be bought
	if (from->size == 0) {
		return CANNOT_BUY;
	}
	int randi = randomIndexNumber(from->size);
	if ((addPiece(to, END, getPiece(*from, randi)) && removePiece(from, randi))
			== SUCESS) {
		return SUCESS;
	} else {
		return ERROR;
	}
}

int movePiece(pieceArray *from, pieceArray *to, int pieceIndex, int where) {
	if (addPiece(to, where, from->piece[pieceIndex])
			&& removePiece(from, pieceIndex)) {
		return SUCESS;
	} else {
		return FALSE;
	}
}

int getBestPieceIndexToMove(pieceArray table, pieceArray simHand, int version,
		pieceArray userBought, int mode) {
	//get all compatibles pieces
	pieceArray compatibles;
	initPieceArray(&compatibles);
	int i;
	for (i = 0; i < simHand.size; i++) {
		if (checkPieceCompatibility(table, simHand.piece[i]) != NOT_COMPATIBLE) {
			addPiece(&compatibles, END, simHand.piece[i]);
		}
	}
	//if mode of game is EASY stops algorithm by here returning first piece
	if (mode == EASY) {
		return getPieceIndex(simHand, compatibles.piece[0]);
	}
	//if there's doubles, remove all non-doubles pieces of compatibles
	pieceArray doubles = getDoubles(compatibles);
	if (doubles.size != 0) {
		compatibles = doubles;
	}
	//if mode of game is MEDIUM stops algorithm by here returning first piece
	if (mode == MEDIUM) {
		return getPieceIndex(simHand, compatibles.piece[0]);
	}
	//get all pieces that has side B user already bought
	piece matchingPiece = getMatchingPiece(table);
	pieceArray bought = getPiecesUserBought(compatibles, userBought,
			matchingPiece);
	if (bought.size != 0) {
		compatibles = bought;
	}
	//get pieces with bigger repetition number
	pieceArray repeatedSides = getMostRepeatedSides(compatibles, version);
	if (repeatedSides.size != 0) {
		compatibles = repeatedSides;
	}
	//return for HARD mode
	return getPieceIndex(simHand, compatibles.piece[0]);
}

piece getMatchingPiece(pieceArray table) {
	piece matchingPiece;
	matchingPiece.side[0] = table.piece[0].side[0];
	matchingPiece.side[1] = table.piece[table.size - 1].side[1];
	return matchingPiece;
}

pieceArray getPiecesUserBought(pieceArray playerHand, pieceArray userBought,
		piece matchingPiece) {
	pieceArray response;
	initPieceArray(&response);
	int i, j;
	for (i = 0; i < playerHand.size; i++) {
		piece p = playerHand.piece[i];
		if (p.side[0] == matchingPiece.side[0]
				|| p.side[0] == matchingPiece.side[1]) {
			p.side[0] = -1;
		}
		if (p.side[1] == matchingPiece.side[0]
				|| p.side[1] == matchingPiece.side[1]) {
			p.side[1] = -1;
		}
		for (j = 0; j < userBought.size; j++) {
			piece p2 = userBought.piece[j];
			if (sideEquals(p, p2)
					&& !containsPiece(response, playerHand.piece[i])) {
				addPiece(&response, END, playerHand.piece[i]);
			}
		}
	}

	return response;
}

int sideEquals(piece p, piece q) {
	if (p.side[0] == q.side[0] || p.side[0] == q.side[1]
			|| p.side[1] == q.side[0] || p.side[1] == q.side[1]) {
		return TRUE;
	}
	return FALSE;
}

pieceArray getDoubles(pieceArray pA) {
	pieceArray doubles;
	initPieceArray(&doubles);
	int i;
	for (i = 0; i < pA.size; i++) {
		piece p = pA.piece[i];
		if (p.side[0] == p.side[1]) {
			addPiece(&doubles, END, p);
		}
	}
	return doubles;
}

pieceArray getMostRepeatedSides(pieceArray pA, int version) {
	pieceArray mostRepeated;
	initPieceArray(&mostRepeated);
	int i, j;
	int maxRepeats = getMaxTimesOfRepetition(pA, version);
	//check max number of ocurrences of some side and remove other sides with less repetitions
	for (i = 0; i <= version; i++) {
		if (getRepetitionTimes(pA, i) == maxRepeats) {
			for (j = 0; j < pA.size; j++) {
				piece p = pA.piece[j];
				if (((p.side[0] == i) || (p.side[1] == i))
						&& !containsPiece(mostRepeated, p)) {
					addPiece(&mostRepeated, END, p);
				}
			}
		}
	}
	return mostRepeated;
}

int containsPiece(pieceArray pA, piece p) {
	int i;
	for (i = 0; i < pA.size; i++) {
		if (pieceEquals(pA.piece[i], p)) {
			return TRUE;
		}
	}
	return FALSE;
}

int getMaxTimesOfRepetition(pieceArray pA, int version) {
	int i, j, max = 0, repeats;
	for (i = 0; i <= version; i++) {
		repeats = getRepetitionTimes(pA, i);
		if (repeats > max) {
			max = repeats;
		}
	}
	return max;
}

int getRepetitionTimes(pieceArray pA, int s) {
	int i, result = 0;
	for (i = 0; i < pA.size; i++) {
		piece p = pA.piece[i];
		if (p.side[0] == s || p.side[1] == s) {
			result++;
		}
	}
	return result;
}

int getPieceIndex(pieceArray pA, piece p) {
	int i;
	for (i = 0; i < pA.size; i++) {
		if (pieceEquals(pA.piece[i], p)) {
			return i;
		}
	}
	return NONE;
}

int pieceEquals(piece p, piece q) {
	if ((p.side[0] == q.side[0] && p.side[1] == q.side[1])
			|| (p.side[1] == q.side[0] && p.side[0] == q.side[1])) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void printPieceArray(pieceArray pA) {
	int i;
	for (i = 0; i < pA.size; i++) {
		printf("[%d|%d] ", pA.piece[i].side[0], pA.piece[i].side[1]);
	}
	printf("\n");
}

void printHighlightedPieceArray(pieceArray pA, int index) {
	int i;
	for (i = 0; i < pA.size; i++) {
		if (i != index) {
			printf("[%d|%d] ", pA.piece[i].side[0], pA.piece[i].side[1]);
		} else {
			printf(COLOR_GREEN);
			printf("[%d|%d] ", pA.piece[i].side[0], pA.piece[i].side[1]);
			printf(COLOR_RESET);
		}
	}
	printf("\n");
}

void highPrintf(char *format, ...) {
	//reimplementation of printf with green color output
	va_list argptr;
	va_start(argptr, format);
	printf(COLOR_GREEN);
	vprintf(format, argptr);
	printf(COLOR_RESET);
	va_end(argptr);
}

int randomIndexNumber(int n) {
	return (rand() % n);
}

bign combination(int n, int p) {
	//check if function is defined in n and p
	if (n >= p && p > 0) {
		bign result = 1;
		int i, bp, wasDiv = 0;
		bign div;
		//pratices to avoid overflow
		if (p > (n - p)) {
			bp = p;
			div = factorial(n - p);
		} else {
			bp = n - p;
			div = factorial(p);
		}
		for (i = n; i > bp; i--) {
			result *= i;
			if (!wasDiv && !(result % div)) {
				result /= div;
				wasDiv = 1;
			}
		}
		return result;
	} else {
		return ERROR;
	}
}

bign factorial(int n) {
	//recursive implementation of factorial
	if (n > 0) {
		return n * factorial(n - 1);
	} else if (!n) {
		return 1;
	} else {
		return ERROR;
	}
}
