/////////////////////////////////////////////////
// Global_Engine.h 
// Global Header File For Mouse Hunt
// Developed By: Ben Draut
// Last Updated: 5/19/06
/////////////////////////////////////////////////

#ifndef GLOBAL_ENGINE_H
#define GLOBAL_ENGINE_H

/////////////////////////////////////////////////
// Misc. Stuff
/////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
#define ReleaseCOM(x) if(x){x->Release(); x = NULL;}

/////////////////////////////////////////////////
// Includes and Constants
/////////////////////////////////////////////////

// Windows Includes
#include <windows.h>

// Standard Inlcudes 
#include <time.h>
#include <vector>

// DirectX Includes 
#include <d3d9.h>
#include <d3dx9.h>
#include <dsutil.h>

// Game Engine Includes
#include "Main_Engine.h"
#include "Graphics_Engine.h"
#include "Input_Engine.h"
#include "Audio_Engine.h"

// Object Class Include 
#include "GameObjects.h"

/////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////
// Menu constants 
#define MENU_ARROW_X			200
#define SINGLE_CHOICE_Y			235
#define MULTI_CHOICE_Y			280
#define INSTRUCT_CHOICE_Y		325
#define EXIT_CHOICE_Y			370

// Game constants 
#define TOTAL_MICE				400
#define MICE_TO_DEPLOY			200
#define FRENZY_MICE_TO_DEPLOY	400
#define FRENZY_DEPLOY_DELAY		10
#define DEPLOY_DELAY			20
#define TOTAL_CATS				4
#define MAX_ARROWS_OUT			4
#define WINNING_AMOUNT			1000
#define ARROW_LIFESPAN			1500  // In Frames 

// Constant game locations
#define MOUSE_HOLE_X			224
#define MOUSE_HOLE_Y			224
#define RED_HOLE_X				96
#define RED_HOLE_Y				96
#define GREEN_HOLE_X			352
#define GREEN_HOLE_Y			96
#define BLUE_HOLE_X				352
#define BLUE_HOLE_Y				352
#define YELLOW_HOLE_X			96
#define YELLOW_HOLE_Y			352

// Enumerations 
enum MenuChoices  
{
	SINGLE = 0,
	MULTI,
	INSTRUCT,
	EXIT
};

// Game Data declarations
extern short TileMap[15][20];	// Game TileMap 
extern int Occupied[13][13];	// Array of the playing area with a int value for each square							
extern int HotSpots[3][4][2];	// Array of "Hot Spot" locations used in AI

/////////////////////////////////////////////////
// Derived Main Game Class
/////////////////////////////////////////////////
class cGame : public cBaseGame
{
private:
	// Engine Graphics Objects 
	cGraphics m_Graphics;

	// Tile engine object
	cTile m_Tile;

	// Font Object
	cFont m_Font;

	// Keyboard object
	cKeyboard m_KB;

	// DirectSound object
	cAudio m_Audio;

	// Music and sound objects
	cWAVSound m_Music;
	cWAVSound m_EatenMouse;
	cWAVSound m_Frenzy;
	cWAVSound m_CatAttack;
	cWAVSound m_Clapping;
	cWAVSound m_Klunk;
	cWAVSound m_BeepBeep;

	// State Machine and State prototypes 
	cStateMachine m_SM;
	static void Menu(void *DataPtr, long Purpose);
	static void Instruct(void *DataPtr, long Purpose);
	static void Single(void *DataPtr, long Purpose);
	static void Multi(void *DataPtr, long Purpose);
	static void GameOver(void *DataPtr, long Purpose);
	static void Countdown(void *DataPtr, long Purpose);
	static void Pause(void *DataPtr, long Purpose);

	// General Variables //
	// Integers
	int m_CurrentNumber;			// Current number in game countdown
	int m_NumX, m_NumY;				// XY coordinates of that number
	int m_NumMiceOut;				// Number of mouse currently exiting hole to enter the playing field 
	int m_NumMiceIn;				// Number of mice left to exit field 
	int m_MenuChoice;				// Current menu choice
	int m_CurrentLeader;			// Current leader of the game 
	int m_Winner;					// Number that represents the number of the winner of the round
	int m_TextX, m_TextY;			// XY coordinates of the text that is printed at the end of a round
	int m_TextMoveX, m_TextMoveY;	// XY move values for the text
	int m_AIDelayCount, m_AIDelay;	// Values to set a delay time between AI updates
	
	DWORD m_StartTime;		  // Start point of a timer
	
	float m_Scaler;			  // Scaling value of that number
	
	bool m_CountdownExpired;  // Value that is true if the countdown has expired
	bool m_DeployMice;		  // Value that is true if mice are to be deployed
	bool m_InAFrenzy;		  // True if extra mice should be deployed
	bool m_CatsAreAttacking;  // True if there is a cat attack

	// General Textures 
	cTexture m_MenuTex;
	cTexture m_PauseTex;
	cTexture m_InstructTex;
	cTexture m_MouseHuntTex;
	cTexture m_FrenzyTex;
	cTexture m_CatAttackTex;

	// General Game Objects 
	cCursor *m_Cursors[4];

	// Mice Objects
	cMouse *m_Mice[TOTAL_MICE];		// Array of Mouse objects
	bool m_MiceAlive[TOTAL_MICE];	// Array of booleans specifying if a mouse is alive

	// Cat objects for Cat Attack
	cCat *m_Cats[TOTAL_CATS];

public:
	// Constructor/Destructor 
	cGame();
	~cGame(){}

	// Overloaded Main Game Functions 
	bool GameInit();
	bool GameRun();
	bool GameEnd();

	// General Functions 
	// Initialization functions 
	void InitObjects();

	// Updating functions
	void UpdateMenu();
	bool UpdatePauseMenu();
	void UpdateGame(bool SinglePlayer);
	void UpdateArrows();
	void UpdateAI(int PlayerNumber);
	void UpdateTextPosition();
	void UpdateCountdown();

	// Misc. functions
	void DeployMouse(int MouseNumber=0);
	void StartNewWave();
	void HandleMouseCollisions(cMouse *Mouse);
	void HandleCatCollisions(cCat *Cat);
	void HandleP1Input();
	void HandleP2Input();
	
	// Drawing functions 
	void DrawMenu();
	void DrawGame();
	void DrawScores();
	void DrawTileMap();
	void DrawWinText();
	void DrawCountdown();
	
	// Shutdown functions 
	void ShutdownObjects();
};

#endif 


