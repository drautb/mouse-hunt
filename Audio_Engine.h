///////////////////////////////////////////////////////////////////////////////////////////////////
// File Title: Audio_Engine.h
// Project: Game Engine
// Author: Ben Draut
// Desc: Header file for Audio aspect of Game Engine, Utilizes dxutil and dsutil files
// Last Updated: 5/22/06
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

/////////////////////////////////////////////////
// Main Audio Class
/////////////////////////////////////////////////
class cAudio
{
private:
	// Main sound object
	CSoundManager *m_DirectSound;

public:
	// Constructor/Destructor
	cAudio(){}
	~cAudio(){}

	// Member methods
	bool InitDirectSound(HWND hWnd, DWORD CooperativeLevel=DSSCL_NORMAL);

	// Accessor methods
	CSoundManager *GetDS()const{return m_DirectSound;}
	
};

/////////////////////////////////////////////////
// Main WAV sound class
/////////////////////////////////////////////////
class cWAVSound
{
private:
	// Pointer to the parent cAudio object
	cAudio *m_ParentAudio;

	// Sound object
	CSound *m_Sound;

public:
	// Constructor/Destructor
	cWAVSound(){}
	~cWAVSound(){}

	// Member methods
	bool LoadWAVFile(cAudio *ParentAudio, char *Filename);
	void Play(DWORD Flags=0);
	void Stop();

	// Accessor Methods
	cAudio *GetParentAudio()const{return m_ParentAudio;}
	CSound *GetSound()const{return m_Sound;}
};

#endif