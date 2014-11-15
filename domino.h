/*
 * domino.h
 *
 *  Created on: Nov 11, 2014
 *      Author: giovanni
 */

#ifndef DOMINO_H_
#define DOMINO_H_

//definitions for functions returns
#define ERROR -1
#define SUCESS 1

#define BEGIN -1
#define END 1

#define NOT_COMPATIBLE 0
#define BOTH 2

#define EMPTY -1

#define NONE -1

#define TRUE 1
#define FALSE 0

#define CANNOT_BUY 0

struct {
	int side[2];
}typedef piece;

struct {
	piece *piece;
	int size;
}typedef pieceArray;

struct {
	int player;
	int index;
}typedef pieceResponse;

struct {
	int *pieceIndex;
	int size;
} typedef compatiblePiecesResponse;

struct {
	int *pieces;
	int size;
}typedef statistics;

/* Initializes a pieceArray with size 0
 *
 * @param *pA 	pointer to a pieceArray already allocated
 */
void initPieceArray(pieceArray *pA);

/* Get piece from pieceArray in indicated index
 *
 * @param p 	pieceArray of desired piece
 * @param index index of desired piece in pieceArray
 * @return 		returns the piece if index exists, else returns a piece with ERROR in both sides
 */
piece getPiece(pieceArray p, int index);

/* Set piece to indicated index position in a pieceArray
 *
 * @param *pA 	pointer to a pieceArray already allocated
 * @param index index to substitution
 * @param piece piece to place in indicated index
 * @return 		returns SUCESS if it was possible to substitute, else returns ERROR
 */
int setPiece(pieceArray *pA, int index, piece piece);

/* Add a piece to BEGIN or END of a pieceArray and increments size of array
 *
 * @param *pA 	pointer to a pieceArray already allocated
 * @param where place to add the new piece has to be BEGIN or END
 * @param piece piece to add in indicated location
 * @return 		returns SUCESS if it was possible to add piece, else returns ERROR
 */
int addPiece(pieceArray *pA, int where, piece piece);

void permutPiece(piece *piece);

/* Removes a piece from a pieceArray and decrements size of array
 *
 * @param *pA 	pointer to a pieceArray already allocated
 * @param index index of piece desired to be removed
 * @return 		returns SUCESS if it was possible to remove, else returns ERROR and don't change pA instance
 */
int removePiece(pieceArray *pA, int index);

/* Generate all combination of pieces for required Domino version and instantiates in pieceArray
 *
 * @param *pA 		pointer to a pieceArray already allocated
 * @param version 	version of playing Domino
 * @return 			SUCESS if it was possible to generate pieces and instantiate in pA, else returns ERROR
 */
int generatePieces(pieceArray *pA, int version);

/* Distribute randomly pieces from all pieces to players hand
 *
 * @param *from				pointer to pieceArray which contains all pieces
 * @param *to	 			pointer to pieceArray which will get the pieces
 * @param piecesPerPlayer	number of pieces per player
 * @return 					returns SUCESS if it was possible to distribute all required pieces, else returns ERROR
 */
int distributePiece(pieceArray *from, pieceArray *to, int piecesPerPlayer);

/* Check by the bigger double piece rule who is the first player
 *
 * @param playersHand[] vector of all players pieceArray
 * @param playersNumber size of playersHand
 * @param version		version of playing Domino
 * @return 				return pieceResponse with the index of the player who has the bigger double or NONE if no player has it
 */
pieceResponse getFirstPlayer(pieceArray playersHand[], int playersNumber,
		int version);

/* Checks if a piece is compatible with the table
 *
 * @param pA 	pieceArray to check piece compatibility
 * @param p 	piece to be checked
 * @return 		returns the status of compatibility, it is, NOT_COMPATIBLE, BEGIN, END or BOTH
 */
int checkPieceCompatibility(pieceArray pA, piece p);

int hasCompatiblePiece(pieceArray table, pieceArray hand);

int buyPiece(pieceArray *from, pieceArray *to);

compatiblePiecesResponse getCompatiblePieces(pieceArray *table, pieceArray *player);

/* Print a pieceArray human-readable in the console
 *
 * @param pA pieceArray to be printed
 */
void printPieceArray(pieceArray pA);

/* Print a pieceArray human-readable in the console with indicated index highlighted
 *
 * @param pA 	pieceArray to be printed
 * @param index	index of piece to be highlighted
 */
void printHighlightedPieceArray(pieceArray pA, int index);

void highPrintf(char *format, ...);

#endif /* DOMINO_H_ */
