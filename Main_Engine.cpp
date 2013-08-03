/////////////////////////////////////////////////
// Main_Engine.cpp
// Main Engine Source File
// Developed By: Ben Draut
// Last Updated: 3/1/06
/////////////////////////////////////////////////

// Include Everything
#include "Global_Engine.h"

/////////////////////////////////////////////////
// Window Procedure Function Definition
/////////////////////////////////////////////////
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
}

// Error Function Definition 
void Error(bool Fatal, char* Text)
{
	if (Fatal)
	{
		MessageBox(NULL, Text, "ERROR", MB_OK | MB_ICONSTOP);
		PostQuitMessage(0);
	}
	else
		MessageBox(NULL, Text, "ERROR", MB_OK | MB_ICONEXCLAMATION);
}

/////////////////////////////////////////////////
// cBaseGame Stuff
/////////////////////////////////////////////////
cBaseGame::cBaseGame()
{
	// Get Instance Handle
	m_hInstance = GetModuleHandle(NULL);

	// Setup Window Attributes
	m_Width = DEFAULT_WIDTH;
	m_Height = DEFAULT_HEIGHT;

	strcpy(m_AppTitle, "Title");
	
	WNDCLASSEX WndClass;
	WndClass.lpszClassName	= "WndClass";
	WndClass.cbSize			= sizeof(WNDCLASSEX);
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 0;
	WndClass.hbrBackground  = (HBRUSH)0; // White Background Default
	WndClass.hCursor		= LoadCursor(m_hInstance, IDC_ARROW);
	WndClass.hIcon			= LoadIcon(m_hInstance, IDI_APPLICATION);
	WndClass.hIconSm		= LoadIcon(m_hInstance, IDI_APPLICATION);
	WndClass.hInstance		= m_hInstance;
	WndClass.lpfnWndProc	= WindowProc;
	WndClass.lpszMenuName	= NULL;
	WndClass.style			= CS_CLASSDC;

	// Register Window Class
	if (!RegisterClassEx(&WndClass))
		Error(true, "Error Registering Window Class,\n\tExiting...");

}

bool cBaseGame::BaseRun()
{
	int Timer = GetTickCount();

	// Create, Show and Update the Window 
	m_hWnd = CreateWindow("WndClass", m_AppTitle, WS_OVERLAPPEDWINDOW, 0, 0, m_Width, m_Height,
						  NULL, NULL, m_hInstance, NULL);
	if (!m_hWnd)
		Error(true, "Error Creating Window,\n\tExiting...");
	ShowWindow(m_hWnd, SW_NORMAL);
	UpdateWindow(m_hWnd);

	MSG Msg;

	if (GameInit() == true)
	{
		ZeroMemory(&Msg, sizeof(MSG));
		while (Msg.message != WM_QUIT)
		{
			if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
			else
			{				
				if (!GameRun())
					return false;
			}
		}
	}

	GameEnd();

	UnregisterClass(m_AppTitle, m_hInstance);

	return true;
}

void cBaseGame::ShowMouse(bool Show)
{
	Show == true ? ShowCursor(TRUE) : ShowCursor(FALSE);
}

/////////////////////////////////////////////////
// cStateMachine stuff 
/////////////////////////////////////////////////
void cStateMachine::Push(void (*Function)(void *Ptr, long Purpose), void *DataPtr)
{
	sState *StatePtr;

	if(Function != NULL) 
	{
        // Allocate memory for state on stack, and call it with Init purpose
		StatePtr = new sState();
		StatePtr->Function = Function;
		StatePtr->Next = m_StateParent;
		m_StateParent = StatePtr;
		StatePtr->Function(DataPtr, INIT);
	}
}

bool cStateMachine::Pop(void *DataPtr)
{
	sState *StatePtr;
	if((StatePtr = m_StateParent) != NULL) 
	{
		m_StateParent->Function(DataPtr, SHUTDOWN);
		m_StateParent = StatePtr->Next;
		StatePtr->Next = NULL;
		delete StatePtr;
	}

	if(m_StateParent == NULL)
		return false;

	return true;
}

void cStateMachine::PopAll(void *DataPtr)
{
	while(Pop(DataPtr) == TRUE);
}

bool cStateMachine::Process(void *DataPtr)
{ 
	if(m_StateParent == NULL)
		return true;

	m_StateParent->Function(DataPtr, FRAME); 

	return TRUE;
}










