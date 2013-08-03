/////////////////////////////////////////////////
// Input_Engine.h
// Input Aspect of Game Engine Header File
// Developed By: Ben Draut
/////////////////////////////////////////////////

#ifndef INPUT_ENGINE_H
#define INPUT_ENGINE_H

/////////////////////////////////////////////////
// cKeyboard class 
/////////////////////////////////////////////////
class cKeyboard
{
public:
	// Variables
	// Array of BOOL values that designate if a key is locked or not
	BOOL KeyLocks[256];

	// Functions
	bool IsKeyDown(short vk_code); 
	bool IsKeyUp(short vk_code);  
	void UpdateKeys();
	void SetLock(short vk_code);
	BOOL GetLock(short vk_code);
};

#endif