/*****************************************************************************************
/* File: Game.cpp
/* Desc: General class for the game
/*
/* gametuto.com - Javier L�pez L�pez (javilop.com)
/*
/*****************************************************************************************
/*
/* Creative Commons - Attribution 3.0 Unported
/* You are free:
/*	to Share � to copy, distribute and transmit the work
/*	to Remix � to adapt the work
/*
/* Under the following conditions:
/* Attribution. You must attribute the work in the manner specified by the author or licensor 
/* (but not in any way that suggests that they endorse you or your use of the work).
/*
/*****************************************************************************************/

// ------ Includes -----
#ifndef LINUX
#include <windows.h>
#endif
#include "Game.h"
#include <iostream>


using namespace std;
/* 
======================================									
Init
====================================== 
*/
Game::Game(Board *pBoard, Pieces *pPieces, IO *pIO, int pScreenHeight, vector<double> pWeights) 
{
	mScreenHeight = pScreenHeight;

	// GA weights
	weights = pWeights;

	// Get the pointer to the Board and Pieces classes
	mBoard = pBoard;
	mPieces = pPieces;
	mIO = pIO;

	// Game initialization
	InitGame ();
}


/* 
======================================									
Get a random int between to integers

Parameters:
>> pA: First number
>> pB: Second number
====================================== 
*/
int Game::GetRand (int pA, int pB)
{
	return rand () % (pB - pA + 1) + pA;
}


/* 
======================================									
Initial parameters of the game
====================================== 
*/
void Game::InitGame()
{
	// Init random numbers
	srand ((unsigned int) time(NULL));

	// First piece
	mPiece			= GetRand (0, 6);
	mRotation		= GetRand (0, 3);
	mPosX 			= (BOARD_WIDTH/2) + mPieces->GetXInitialPosition (mPiece, mRotation);
	mPosY 			= mPieces->GetYInitialPosition (mPiece, mRotation);

	//  Next piece
	mNextPiece 		= GetRand (0, 6);
	mNextRotation 	= GetRand (0, 3);
	mNextPosX 		= BOARD_WIDTH + 5;
	mNextPosY 		= 5;	
}


/* 
======================================									
Create a random piece
====================================== 
*/
void Game::CreateNewPiece()
{
	// The new piece
	mPiece			= mNextPiece;
	mPosX 			= (BOARD_WIDTH / 2) + mPieces->GetXInitialPosition (mPiece, mRotation);

	double minCost = INT_MAX;
	for(int i = -1; i < BOARD_WIDTH-1; i++) { //For each horizontal position
		for(int j = 0; j < 4; j++) { //For each rotation
			double newCost = costOfPosAndRot(i, j, mPiece);
			if(newCost < minCost) {
				minCost = newCost;
				mPosX = i;
				mRotation = j;
				cout << "New min: " << minCost << ", posX: " << mPosX << ", rotation: " << mRotation << endl; 
			}
		}
	}
	//mPosX = -2;
	//mRotation		= mNextRotation;
	//mPosX 			= (BOARD_WIDTH / 2) + mPieces->GetXInitialPosition (mPiece, mRotation);
	mPosY 			= mPieces->GetYInitialPosition (mPiece, mRotation);
	//if (!mBoard->IsPossibleMovement(mPosX, mPosY, mPiece, mRotation)) {
	//	mPosX = (BOARD_WIDTH / 2) + mPieces->GetXInitialPosition (mPiece, mRotation);
	//}

	// Random next piece
	mNextPiece 		= GetRand (0, 6);
	mNextRotation 	= GetRand (0, 3);
	
}

int Game::findFirstRowWithBlock() {
	for(int j = 0; j < BOARD_HEIGHT; j++) {
		for(int i = 0; i < BOARD_WIDTH; i++) {
			if(!mBoard->IsFreeBlock(i, j)){
				return j;
			}
		}
	}
}

double Game::costOfPosAndRot(int positionX, int rotation, int piece) {
	double cost = 0;
	int temp_mPosY = 0;
	
	while (mBoard->IsPossibleMovement(positionX, temp_mPosY, piece, rotation)) { temp_mPosY++; }
	
	mBoard->StorePiece (positionX, temp_mPosY - 1, piece, rotation);

	//Calculate cost
	//int dmax = (BOARD_HEIGHT-findFirstRowWithBlock())*BOARD_WIDTH;
	for(int i = findFirstRowWithBlock(); i < BOARD_HEIGHT; i++){
		for(int j = 0; j < BOARD_WIDTH; j++){
			if(mBoard->IsFreeBlock(j, i)){
				cost += weights[0];
				cost += weights[1]*(BOARD_HEIGHT-i);
			}
			cost -= weights[2]*mBoard->CheckLinesDeleted();
		}
	}

	//Remove peice
//	if(stored){
		mBoard->RemovePiece(positionX, temp_mPosY - 1, piece, rotation);
//	} else {
//		return INT_MAX;
//	}

	if (mBoard->IsPossibleMovement(positionX, temp_mPosY - 1, piece, rotation)) {
		return cost;
	} else {
		return INT_MAX;
	}
}

/* 
======================================									
Draw piece

Parameters:

>> pX:		Horizontal position in blocks
>> pY:		Vertical position in blocks
>> pPiece:	Piece to draw
>> pRotation:	1 of the 4 possible rotations
====================================== 
*/
void Game::DrawPiece (int pX, int pY, int pPiece, int pRotation)
{
	color mColor;				// Color of the block 

	// Obtain the position in pixel in the screen of the block we want to draw
	int mPixelsX = mBoard->GetXPosInPixels (pX);
	int mPixelsY = mBoard->GetYPosInPixels (pY);

	// Travel the matrix of blocks of the piece and draw the blocks that are filled
	for (int i = 0; i < PIECE_BLOCKS; i++)
	{
		for (int j = 0; j < PIECE_BLOCKS; j++)
		{
			// Get the type of the block and draw it with the correct color
			switch (mPieces->GetBlockType (pPiece, pRotation, j, i))
			{
				case 1: mColor = GREEN; break;	// For each block of the piece except the pivot
				case 2: mColor = BLUE; break;	// For the pivot
			}
			
			if (mPieces->GetBlockType (pPiece, pRotation, j, i) != 0)
				mIO->DrawRectangle	(mPixelsX + i * BLOCK_SIZE, 
									mPixelsY + j * BLOCK_SIZE, 
									(mPixelsX + i * BLOCK_SIZE) + BLOCK_SIZE - 1, 
									(mPixelsY + j * BLOCK_SIZE) + BLOCK_SIZE - 1, 
									mColor);
		}
	}
}


/* 
======================================									
Draw board

Draw the two lines that delimit the board
====================================== 
*/
void Game::DrawBoard ()
{
	// Calculate the limits of the board in pixels	
	int mX1 = BOARD_POSITION - (BLOCK_SIZE * (BOARD_WIDTH / 2)) - 1;
	int mX2 = BOARD_POSITION + (BLOCK_SIZE * (BOARD_WIDTH / 2));
	int mY = mScreenHeight - (BLOCK_SIZE * BOARD_HEIGHT);
	
	// Check that the vertical margin is not to small
	//assert (mY > MIN_VERTICAL_MARGIN);

	// Rectangles that delimits the board
	mIO->DrawRectangle (mX1 - BOARD_LINE_WIDTH, mY, mX1, mScreenHeight - 1, BLUE);
	mIO->DrawRectangle (mX2, mY, mX2 + BOARD_LINE_WIDTH, mScreenHeight - 1, BLUE);
	
	// Check that the horizontal margin is not to small
	//assert (mX1 > MIN_HORIZONTAL_MARGIN);

	// Drawing the blocks that are already stored in the board
	mX1 += 1;
	for (int i = 0; i < BOARD_WIDTH; i++)
	{
		for (int j = 0; j < BOARD_HEIGHT; j++)
		{	
			// Check if the block is filled, if so, draw it
			if (!mBoard->IsFreeBlock(i, j))	
				mIO->DrawRectangle (	mX1 + i * BLOCK_SIZE, 
										mY + j * BLOCK_SIZE, 
										(mX1 + i * BLOCK_SIZE) + BLOCK_SIZE - 1, 
										(mY + j * BLOCK_SIZE) + BLOCK_SIZE - 1, 
										RED);
		}
	}	
}


/* 
======================================									
Draw scene

Draw all the objects of the scene
====================================== 
*/
void Game::DrawScene ()
{
	DrawBoard ();													// Draw the delimitation lines and blocks stored in the board
	DrawPiece (mPosX, mPosY, mPiece, mRotation);					// Draw the playing piece
	DrawPiece (mNextPosX, mNextPosY, mNextPiece, mNextRotation);	// Draw the next piece
}
