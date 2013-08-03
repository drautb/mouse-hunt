///////////////////////////////////////////////////////////////////////////////////////////////////
// File Title: Audio_Engine.cpp
// Project: Game Engine
// Author: Ben Draut
// Desc: Source file for Audio aspect of Game Engine
// Last Updated: 5/22/06
///////////////////////////////////////////////////////////////////////////////////////////////////

// Include everything
#include "Global_Engine.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// cAudio stuff
///////////////////////////////////////////////////////////////////////////////////////////////////
bool cAudio::InitDirectSound(HWND hWnd, DWORD CooperativeLevel)
{
	// Create DirectSound object
	m_DirectSound = new CSoundManager();
	HRESULT Result = m_DirectSound->Initialize(hWnd, CooperativeLevel);
	if (Result != DS_OK)
		Error(false, "Error initializing DirectSound, may experience audio failure.");

	// Return success
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// cWAVSound stuff
///////////////////////////////////////////////////////////////////////////////////////////////////
bool cWAVSound::LoadWAVFile(cAudio *ParentAudio, char *Filename)
{
	m_ParentAudio = ParentAudio;
	HRESULT Result = m_ParentAudio->GetDS()->Create(&m_Sound, Filename);
	if (Result != DS_OK)
	{
		Error(false, "Error loading WAVE sound file.");
		return false;
	}

	return true;
}

void cWAVSound::Play(DWORD Flags)
{
	// Play the sound
	m_Sound->Play(0, Flags);
}

void cWAVSound::Stop()
{
	// Stop the sound
	m_Sound->Stop();
}





