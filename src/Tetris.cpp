// Tetris.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <ctime>
#include <math.h>

// also, make a "checkIfPiecefits(pieceType, posx, posy, roation)" function

using namespace std;

enum input { LEFT, RIGHT, NONE, RESTART, END, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN };
enum piece { I, O, T, S, Z, J, L, ACTIVE, BLANK };

const unsigned short BLACK = 0;
const unsigned short DARKBLUE = FOREGROUND_BLUE;
const unsigned short DARKGREEN = FOREGROUND_GREEN;
const unsigned short DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE;
const unsigned short DARKRED = FOREGROUND_RED;
const unsigned short DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE;
const unsigned short DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
const unsigned short DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const unsigned short GRAY = FOREGROUND_INTENSITY;
const unsigned short BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
const unsigned short GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
const unsigned short CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
const unsigned short RED = FOREGROUND_INTENSITY | FOREGROUND_RED;
const unsigned short MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
const unsigned short YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
const unsigned short WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

struct tile {
	bool enabled;
	bool active;
	piece ID;

	tile() : enabled(0), active(0), ID(BLANK) { }
	tile(bool a, bool b, piece c) : enabled(a), active(b), ID(c) { }
};

unsigned short randColor();
unsigned short idToColor(int);
void makeRandomPiece();
void printUpdatedGrid();
void printFullGrid();
void showConsoleCursor(bool);
void drawBox(int, int);
void eraseBox(int, int);
void setCursorPos(int, int);
void setConsoleColor(unsigned short);
void updateGrid();
void createPiece(piece);
void setNextGrid();
void setPiece();
void drawBoundaries();
void drawHoldingPiece(piece, piece);
void drawHoldingBox();
void makePiece(int, int, piece);
void deletePiece(int, int);
void rotatePieceCW();
void rotatePieceCCW();
void cls();
bool getInput(input*);
bool movePieceRight();
bool movePieceLeft();

static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

const int charPixelSizeH = 14;
const int charPixelSizeW = 7;
const int boxW = 5;
const int boxH = 2;
const int gridW = 10; // the width of the grid (grid)
const int gridH = 23;
const int holdingBoxW = (boxW * 4) + 4; // the widht of the holding box (spaces)
const int holdingBoxH = (boxH * 4);
const int blankID = -1;
const int updatesPerSecond = 100;
const int maxSpeed = 5;
const int defaultSpeed = 30; // (100 / x)

input i = NONE;
piece p = BLANK;
piece holding = BLANK;

int speed = 10;
int timer = 0;
int r = 0;
int offsetY; // the offset of the main grid
int offsetX;
int offsetHoldingX; // the offset of the holding box
int offsetHoldingY;
int centerX;
int centerY;
int rowCleared;

bool holdingDown;
bool holdPiece;
bool rotateCW;
bool rotateCCW;
bool pieceMoved;
bool pieceSet;

unsigned char box = 219;

tile grid[gridW][gridH];
bool nextGrid[gridW][gridH];

int main()
{
	srand(unsigned int(time(NULL)));			// randomizes rand
	system("mode con COLS=700");  // fullscreen
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
	SendMessage(GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);
	showConsoleCursor(false);	// hides cursor

START:

	drawBoundaries();
	//drawHoldingBox();

	for (int y = 0; y < gridH; y++) {  // initialize grids
		for (int x = 0; x < gridW; x++) {
			grid[x][y] = tile(false, false, BLANK);
			nextGrid[x][y] = false;
		}
	}

	pieceSet = false;

	while (true) {
		
		getInput(&i);
		pieceMoved = false;
		rowCleared = -1;

		if (holdingDown) {
			speed = maxSpeed;
		}
		else {
			speed = defaultSpeed;
		}

		if (rotateCW ^ rotateCCW) {
			if (rotateCW)
				rotatePieceCW();

			else if (rotateCCW)
				rotatePieceCCW();
		}

		if (i != NONE) {
			switch (i) {
			case LEFT:
				movePieceLeft();
				break;
			case RIGHT:
				movePieceRight();
				break;
			case END:
				goto STOP;
				break;
			case RESTART:
				cls();
				goto START;
				break;
			case ONE:	// I
				createPiece(I);
				drawHoldingPiece(holding, I);
				break;
			case TWO:	// O
				createPiece(O);
				drawHoldingPiece(holding, O);
				break;
			case THREE:	// T
				createPiece(T);
				drawHoldingPiece(holding, T);
				break;
			case FOUR:	// S
				createPiece(S);
				drawHoldingPiece(holding, S);
				break;
			case FIVE:	// Z
				createPiece(Z);
				drawHoldingPiece(holding, Z);
				break;
			case SIX:	// J
				createPiece(J);
				drawHoldingPiece(holding, J);
				break;
			case SEVEN:	// L
				createPiece(L);
				drawHoldingPiece(holding, L);
				break;
			}
		} 
		if (timer % speed == 0) {
			updateGrid();
		}

		if (holdPiece) {
			// delete Current Piece
			for (int y = 0; y < gridH; y++) {
				for (int x = 0; x < gridW; x++) {
					if (grid[x][y].ID == ACTIVE) {
						deletePiece(x, y);
					}
				}
			}

			piece temp = holding;
			holding = p;

			if (temp == BLANK) {
				makeRandomPiece();
			}
			else {
				createPiece(temp);
			}
		}

		// special print function for when a row is cleared
		if (rowCleared == -1) {
			printUpdatedGrid();
		}
		else {
			printFullGrid();
		}

		Sleep(1000 / updatesPerSecond);
		timer++;
	}

STOP:
	return 0;
}

void drawHoldingPiece(piece oldPiece, piece newPiece) {
	int xref = offsetHoldingX + (2 * boxW) + 2;
	int yref = offsetHoldingY + boxH;

	switch (oldPiece) {
	case I:
		setConsoleColor(CYAN);
		for (int x = 0; x < 4; x++) {
			eraseBox(xref + (x * boxW), yref + 1);
		}
		break;
	case O:
	case T:
	case S:
	case Z:
	case J:
	case L:
		break;
	}

	switch (newPiece) {
	case I:
		setConsoleColor(CYAN);
		for (int x = -1; x < 3; x++) {
			drawBox(xref + (x * boxW), yref + 1);
		}
		break;
	case O:
		setConsoleColor(YELLOW);
		for (int x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				drawBox(xref + (x * boxW), yref + (boxH * y));
			}
		}
		break;
	case T:
	case S:
	case Z:
	case J:
	case L:
		break;
	}
}

void makeRandomPiece() {
	piece p2;
	int type = (rand() % 7);

	switch (type) {
	case 0:
		p2 = I;
		break;
	case 1:
		p2 = O;
		break;
	case 2:
		p2 = T;
		break;
	case 3:
		p2 = S;
		break;
	case 4:
		p2 = Z;
		break;
	case 5:
		p2 = J;
		break;
	case 6:
		p2 = L;
		break;
	}

	createPiece(p2);
}

void rotatePieceCW() {

	if (pieceSet) {
		return;
	}

	piece id = ACTIVE;

	switch (p) {
		case I:
		{
			if (r == 0) {
				if (centerY - 1 < 0 || centerY + 2 >= gridH) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 1][centerY - 1].enabled;
				bool b1 = grid[centerX + 1][centerY + 1].enabled;
				bool b2 = grid[centerX + 1][centerY + 2].enabled;


				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX, centerY);
					deletePiece(centerX + 2, centerY);

					makePiece(centerX + 1, centerY - 1, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX + 1, centerY + 1, id);
					makePiece(centerX + 1, centerY + 2, id);

					r++;
					centerX++;
				}
			}
			else if (r == 1) {	// RIGHT
				// Checking if the next piece positionin bounds
				if (centerX + 1 >= gridW || centerX - 2 < 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX - 2][centerY + 1].enabled;
				bool b1 = grid[centerX - 1][centerY + 1].enabled;
				bool b2 = grid[centerX + 1][centerY + 1].enabled;


				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX, centerY);
					deletePiece(centerX, centerY + 2);

					makePiece(centerX - 2, centerY + 1, id);
					makePiece(centerX - 1, centerY + 1, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX + 1, centerY + 1, id);

					r++;
					centerY++;
				}
			}
			else if (r == 2) {	// DOWN
				// Checking if the next piece positionin bounds
				if (centerY + 1 >= gridH || centerY - 2 < 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX - 1][centerY + 1].enabled;
				bool b1 = grid[centerX - 1][centerY - 1].enabled;
				bool b2 = grid[centerX - 1][centerY - 2].enabled;


				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY);
					deletePiece(centerX, centerY);
					deletePiece(centerX - 2, centerY);

					makePiece(centerX - 1, centerY + 1, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX - 1, centerY - 1, id);
					makePiece(centerX - 1, centerY - 2, id);

					r++;
					centerX--;
				}
			}
			else if (r == 3) {	// LEFT
				// Checking if the next piece positionin bounds
				if (centerX - 1 < 0 || centerX + 2 >= gridW) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 2][centerY - 1].enabled;
				bool b1 = grid[centerX + 1][centerY - 1].enabled;
				bool b2 = grid[centerX - 1][centerY - 1].enabled;


				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY + 1);
					deletePiece(centerX, centerY);
					deletePiece(centerX, centerY - 2);

					makePiece(centerX + 2, centerY - 1, id);
					makePiece(centerX + 1, centerY - 1, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX - 1, centerY - 1, id);

					r = 0;
					centerY--;
				}
			}
			break;
		}
		case O:
			return;
		case T:
		{
			if (r == 0) {
				if (centerY + 1 >= gridH) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;


				if (!b0) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX + 1, centerY, id);

					r++;
				}
			}
			else if (r == 1) {
				if (centerX == 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX - 1][centerY].enabled && grid[centerX - 1][centerY].ID != id;


				if (!b0) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY + 1, id);

					r++;
				}
			}
			else if (r == 2) {
				if (centerY == 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY - 1].enabled && grid[centerX][centerY - 1].ID != id;


				if (!b0) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX - 1, centerY, id);

					r++;
				}
			}
			else if (r == 3) {
				// Checking if the next piece positionin bounds
				if (centerX == gridW - 1) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 1][centerY].enabled && grid[centerX + 1][centerY].ID != id;


				if (!b0) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY + 1);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY - 1, id);

					r = 0;
				}
			}
			break;
		}
		case S:
		{
			if (r == 0) {
				if (centerY + 1 == gridH) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 1][centerY].enabled && grid[centerX + 1][centerY].ID != id;
				bool b1 = grid[centerX + 1][centerY + 1].enabled && grid[centerX + 1][centerY + 1].ID != id;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY - 1);
					deletePiece(centerX - 1, centerY);
				
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX + 1, centerY + 1, id);

					r++;
				}
			}
			else if (r == 1) {
				if (centerX - 1 < 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;
				bool b1 = grid[centerX - 1][centerY + 1].enabled && grid[centerX - 1][centerY + 1].ID != id;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX + 1, centerY + 1);

					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX - 1, centerY + 1, id);

					r++;
				}
			}
			else if (r == 2) {
				if (centerY - 1 < 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX - 1][centerY].enabled && grid[centerX - 1][centerY].ID != id;
				bool b1 = grid[centerX - 1][centerY - 1].enabled && grid[centerX - 1][centerY - 1].ID != id;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY);
					deletePiece(centerX - 1, centerY + 1);

					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX - 1, centerY - 1, id);

					r++;
				}
			}
			else if (r == 3) {
				if (centerX + 1 >= gridW) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;
				bool b1 = grid[centerX - 1][centerY - 1].enabled && grid[centerX - 1][centerY - 1].ID != id;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY + 1);
					deletePiece(centerX - 1, centerY - 1);

					makePiece(centerX, centerY, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX + 1, centerY - 1, id);

					r = 0;
				}
			}
			break;
		}
		case Z:
		{
			if (r == 0) {
				if (centerY + 1 == gridH) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 1][centerY - 1].enabled && grid[centerX + 1][centerY - 1].ID != id;
				bool b1 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX - 1, centerY - 1);

					makePiece(centerX, centerY + 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX + 1, centerY - 1, id);

					r++;
				}
			}
			else if (r == 1) {
				if (centerX == 0) {
					break;
				}
				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX - 1][centerY].enabled && grid[centerX - 1][centerY].ID != id;
				bool b1 = grid[centerX + 1][centerY - 1].enabled && grid[centerX + 1][centerY - 1].ID != id;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY);
					deletePiece(centerX + 1, centerY - 1);

					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX + 1, centerY + 1, id);

					r++;
				}
			}
			else if (r == 2) {
				if (centerY == 0) {
					break;
				}
				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY - 1].enabled && grid[centerX][centerY - 1].ID != id;
				bool b1 = grid[centerX - 1][centerY + 1].enabled && grid[centerX - 1][centerY + 1].ID != id;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY + 1);
					deletePiece(centerX + 1, centerY + 1);

					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX - 1, centerY + 1, id);

					r++;
				}
			}
			else if (r == 3) {
				if (centerX == gridW - 1) {
					break;
				}
				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 1][centerY].enabled && grid[centerX + 1][centerY].ID != id;
				bool b1 = grid[centerX - 1][centerY - 1].enabled && grid[centerX - 1][centerY - 1].ID != id;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX - 1, centerY + 1);

					makePiece(centerX - 1, centerY - 1, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);

					r = 0;
				}
			}
			break;
		}
		case J:
		{
			if (r == 0) {
				if (centerY + 1 == gridH) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY - 1].enabled && grid[centerX][centerY - 1].ID != id;
				bool b1 = grid[centerX + 1][centerY - 1].enabled && grid[centerX + 1][centerY - 1].ID != id;
				bool b2 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX - 1, centerY - 1);
					deletePiece(centerX + 1, centerY);

					makePiece(centerX, centerY + 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX + 1, centerY - 1, id);

					r++;
				}
			}
			else if (r == 1) {
				// Check if rotation will cause piece to go out of bounds
				if (centerX == 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX - 1][centerY].enabled && grid[centerX - 1][centerY].ID != id;
				bool b1 = grid[centerX + 1][centerY].enabled && grid[centerX + 1][centerY].ID != id;
				bool b2 = grid[centerX + 1][centerY + 1].enabled && grid[centerX + 1][centerY + 1].ID != id;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX + 1, centerY - 1);
					deletePiece(centerX, centerY + 1);

					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX + 1, centerY + 1, id);

					r++;
				}
			}
			else if (r == 2) {
				// Check if rotation will cause piece to go out of bounds
				if (centerY == 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY - 1].enabled && grid[centerX][centerY - 1].ID != id;
				bool b1 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;
				bool b2 = grid[centerX - 1][centerY + 1].enabled && grid[centerX - 1][centerY + 1].ID != id;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX + 1, centerY);
					deletePiece(centerX + 1, centerY + 1);

					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX - 1, centerY + 1, id);

					r++;
				}
			}
			else if (r == 3) {
				// Check if rotation will cause piece to go out of bounds
				if (centerX + 1 == gridW) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 1][centerY].enabled && grid[centerX + 1][centerY].ID != id;
				bool b1 = grid[centerX - 1][centerY].enabled && grid[centerX - 1][centerY].ID != id;
				bool b2 = grid[centerX - 1][centerY - 1].enabled && grid[centerX - 1][centerY - 1].ID != id;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX, centerY + 1);
					deletePiece(centerX - 1, centerY + 1);

					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX - 1, centerY - 1, id);

					r = 0;
				}
			}
		}
		case L:
		{
			if (r == 0) {
				if (centerY + 1 == gridH) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY - 1].enabled && grid[centerX][centerY - 1].ID != id;
				bool b1 = grid[centerX + 1][centerY + 1].enabled && grid[centerX + 1][centerY + 1].ID != id;
				bool b2 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX + 1, centerY - 1);
					deletePiece(centerX + 1, centerY);

					makePiece(centerX, centerY + 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX + 1, centerY + 1, id);

					r++;
				}
			}
			else if (r == 1) {
				// Check if rotation will cause piece to go out of bounds
				if (centerX == 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX - 1][centerY].enabled && grid[centerX - 1][centerY].ID != id;
				bool b1 = grid[centerX + 1][centerY].enabled && grid[centerX + 1][centerY].ID != id;
				bool b2 = grid[centerX - 1][centerY + 1].enabled && grid[centerX - 1][centerY + 1].ID != id;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX + 1, centerY + 1);
					deletePiece(centerX, centerY + 1);

					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX - 1, centerY + 1, id);

					r++;
				}
			}
			else if (r == 2) {
				// Check if rotation will cause piece to go out of bounds
				if (centerY == 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY - 1].enabled && grid[centerX][centerY - 1].ID != id;
				bool b1 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;
				bool b2 = grid[centerX - 1][centerY - 1].enabled && grid[centerX - 1][centerY - 1].ID != id;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX + 1, centerY);
					deletePiece(centerX - 1, centerY + 1);

					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX - 1, centerY - 1, id);

					r++;
				}
			}
			else if (r == 3) {
				// Check if rotation will cause piece to go out of bounds
				if (centerX + 1 == gridW) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 1][centerY].enabled && grid[centerX + 1][centerY].ID != id;
				bool b1 = grid[centerX - 1][centerY].enabled && grid[centerX - 1][centerY].ID != id;
				bool b2 = grid[centerX - 1][centerY + 1].enabled && grid[centerX - 1][centerY + 1].ID != id;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX, centerY + 1);
					deletePiece(centerX - 1, centerY - 1);

					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX + 1, centerY - 1, id);

					r = 0;
				}
			}
		}
			break;
	}
}

void rotatePieceCCW() {

	if (pieceSet) {
		return;
	}

	piece id = ACTIVE;

	switch (p) {
		case I:
		{
			if (r == 0) {
				if (centerY - 1 < 0 || centerY + 2 >= gridH) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY - 1].enabled && grid[centerX][centerY - 1].ID != id;
				bool b1 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;
				bool b2 = grid[centerX][centerY + 2].enabled && grid[centerX][centerY + 2].ID != id;


				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX + 1, centerY);
					deletePiece(centerX + 2, centerY);

					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX, centerY + 2, id);

					r = 3;
					centerY++;
				}
			}
			else if (r == 1) {	// RIGHT
				// Checking if the next piece positionin bounds
				if (centerX + 1 >= gridW || centerX - 2 < 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 1][centerY].enabled && grid[centerX + 1][centerY].ID != id;
				bool b1 = grid[centerX - 1][centerY].enabled && grid[centerX - 1][centerY].ID != id;
				bool b2 = grid[centerX - 2][centerY].enabled && grid[centerX - 2][centerY].ID != id;


				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX, centerY + 1);
					deletePiece(centerX, centerY + 2);


					makePiece(centerX - 2, centerY, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);

					r--;
					centerX--;
				}
			}
			else if (r == 2) {	// DOWN
				// Checking if the next piece positionin bounds
				if (centerY + 1 >= gridH || centerY - 2 < 0) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX][centerY + 1].enabled && grid[centerX][centerY + 1].ID != id;
				bool b1 = grid[centerX][centerY - 1].enabled && grid[centerX][centerY - 1].ID != id;
				bool b2 = grid[centerX][centerY - 2].enabled && grid[centerX][centerY - 2].ID != id;


				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY);
					deletePiece(centerX - 1, centerY);
					deletePiece(centerX - 2, centerY);

					makePiece(centerX, centerY + 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY - 2, id);

					r--;
					centerY--;
				}
			}
			else if (r == 3) {	// LEFT
				// Checking if the next piece positionin bounds
				if (centerX - 1 < 0 || centerX + 2 >= gridW) {
					break;
				}

				// Checkng if the next pieces are open or not
				bool b0 = grid[centerX + 2][centerY].enabled && grid[centerX + 2][centerY].ID != id;
				bool b1 = grid[centerX + 1][centerY].enabled && grid[centerX + 1][centerY].ID != id;
				bool b2 = grid[centerX - 1][centerY].enabled && grid[centerX - 1][centerY].ID != id;


				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 2);
					deletePiece(centerX, centerY - 1);
					deletePiece(centerX, centerY + 1);

					makePiece(centerX + 2, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX - 1, centerY, id);

					r--;
					centerX++;
				}
			}
			break;
		}
		case O:
			break;
		case T:
		{
			if (r == 0) {
				
				if (centerY + 1 == gridH) {
					break;
				}

				bool b0 = grid[centerX][centerY + 1].enabled;

				if (!b0) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY);
					
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX - 1, centerY, id);

					r = 3;
				}
			}
			else if (r == 3) {

				if (centerX + 1 == gridW) {
					break;
				}

				bool b0 = grid[centerX + 1][centerY].enabled;

				if (!b0) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);

					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY + 1, id);

					r--;
				}
			}
			else if (r == 2) {

				if (centerY == 0) {
					break;
				}

				bool b0 = grid[centerX][centerY - 1].enabled;

				if (!b0) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);

					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX + 1, centerY, id);

					r--;
				}
			}
			else if (r == 1) {

				if (centerX == 0) {
					break;
				}

				bool b0 = grid[centerX - 1][centerY].enabled;

				if (!b0) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY + 1);

					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY - 1, id);

					r--;
				}
			}
			break;
		}
		case S:
		{
			if (r == 0) {

				if (centerY + 1 == gridH) {
					break;
				}

				bool b0 = grid[centerX - 1][centerY - 1].enabled;
				bool b1 = grid[centerX][centerY + 1].enabled;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX + 1, centerY - 1);

					makePiece(centerX - 1, centerY - 1, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);

					r = 3;
				}
			}
			else if (r == 3) {


				if (centerX + 1 == gridW) {
					break;
				}

				bool b0 = grid[centerX + 1][centerY].enabled;
				bool b1 = grid[centerX - 1][centerY + 1].enabled;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX - 1, centerY - 1);

					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX - 1, centerY + 1, id);

					r--;
				}
			}
			else if (r == 2) {

				if (centerY == 0) {
					break;
				}

				bool b0 = grid[centerX][centerY - 1].enabled;
				bool b1 = grid[centerX + 1][centerY + 1].enabled;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY + 1);
					deletePiece(centerX - 1, centerY + 1);

					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX + 1, centerY + 1, id);

					r--;
				}
			}
			else if (r == 1) {

				if (centerX == 0) {
					break;
				}

				bool b0 = grid[centerX + 1][centerY - 1].enabled;
				bool b1 = grid[centerX - 1][centerY].enabled;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY);
					deletePiece(centerX + 1, centerY + 1);

					makePiece(centerX + 1, centerY - 1, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX - 1, centerY, id);

					r--;
				}
			}
			break;
		}
		case Z:
		{
			if (r == 0) {
				if (centerY + 1 == gridH) {
					break;
				}

				bool b0 = grid[centerX - 1][centerY].enabled;
				bool b1 = grid[centerX - 1][centerY + 1].enabled;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY);
					deletePiece(centerX - 1, centerY - 1);

					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX - 1, centerY + 1, id);

					r = 3;
				}

			}
			else if (r == 3) {
				if (centerX + 1 == gridW) {
					break;
				}

				bool b0 = grid[centerX][centerY + 1].enabled;
				bool b1 = grid[centerX + 1][centerY + 1].enabled;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX - 1, centerY + 1);

					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX + 1, centerY + 1, id);

					r--;
				}
			}
			else if (r == 2) {
				if (centerY == 0) {
					break;
				}

				bool b0 = grid[centerX + 1][centerY].enabled;
				bool b1 = grid[centerX + 1][centerY - 1].enabled;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX + 1, centerY + 1);

					makePiece(centerX, centerY + 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX + 1, centerY - 1, id);

					r--;
				}
			}
			else if (r == 1) {
				if (centerX == 0) {
					break;
				}

				bool b0 = grid[centerX][centerY - 1].enabled;
				bool b1 = grid[centerX - 1][centerY - 1].enabled;

				if (!b0 && !b1) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY - 1);
					deletePiece(centerX, centerY + 1);

					makePiece(centerX - 1, centerY - 1, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);

					r--;
				}
			}
			break;
		}
		case J:
		{
			if (r == 0) {
				if (centerY + 1 == gridH) {
					break;
				}

				bool b0 = grid[centerX][centerY - 1].enabled;
				bool b1 = grid[centerX][centerY + 1].enabled;
				bool b2 = grid[centerX - 1][centerY + 1].enabled;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY - 1);
					deletePiece(centerX - 1, centerY);
					deletePiece(centerX + 1, centerY);

					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX - 1, centerY + 1, id);

					r = 3;
				}
			}
			else if (r == 3) {
				if (centerX + 1 == gridW) {
					break;
				}

				bool b0 = grid[centerX - 1][centerY].enabled;
				bool b1 = grid[centerX + 1][centerY].enabled;
				bool b2 = grid[centerX + 1][centerY + 1].enabled;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX, centerY + 1);
					deletePiece(centerX - 1, centerY + 1);

					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX + 1, centerY + 1, id);

					r--;
				}
			}
			else if (r == 2) {
				if (centerY == 0) {
					break;
				}

				bool b0 = grid[centerX][centerY + 1].enabled;
				bool b1 = grid[centerX][centerY - 1].enabled;
				bool b2 = grid[centerX + 1][centerY - 1].enabled;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX + 1, centerY);
					deletePiece(centerX + 1, centerY + 1);

					makePiece(centerX, centerY + 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX + 1, centerY - 1, id);

					r--;
				}
			}
			else if (r == 1) {
				if (centerX == 0) {
					break;
				}

				bool b0 = grid[centerX + 1][centerY].enabled;
				bool b1 = grid[centerX - 1][centerY].enabled;
				bool b2 = grid[centerX - 1][centerY - 1].enabled;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY - 1);
					deletePiece(centerX, centerY - 1);
					deletePiece(centerX, centerY + 1);

					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX - 1, centerY - 1, id);

					r--;
				}
			}
			break;
		}
		case L:
		{
			if (r == 0) {
				if (centerY + 1 == gridH) {
					break;
				}

				bool b0 = grid[centerX][centerY + 1].enabled;
				bool b1 = grid[centerX][centerY - 1].enabled;
				bool b2 = grid[centerX - 1][centerY - 1].enabled;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX - 1, centerY);
					deletePiece(centerX + 1, centerY);
					deletePiece(centerX + 1, centerY - 1);

					makePiece(centerX, centerY + 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY - 1, id);
					makePiece(centerX - 1, centerY - 1, id);

					r = 3;
				}
			}
			else if (r == 3) {
				if (centerX + 1 == gridW) {
					break;
				}

				bool b0 = grid[centerX + 1][centerY].enabled;
				bool b1 = grid[centerX - 1][centerY].enabled;
				bool b2 = grid[centerX - 1][centerY + 1].enabled;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY + 1);
					deletePiece(centerX, centerY - 1);
					deletePiece(centerX - 1, centerY - 1);

					makePiece(centerX + 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX - 1, centerY, id);
					makePiece(centerX - 1, centerY + 1, id);

					r--;
				}
			}
			else if (r == 2) {
				if (centerY == 0) {
					break;
				}

				bool b0 = grid[centerX][centerY - 1].enabled;
				bool b1 = grid[centerX][centerY + 1].enabled;
				bool b2 = grid[centerX + 1][centerY + 1].enabled;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX + 1, centerY);
					deletePiece(centerX - 1, centerY);
					deletePiece(centerX - 1, centerY + 1);

					makePiece(centerX, centerY - 1, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX, centerY + 1, id);
					makePiece(centerX + 1, centerY + 1, id);

					r--;
				}
			}
			else if (r == 1) {
				if (centerX == 0) {
					break;
				}

				bool b0 = grid[centerX + 1][centerY - 1].enabled;
				bool b1 = grid[centerX + 1][centerY].enabled;
				bool b2 = grid[centerX - 1][centerY].enabled;

				if (!b0 && !b1 && !b2) {
					if (!pieceMoved) {
						pieceMoved = true;
						setNextGrid();
					}

					deletePiece(centerX, centerY - 1);
					deletePiece(centerX, centerY + 1);
					deletePiece(centerX + 1, centerY + 1);

					makePiece(centerX - 1, centerY, id);
					makePiece(centerX, centerY, id);
					makePiece(centerX + 1, centerY, id);
					makePiece(centerX + 1, centerY - 1, id);

					r--;
				}
			}
			break;
		}
	}
}

void drawBoundaries() {
	int cx, cy;

	RECT rect;
	HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &rect);

	int monitorW = (rect.right - rect.left) / charPixelSizeW;
	int monitorH = (rect.bottom - rect.top) / charPixelSizeH;

	int gameWidth = (boxW * (gridW + 2));
	int gameHeight = (boxH * (gridH + 1));

	offsetX = (monitorW - gameWidth) / (2);
	offsetY = (monitorH - gameHeight) / (2);

	offsetHoldingX = offsetX - (holdingBoxW + (3 * boxW));
	offsetHoldingY = offsetY + (boxH * 2);

	setConsoleColor(WHITE);

	// left wall
	cx = offsetX - boxW;
	for (int y = 0; y < gridH + 1; y++) {
		cy = (boxH * y) + offsetY;
		drawBox(cx, cy);
	}

	// right wall
	cx = (gridW * boxW) + offsetX;
	for (int y = 0; y < gridH + 1; y++) {
		cy = (boxH * y) + offsetY;
		drawBox(cx, cy);
	}

	// floor
	cy = (boxH * gridH) + offsetY;
	for (int x = 0; x < gridW + 1; x++) {
		cx = (boxW * x) + offsetX;
		drawBox(cx, cy);
	}
}

// this shit is so fucked
void drawHoldingBox() {
	//drawBox(offsetHoldingX, offsetHoldingY);

	string output = "";
	for (int y = 0; y < boxH; y++) {
		for (int x = 0; x < holdingBoxW; x++) {
			output += box;
		}

		output += '\n';
	}

	setCursorPos(offsetHoldingX, offsetHoldingY);
	cout << output;
}

bool getInput(input* dir) {
	holdPiece = GetAsyncKeyState(VK_SPACE) & 0x0001;
	holdingDown = GetAsyncKeyState(VK_DOWN) & 0x8000;
	rotateCCW = GetAsyncKeyState(0x41) & 0x0001;
	rotateCW = GetAsyncKeyState(0x44) & 0x0001;

	if (GetAsyncKeyState(VK_LEFT) & 0x0001) { // left
		*dir = LEFT;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x0001) { // right
		*dir = RIGHT;
	}
	else if (GetAsyncKeyState(VK_ESCAPE)) {
		*dir = END;
	}
	else if (GetAsyncKeyState(0x52) & 0x0001) {
		*dir = RESTART;
	}
	else if (GetAsyncKeyState(0x31) & 0x0001) { //
		*dir = ONE;
	}
	else if (GetAsyncKeyState(0x32) & 0x0001) { //
		*dir = TWO;
	}
	else if (GetAsyncKeyState(0x33) & 0x0001) { // 
		*dir = THREE;
	}
	else if (GetAsyncKeyState(0x34) & 0x0001) { // 
		*dir = FOUR;
	}
	else if (GetAsyncKeyState(0x35) & 0x0001) { // 
		*dir = FIVE;
	}
	else if (GetAsyncKeyState(0x36) & 0x0001) { // 
		*dir = SIX;
	}
	else if (GetAsyncKeyState(0x37) & 0x0001) {
		*dir = SEVEN;
	}
	else {
		*dir = NONE;
		return false;
	}
	return true;
}

// Only draws tiles that are needed
void printUpdatedGrid() {
	int cx;
	int cy;

	for (int x = 0; x < gridW; x++) {
		for (int y = 0; y < gridH; y++) {
			unsigned int color;
			if (grid[x][y].ID == ACTIVE) {
				color = idToColor(p);
			}
			else {
				color = idToColor(grid[x][y].ID);
			}

			if (grid[x][y].enabled != nextGrid[x][y]) {
				cx = (x * boxW) + offsetX;
				cy = (y * boxH) + offsetY;

				if (grid[x][y].enabled) {	// if piece is enabled when next is not
					eraseBox(cx, cy);
				}
				else {						// if piece is deisable when next is
					setConsoleColor(color);
					drawBox(cx, cy);
				}
				
				grid[x][y].enabled = nextGrid[x][y];
			}
		}
	}
}

void printFullGrid() {
	for (int y = rowCleared; y >= 0; y--) {
		for (int x = 0; x < gridW; x++) {
			if (grid[x][y].ID != BLANK) {
				piece type;

				if (grid[x][y].ID == ACTIVE) {
					type = p;
				}
				else {
					type = grid[x][y].ID;
				}

				int cx = (x * boxW) + offsetX;
				int cy = (y * boxH) + offsetY;

				setConsoleColor(idToColor(type));
				drawBox(cx, cy);
			}
		}
	}
}

unsigned short idToColor(int id) {
	switch (id) {
	case I:
		return CYAN;
	case O:
		return YELLOW;
	case T:
		return MAGENTA;
	case S:
		return GREEN;
	case Z:
		return RED;
	case J:
		return BLUE;
	case L:
		return GRAY;
	default:
		return 0;
	}
}

void makePiece(int x, int y, piece id) {
	nextGrid[x][y] = true;
	grid[x][y].active = true;
	grid[x][y].ID = id;
}

void deletePiece(int x, int y) {
	grid[x][y].ID = BLANK;
	grid[x][y].active = false;
	nextGrid[x][y] = false;
}

void updateGrid() {
	
	if (!pieceMoved) {
		setNextGrid();
		pieceMoved = true;
	}

	bool hit = false;

	// Checks if the block can move down
	for (int y = gridH - 1; y >= 0; y--) {
		for (int x = 0; x < gridW; x++) {
			if (grid[x][y].active) {
				if (y == gridH - 1) {
					hit = true;
				}
				else if (nextGrid[x][y + 1] && grid[x][y + 1].ID != grid[x][y].ID) {
					hit = true;
				}
			}
		}
	}

	if (!hit) {
		centerY++;

		for (int y = gridH - 1; y >= 0; y--) {
			for (int x = 0; x < gridW; x++) {
				if (grid[x][y].active) {	// if enabled and active
					makePiece(x, y + 1, grid[x][y].ID);
					deletePiece(x, y);
				}
			}
		}
	}
	else { // block is going to hit
		setPiece();
	}
}

bool movePieceRight() {
	// check if the piece can move right
	bool canMove = true;

	for (int x = 0; x < gridW; x++) {
		for (int y = 0; y < gridH; y++) {
			if (grid[x][y].active) {
				if (x == gridW - 1) {
					canMove = false;
				}
				else if (grid[x + 1][y].enabled && grid[x + 1][y].ID != grid[x][y].ID) {
					canMove = false;
				}
			}
		}
	}

	if (canMove) {
		centerX++;

		if (!pieceMoved) {
			pieceMoved = true;
			setNextGrid();
		}

		for (int x = gridW - 1; x >= 0; x--) {
			for (int y = 0; y < gridH; y++) {
				if (grid[x][y].active) {
					makePiece(x + 1, y, grid[x][y].ID);
					deletePiece(x, y);
				}
			}
		}
	}

	return canMove;
}

bool movePieceLeft() {
	// check if the piece can move right
	bool canMove = true;

	for (int x = 0; x < gridW; x++) {
		for (int y = 0; y < gridH; y++) {
			if (grid[x][y].active) {
				if (x == 0) {
					canMove = false;
				}
				else if (grid[x - 1][y].enabled && grid[x - 1][y].ID != grid[x][y].ID) {
					canMove = false;
				}
			}
		}
	}

	if (canMove) {
		centerX--;

		if (!pieceMoved) {
			pieceMoved = true;
			setNextGrid();
		}

		for (int x = 0; x < gridW; x++) {
			for (int y = 0; y < gridH; y++) {
				if (grid[x][y].active) {
					makePiece(x - 1, y, grid[x][y].ID);
					deletePiece(x, y);
				}
			}
		}
	}

	return canMove;
}

void setPiece() {
	pieceSet = true;

	for (int x = 0; x < gridW; x++) {
		for (int y = 0; y < gridH; y++) {
			if (grid[x][y].ID == ACTIVE) {
				nextGrid[x][y] = true;
				grid[x][y].active = false;
				grid[x][y].ID = p;
			}
		}
	}

	//check every row to see if it is full
	for (int row = gridH - 1; row >= 0; row--) {
		bool fullRow = true;

		for (int col = 0; col < gridW; col++) {
			if (grid[col][row].ID == BLANK) {
				fullRow = false;
				break;
			}
		}

		if (fullRow) {
			// flag for choosing the update function
			if (row > rowCleared) {
				rowCleared = row;
			}

			// delete the row
			for (int x = 0; x < gridW; x++) {
				deletePiece(x, row);
			}

			for (int y = row - 1; y >= 0; y--) { // loop from the bottom up
				for (int x = 0; x < gridW; x++) {
					if (grid[x][y].ID != BLANK) {
						piece id = grid[x][y].ID;

						deletePiece(x, y);
						makePiece(x, y + 1, id);
						grid[x][y + 1].active = false;
					}
				}
			}
			row++;
		}
	}

	makeRandomPiece();
}

// sets next grid array to current enabled grid
void setNextGrid() {
	for (int y = 0; y < gridH; y++) {
		for (int x = 0; x < gridW; x++) {
			nextGrid[x][y] = false;

			if (!grid[x][y].active) {
				nextGrid[x][y] = grid[x][y].enabled;
			}
		}
	}
}

void createPiece(piece x) {
	r = 0;
	pieceSet = false;

	switch (x) {
	case I: // I
		p = I;
		centerX = 4;
		centerY = 0;
		//setConsoleColor(CYAN);

		for (int x = 3; x <= 6; x++) {
			makePiece(x, 0, ACTIVE);
		}
		break;

	case O: // O
		p = O;
		centerX = 0;
		centerY = 0;
		//setConsoleColor(YELLOW);

		for (int x = 4; x <= 5; x++) {
			for (int y = 0; y <= 1; y++) {
				makePiece(x, y, ACTIVE);
			}
		}
		break;

	case T: // T
		p = T;
		centerX = 4;
		centerY = 1;

		//setConsoleColor(MAGENTA);

		makePiece(4, 0, ACTIVE);
		for (int x = 3; x <= 5; x++) {
			makePiece(x, 1, ACTIVE);
		}
		break;

	case S: // S
		p = S;
		centerX = 4;
		centerY = 1;
		//setConsoleColor(GREEN);

		makePiece(4, 0, ACTIVE);
		makePiece(5, 0, ACTIVE);
		makePiece(3, 1, ACTIVE);
		makePiece(4, 1, ACTIVE);
		break;

	case Z: // Z 
		p = Z;
		centerX = 4;
		centerY = 1;
		//setConsoleColor(RED);

		makePiece(3, 0, ACTIVE);
		makePiece(4, 0, ACTIVE);
		makePiece(4, 1, ACTIVE);
		makePiece(5, 1, ACTIVE);
		break;

	case J: // J 
		p = J;
		centerX = 4;
		centerY = 1;
		//setConsoleColor(BLUE);

		makePiece(3, 0, ACTIVE);
		for (int x = 3; x <= 5; x++) {
			makePiece(x, 1, ACTIVE);
		}
		break;

	case L: // L
		p = L;
		centerX = 4;
		centerY = 1;
		//setConsoleColor(GRAY);

		makePiece(5, 0, ACTIVE);
		for (int x = 3; x <= 5; x++) {
			makePiece(x, 1, ACTIVE);
		}
		break;
		
	default:
		cout << "Shits so Fucked";
	}
}

unsigned short randColor() {
	int index = rand() % 7;

	switch (index) {
	case 0:
		return RED;
	case 1:
		return BLUE;
	case 2:
		return GREEN;
	case 3: 
		return MAGENTA;
	case 4:
		return YELLOW;
	case 5:
		return CYAN;
	case 6:
		return WHITE;
	default:
		cout << "random color function failed";
		return WHITE;
	}

	return WHITE;
}

void drawBox(int x, int y) {

	string output;
	for (int i = 1; i <= boxH; i++) {
		setCursorPos(x, y + i);
		output = "";
		for (int j = 0; j < boxW; j++) {
			output += box;
		}
		cout << output;
	}
}

void eraseBox(int x, int y) {
	setConsoleColor(BLACK);

	string output;
	for (int i = 1; i <= boxH; i++) {
		setCursorPos(x, y + i);
		output = "";
		for (int j = 0; j < boxW; j++) {
			output += box;
		}
		cout << output;
	}
}

void setCursorPos(int x, int y)
{
	std::cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}

void setConsoleColor(unsigned short colour)
{
	std::cout.flush();
	SetConsoleTextAttribute(hOut, colour);
}

void showConsoleCursor(bool showFlag)
{
	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(hOut, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(hOut, &cursorInfo);
}

void cls()
{

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD topLeft = { 0, 0 };

	// std::cout uses a buffer to batch writes to the underlying console.
	// We need to flush that to the console because we're circumventing
	// std::cout entirely; after we clear the console, we don't want
	// stale buffered text to randomly be written out.
	std::cout.flush();

	// Figure out the current width and height of the console window
	if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
		// TODO: Handle failure!
		abort();
	}
	DWORD length = csbi.dwSize.X * csbi.dwSize.Y;

	DWORD written;

	// Flood-fill the console with spaces to clear it
	FillConsoleOutputCharacter(hOut, TEXT(' '), length, topLeft, &written);

	// Reset the attributes of every character to the default.
	// This clears all background colour formatting, if any.
	FillConsoleOutputAttribute(hOut, csbi.wAttributes, length, topLeft, &written);

	// Move the cursor back to the top left for the next sequence of writes
	SetConsoleCursorPosition(hOut, topLeft);
}
