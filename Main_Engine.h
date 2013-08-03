/////////////////////////////////////////////////
// Main_Engine.h				
// Main Engine Header File		
// Developed By: Ben Draut		
// Last Updated: 3/1/06			
/////////////////////////////////////////////////

#ifndef MAIN_ENGINE_H
#define MAIN_ENGINE_H

// Include Windows Stuff 
#include <windows.h>

// Constants
const DEFAULT_WIDTH  = 640;
const DEFAULT_HEIGHT = 480;

/////////////////////////////////////////////////
// Window Procedure Function Prototype
/////////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

// Error Function Prototype //
void Error(bool Fatal, char* Text);

/////////////////////////////////////////////////
// Base Game Class
/////////////////////////////////////////////////
class cBaseGame
{
private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;

protected:
	WORD m_Width, m_Height;
	char m_AppTitle[MAX_PATH];

public:
	// Constructor/Destructor 
	cBaseGame();
	~cBaseGame(){}

	// Base Game Run Function 
	bool BaseRun();
	
	// Overloaded Main Game Functions
	virtual bool GameInit(){return true;}
	virtual bool GameRun(){return true;}
	virtual bool GameEnd(){return true;}

	// Functions
	void ShowMouse(bool Show=true);

	// Accessor Methods 
	HWND GetHWND()const{return m_hWnd;}
	HINSTANCE GetHINSTANCE()const{return m_hInstance;}
	WORD GetWidth()const{return m_Width;}
	WORD GetHeight()const{return m_Height;}
};

/////////////////////////////////////////////////
// cStateMachine class/State purpose enumeration
/////////////////////////////////////////////////
enum Purposes 
{
    NONE = 0,
    INIT,
    SHUTDOWN,
    FRAME
};

class cStateMachine
{
private:
	// State function pointers in a linked list
	struct sState
	{
		void (*Function)(void *Ptr, long Purpose);
		sState *Next;

		sState(){Function = NULL; Next = NULL;}
		~sState(){delete Next;}
	};

protected:
	sState *m_StateParent;

public:
	// Constructor/Destructor
	cStateMachine(){m_StateParent = NULL;}
	~cStateMachine(){PopAll();}

	// Functions
	void Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr = NULL);
	bool Pop(void *DataPtr = NULL);
	void PopAll(void *DataPtr = NULL);
	bool Process(void *DataPtr = NULL);
};

#endif 