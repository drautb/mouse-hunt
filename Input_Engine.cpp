/////////////////////////////////////////////////
// Input_Engine.cpp
// Input Aspect of Game Engine source File
// Developed By: Ben Draut
/////////////////////////////////////////////////

// Include everthing 
#include "Global_Engine.h"

/////////////////////////////////////////////////
// Function definitions 
/////////////////////////////////////////////////
bool cKeyboard::IsKeyDown(short vk_code)
{
	if (GetAsyncKeyState(vk_code) & 0x8000)
		return true;

	return false;
}

bool cKeyboard::IsKeyUp(short vk_code)
{
	if (!(GetAsyncKeyState(vk_code) & 0x8000))
		return true;

	return false;
}

void cKeyboard::UpdateKeys()
{
	for (short i=0; i<256; i++)
	{
		if (IsKeyUp(i))
			KeyLocks[i] = FALSE;
	}
}

void cKeyboard::SetLock(short vk_code)
{
	KeyLocks[vk_code] = TRUE;
}

BOOL cKeyboard::GetLock(short vk_code)
{
	return KeyLocks[vk_code];
}