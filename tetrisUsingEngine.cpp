#include "consoleGameEngine.h" // imports our game engine
#include <vector>

// this is my console game engine and its sample usage (porting my tetris game to this engine)
// created following javidx9's follow-along videos

class tetris : public consoleGameEngine {
public:
	virtual void setupGameAssets() {
		// create assets (7 common shapes in tetris)
		tetromino[0].append(L"..x."); // indexes are calculated using r + c * w
		tetromino[0].append(L"..x.");
		tetromino[0].append(L"..x.");
		tetromino[0].append(L"..x.");

		tetromino[1].append(L"..x.");
		tetromino[1].append(L".xx.");
		tetromino[1].append(L".x..");
		tetromino[1].append(L"....");

		tetromino[2].append(L".x..");
		tetromino[2].append(L".xx.");
		tetromino[2].append(L"..x.");
		tetromino[2].append(L"....");

		tetromino[3].append(L"....");
		tetromino[3].append(L".xx.");
		tetromino[3].append(L".xx.");
		tetromino[3].append(L"....");

		tetromino[4].append(L"..x.");
		tetromino[4].append(L".xx.");
		tetromino[4].append(L"..x.");
		tetromino[4].append(L"....");

		tetromino[5].append(L"....");
		tetromino[5].append(L".xx.");
		tetromino[5].append(L"..x.");
		tetromino[5].append(L"..x.");

		tetromino[6].append(L"....");
		tetromino[6].append(L".xx.");
		tetromino[6].append(L".x..");
		tetromino[6].append(L".x..");

		// initializes our playing field (map) using an algorithm
		//  rather than drawing it out one pixel by pixel
		pField = (unsigned char*)malloc(sizeof(unsigned char) * nFieldWidth * nFieldHeight); // new unsigned char[nFieldWidth * nFieldHeight];
		for (int r = 0; r < nFieldWidth; r++) {
			for (int c = 0; c < nFieldHeight; c++) {
				pField[c * nFieldWidth + r] = (r == 0 || r == nFieldWidth - 1 || c == nFieldHeight - 1) ? 9 : 0;
				// left side? || right side? || bottom?
				// "condition ? case1 : case2" can be used to replace the if else statement
			}
		}
	}

	virtual bool GameUpdate(float ElapsedTime) {
		this_thread::sleep_for(50ms);// not using the ElapsedTime to handle ticks for now
		bool gameActive = consoleGameEngine::gameActive;// in function scope, not the gameActive in class

		nPaceCounter++;// updates paceCounter every tick
		bForceDown = (nPaceCounter == nPace);// signals that Pace has been reached
		if (nPaceCounter == nPace) {
			nPaceCounter = 0;
		}

		if (keys[VK_LEFT].bHeld) { //when left arrow is pressed, if we can move to the left, our current shape will move to the left
			if (canMoveToLocation(nCurrentPiece, nCurrentRotation, nCurrentHori - 1, nCurrentVerti)) {
				nCurrentHori--;
			}
		}

		if (keys[VK_RIGHT].bHeld) { //same as left arrow, but for the right arrow
			if (canMoveToLocation(nCurrentPiece, nCurrentRotation, nCurrentHori + 1, nCurrentVerti)) {
				nCurrentHori++;
			}
		}

		if (keys[VK_DOWN].bHeld) { //same as left arrow, but for the down arrow
			if (canMoveToLocation(nCurrentPiece, nCurrentRotation, nCurrentHori, nCurrentVerti + 1)) {
				nCurrentVerti++;
			}
		}

		if (bForceDown) {
			if (canMoveToLocation(nCurrentPiece, nCurrentRotation, nCurrentHori, nCurrentVerti + 1)) {
				nCurrentVerti++;
			}
			else {
				// lock current piece in the field
				for (int c = 0; c < 4; c++) { // makes the piece part of the field
					for (int r = 0; r < 4; r++) {
						if (tetromino[nCurrentPiece][rotate90(r, c, nCurrentRotation)] == L'x') {
							pField[(nCurrentVerti + r) * nFieldWidth + (nCurrentHori + c)] = nCurrentPiece + 1; // + 1 because we want to make it any number other than 0
						}
					}
				}

				nPieceCount++;// tracks pieces given to player and adjusts difficulty accordingly
				if (nPieceCount % 10 == 0) {
					if (nPace >= 10) nPace--;
				}

				// Check have we got any full horizontal lines
				// notes: only need to check the four rows occupied by the last tetromino
				//  instead of whole field because the last line can only be formed at the
				//  last tetromino
				for (int r = 0; r < 4; r++) {
					if (nCurrentVerti + r < nFieldHeight - 1) {// don't check the last row in playing field which is the boundary
						// check each line: if there's any empty cells, we'll set bLine to false
						bool bLine = true;
						for (int c = 1; c < nFieldWidth - 1; c++) {// not checking first and last column of playing field whcih are also boundaries
							if ((pField[(nCurrentVerti + r) * nFieldWidth + c]) == 0) {
								bLine = false;
							}
						}

						if (bLine) {
							// set the line to all equal symbols before destroying them to make it look cooler, :)
							for (int c = 1; c < nFieldWidth - 1; c++) {
								pField[(nCurrentVerti + r) * nFieldWidth + c] = 8;
							}

							vLines.push_back(nCurrentVerti + r);
						}
					}
				}

				// update score
				nScore += 25; // get 25 points for every piece you land
				if (!vLines.empty()) {
					nScore += (1 << vLines.size()) * 100;
				} // you get exponentially more points the more lines you get, it's things like this that makes a game addicting, lmao

				// generate next piece
				nCurrentHori = nFieldWidth / 2;
				nCurrentVerti = 0;
				nCurrentRotation = 0;

				srand(time(NULL)); // to make rand() truly random
				nCurrentPiece = rand() % 7; //rand is not really random, it's pseudo random and generates a deterministic sequence

				// if next piece cannot be added
				gameActive = canMoveToLocation(nCurrentPiece, nCurrentRotation, nCurrentHori, nCurrentVerti);
			}
		}

		// notes: if Z was pressed the last tick, and it's pressed now, nothing happens and bRotateHold remains true
		//  if Z was not presed last tick, but it's pressed now, then rotation will happen if possible, and bRotateHOld becomes
		//  true. if Z is not pressed this tick, bRotateHold will be set to false
		if (keys[0x5A].bHeld) { //z for rotation
			if (!bRotateHold && canMoveToLocation(nCurrentPiece, nCurrentRotation + 1, nCurrentHori, nCurrentVerti)) {
				nCurrentRotation++;
			}
			bRotateHold = true;
		}
		else {
			bRotateHold = false;
		}

		// Draw our playing field onto the screen buffer
		// !note: cmd prompt goes through the indices of the array one by one and moves to the
		//  next line only when it fills the window, so the screen can get all sorts of deforms
		//  if you resize the window or have a default window size different from expected
		for (int r = 0; r < nFieldWidth; r++) {
			for (int c = 0; c < nFieldHeight; c++) {
				screen[(c + 2) * nScreenWidth + (r + 2)] = L" ABCDEFG=#"[pField[c * nFieldWidth + r]];
				// this c + 2, r + 2 offsets the screen toward to bottom left
				// the " ABCDEFG=#"[] is basically an array, and it assigns the
				//  number specified in the playing field to the right "pixel" 
				//  (char in reality) to print on screen
			}
		}

		// Draw Current Piece
		// note: if the pixel on the currently rotated shape was a 'x' on the original rotation then it must be drawn
		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				if (tetromino[nCurrentPiece][rotate90(r, c, nCurrentRotation)] == L'x') {
					screen[(nCurrentVerti + r + 2) * nScreenWidth + (nCurrentHori + c + 2)] = nCurrentPiece + 65;
					// nCurrentPiece + 65 gives us the correct char, e.x. 0 + 65 is 'A'
				}
			}
		}

		// Draw Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		// we are cheating a bit and doing a bit of game logic in output: destroying full lines
		//  although i think that doing this here is not really necessary and i can do it in 
		//  game logic
		if (!vLines.empty()) {
			// for each line, each row above it must drop down 1 step
			for (auto& v : vLines) {
				for (int c = 1; c < nFieldWidth - 1; c++) {
					for (int r = v; r > 0; r--) {
						pField[r * nFieldWidth + c] = pField[(r - 1) * nFieldWidth + c];
					}
					pField[c] = 0;// fills an empty cell at the top
				}
			}

			vLines.clear();
		}

		return gameActive;
	}
private:
	// game specific functions:

	// rotate90: the row and column passed in specify indices on a shape
	//  already rotated in 90 degrees by r times, now we just want to know
	//  what would the that pixel's index be on the original rotation of the shape
	// notes: when rotating by 90 degrees, your column becomes the new row while 
	//  your inverted row becomes the new column, so we want to reverse this
	int rotate90(int row, int column, int r) {
		for (int i = 1; i <= r; i++) {
			int temp_column = column;
			column = row;
			row = 3 - temp_column;
		}
		return column + row * 4; //4 is width
	}

	// canMoveToLocation: returns the truth value of whether the tetromino with rotation
	//  applied can move to the location at nPosHoir, nPosVerti
	bool canMoveToLocation(int nTetromino, int nRotation, int nPosHori, int nPosVerti) {
		for (int r = 0; r < 4; r++) {// 4 is width of tetromino
			for (int c = 0; c < 4; c++) {
				// index before rotation
				int pi = rotate90(r, c, nRotation);

				// index of each pixel of tetromino in our playing field
				// tetromino's location is determined by the location of its top left pixel
				int fVerti = nPosVerti + r;
				int fHori = nPosHori + c;
				int fi = fVerti * nFieldWidth + fHori;

				//OutputDebugStringW(L"sfd");
				if (fHori >= 0 && fHori < nFieldWidth) {
					if (fVerti >= 0 && fVerti < nFieldHeight) {
						if (tetromino[nTetromino][pi] == L'x' && pField[fi] != 0) {
							return false;
							// there has to be a collision if the tetromino pixel is 'x' and the field pixel is not 0
						}
					}
				}
			}
		}

		return true;
	}

private:
	// game stuff
	wstring tetromino[7];
	// the playing field asset:
	// notes: 9 is solid space, 0 is empty space
	int nFieldWidth = 12;
	int nFieldHeight = 18;
	unsigned char* pField = nullptr;// array of unsigned chars

	int nCurrentPiece = 1;
	int nCurrentRotation = 0;
	int nCurrentHori = nFieldWidth / 2;
	int nCurrentVerti = 0;

	bool bRotateHold = false;

	int nPace = 20; // in tetris, you want to force the piece down every few seconds. pace specifies how many ticks before force down
	int nPaceCounter = 0; // paceCounter counts the ticks for our pace
	bool bForceDown = false;
	int nPieceCount = 0; // tracks the pieces given to the player for increasing the difficulty
	int nScore = 0; // tracks the score for hte player

	vector<int> vLines;// store the the current row of full lines in field before we destroy them
};

int main() {
	tetris game;// game engine alone cannot be played, you have to fill it with game specific stuff in inheritance, then you can use it
	game.setupScreenBuffer();
	game.runGame();
	return 0;
}