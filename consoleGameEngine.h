// In the games, we've made so far. we are setting up assets,
//  making a game loop (handle timing, handle input, handle output) 
//  and adding screen buffers for output a lot. And if a programmer
//  is doing something repeatedly then it should be wrapped up and 
//  packaged away. That brings us to our console game engine

// created following javidx9's follow-along videos

// rough organization of game engines:
// 1. setup screen buffer
// 2. create game assets
// 3. game loop : timing, handle input, game logic, handle output
#include <iostream>
using namespace std;

#include <crtdbg.h> // for debug output

#include <Windows.h> //gives us our console, screen buffer
#include <thread> // allows us to deal with timing, ticks

class consoleGameEngine
{
public:
	// sets up the screen buffer before game loop
	//  the engine handles screen buffer for us
	void setupScreenBuffer() {
		screen = (wchar_t*)malloc(sizeof(wchar_t) * nScreenWidth * nScreenHeight); // new wchar_t[nScreenWidth * nScreenHeight];
		for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
			screen[i] = L' ';
		}// initializes the screen array (the buffer)
		hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		SetConsoleActiveScreenBuffer(hConsole); // grabs a handle to the consolve buffer and set it as the active screen buffer
	}
	
	// initialize assets for our game loop
	virtual void setupGameAssets() = 0;
	// pure virtual function, inherited class must implement
	// 	this is why this class is abstract

	// handles game logic
	// notes: consumes ElapsedTime so that our game runs smoothly regardless
	//  of computer performance
	// return value will determine whether we will exit the game loop
	virtual bool GameUpdate(float ElapsedTime) = 0;

	// runs the game, the code in our int main before we wrote the engine
	void runGame() {
		gameActive = true;

		setupGameAssets();

		auto startTime = std::chrono::system_clock::now();// time points for handling timing. elapsed time per tick in while loop in particular
		auto endTime = std::chrono::system_clock::now();

		while (gameActive) {
			// game timing: timing is important because some people's computers run faster than others'
			// this_thread::sleep_for(50ms); // one game tick, previous timing code
			endTime = std::chrono::system_clock::now();// calculates elapsed time and updates tp1 for next tick
			std::chrono::duration<float> elapsedTime = endTime - startTime;
			startTime = endTime;
			float fElapsedTime = elapsedTime.count();

			handleInput();
			if (!GameUpdate(fElapsedTime)) {
				gameActive = false;
			}
			handleOutput();
		}

		// display the score after the loop ends
		CloseHandle(hConsole);
		system("pause");
	}
		
private:
	// records state of all keys on keyboard, mouse by comparing new state
	//  of keys vs old state of keys every tick
	// notes: getAsyncKeyState checks if the button is pressed
	// The function GetAsyncKeyState returns a SHORT, which is a 16-bit signed integer. The highest
	//  bit of the returned value represents whether the key is currently being held down.You can test
	//  this bit with GetAsyncKeyState(KEY)& 0x8000.
	void handleInput() {
		// Handle Keyboard Input:
		for (int i = 0; i < 256; i++)
		{
			keyNewState[i] = GetAsyncKeyState(i);

			keys[i].bPressed = false;
			keys[i].bReleased = false;

			if (keyNewState[i] != keyOldState[i])
			{
				if (keyNewState[i] & 0x8000)
				{
					keys[i].bPressed = !keys[i].bHeld;
					keys[i].bHeld = true;
				}
				else
				{
					keys[i].bReleased = true;
					keys[i].bHeld = false;
				}
			}

			keyOldState[i] = keyNewState[i];
		}
		// handle mouse input: will do this later
	}

	// display Frame, draw to the buffer
	// notes: draws screen array in the dimensions, starting at position 0,0, to the console
	// notes: drawing to the screen array is handled in gameUpdate
	void handleOutput() {
		// you can also add code to update the title of the console
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0 ,0 }, &dwBytesWritten);
	}

protected:
	// assets, variables used in the game are declared as class variables for now
	
	// screen buffer stuff
	wchar_t* screen;
	HANDLE hConsole;
	DWORD dwBytesWritten = 0;
	int nScreenWidth = 120; // console screen width (my default is 120)
	int nScreenHeight = 30; // console screen height

	// handle input stuff
	bool bRotateHold = false;
	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} keys[256], mouse[5]; // array of keys and whether they are pressed, released or held
	short keyOldState[256] = { 0 };
	short keyNewState[256] = { 0 };
	bool mouseOldState[5] = { 0 };
	bool mouseNewState[5] = { 0 };

	// game stuff

	// game state
	bool gameActive = false;
	// here you can put more specific game state variables, game stuff
};

/*int main() {
	consoleGameEngine game;// game engine alone cannot be played, you have to fill it with game specific stuff in inheritance, then you can use it
	game.setupScreenBuffer();
	game.runGame();
	return 0;
}*/