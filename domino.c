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

//Private functions
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
	pA->size = 0;
}

piece getPiece(const pieceArray pA, int index) {
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
	if (!(where == BEGIN || where == END)) {
		return ERROR;
	} else {
		if(pA->size!=0&&pA->piece[pA->size-1].side[1]!=piece.side[0]){
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
		if(pA->size!=0&&pA->piece[0].side[0]!=piece.side[1]){
			permutPiece(&piece);
		}
		int i;
		for (i = pA->size - 1; i > 0; i--) {
			pA->piece[i] = pA->piece[i - 1];
		}
		pA->piece[0] = piece;
	} else if (where == END) {
		pA->piece[pA->size - 1] = piece;
	}
	return SUCESS;
}

void permutPiece(piece *piece){
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

int distributePiece(pieceArray *from, pieceArray *to, int piecesPerPlayer){
	int i;
	for(i=0; i<piecesPerPlayer; i++){
		if(from->size == 0){
			return ERROR;
		}
		int rand = randomIndexNumber(from->size);
		piece p = from->piece[rand];
		addPiece(to, END, p);
		removePiece(from, rand);
	}
	return SUCESS;
}

pieceResponse getFirstPlayer(pieceArray playersHand[], int playersNumber, int version){
	pieceResponse result;
	int i,j,k;
	//start checking from bigger possible piece to the smaller one
	for(i=version; i>=0; i--){
		//search all players hands
		for(j=0; j<playersNumber; j++){
			pieceArray player = playersHand[j];
			//search all players pieces
			for(k=0; k<player.size; k++){
				piece piece = player.piece[k];
				if(piece.side[0]==i && piece.side[1]==i){
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

int checkPieceCompatibility(pieceArray pA, piece p){
	if(pA.size==0){
		return END;
	}else{
		int aux = 0;
		if(pA.piece[0].side[0]==p.side[0]||pA.piece[0].side[0]==p.side[1]){
			aux++;
		}
		if(pA.piece[pA.size-1].side[1]==p.side[0]||pA.piece[pA.size-1].side[1]==p.side[1]){
			aux = aux + 2;
		}
		switch(aux){
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

int hasCompatiblePiece(pieceArray table, pieceArray hand){
	int i;
	for(i=0; i<hand.size; i++){
		if(checkPieceCompatibility(table, hand.piece[i])!=NOT_COMPATIBLE){
			return TRUE;
		}
	}
	return FALSE;
}

int buyPiece(pieceArray *from, pieceArray *to){
	if(from->size==0){
		return CANNOT_BUY;
	}
	int randi = randomIndexNumber(from->size);
	if((addPiece(to,END,getPiece(*from,randi))&&removePiece(from,randi))==SUCESS){
		return SUCESS;
	}else{
		return ERROR;
	}
}

compatiblePiecesResponse getCompatiblePieces(pieceArray *table, pieceArray *player){
	compatiblePiecesResponse compatibles;
	compatibles.pieceIndex = NULL;
	int size = 0;
	int i;
	for(i=0; i<player->size; i++){
		if(checkPieceCompatibility(*table,player->piece[i])!=NOT_COMPATIBLE){
			size++;
			assert((compatibles.pieceIndex = realloc(compatibles.pieceIndex,size*sizeof(int)))!=NULL);
			compatibles.pieceIndex[size-1] = i;
		}
	}
	compatibles.size = size;
	return compatibles;
}

void printPieceArray(pieceArray pA) {
	int i;
	for (i = 0; i < pA.size; i++) {
		printf("[%d|%d] ", pA.piece[i].side[0], pA.piece[i].side[1]);
	}
	printf("\n");
}

void printHighlightedPieceArray(pieceArray pA, int index){
	int i;
	for (i = 0; i < pA.size; i++) {
		if(i!=index){
			printf("[%d|%d] ", pA.piece[i].side[0], pA.piece[i].side[1]);
		}else{
			printf(COLOR_GREEN);
			printf("[%d|%d] ", pA.piece[i].side[0], pA.piece[i].side[1]);
			printf(COLOR_RESET);
		}
	}
	printf("\n");
}

void highPrintf(char *format, ...){
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
