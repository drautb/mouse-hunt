/////////////////////////////////////////////////
// WinMain.cpp
// Main File of Mouse Hunt
// Utilizes Game Engine Developed by Ben Draut
// Developed By: Ben Draut
// Last Updated: 5/19/06
/////////////////////////////////////////////////

// Include Everything
#include "Global_Engine.h"

/////////////////////////////////////////////////
// Main Game Functions
/////////////////////////////////////////////////
// Derived Game Constructor
cGame::cGame()
{
	m_Width = 640;
	m_Height = 480;
	strcpy(m_AppTitle, "Mouse Hunt v1.0");
}

bool cGame::GameInit()
{
	// Seed random generator
	srand((unsigned)time(0));

	// Load audio
	if (!m_Audio.InitDirectSound(GetHWND(), DSSCL_PRIORITY))
		Error(false, "Audio systems failure.");
	// Music
	if (!m_Music.LoadWAVFile(&m_Audio, "Data/Music.wav"))
		Error(false, "Error loading music, unable to play music.");
	// Sounds
	if (!m_EatenMouse.LoadWAVFile(&m_Audio, "Data/Eaten Mouse.wav"))
		Error(false, "Error loading WAV file.");
	if (!m_Frenzy.LoadWAVFile(&m_Audio, "Data/Frenzy.wav"))
		Error(false, "Error loading WAV file.");
	if (!m_CatAttack.LoadWAVFile(&m_Audio, "Data/Cat Attack.wav"))
		Error(false, "Error loading WAV file.");
	if (!m_Clapping.LoadWAVFile(&m_Audio, "Data/Clapping.wav"))
		Error(false, "Error loading WAV file.");
	if (!m_Klunk.LoadWAVFile(&m_Audio, "Data/Klunk.wav"))
		Error(false, "Error loading WAV file.");
	if (!m_BeepBeep.LoadWAVFile(&m_Audio, "Data/Beep Beep.wav"))
		Error(false, "Error loading WAV file.");

	// Initialize Graphics
	//if (!m_Graphics.GraphicsInit(GetHWND(), true, m_Width, m_Height)) // Fullscreen
	if (!m_Graphics.GraphicsInit(GetHWND(), false, m_Width, m_Height)) // Windowed
		Error(true, "Error initializing Direct3D!\n\tExiting..."); 
	if (!m_Tile.Create(&m_Graphics, 1))
		Error(true, "Error initializing tile engine,\n\tExiting...");
	ShowMouse(false);

	// Load tile engine textures
	if (!m_Tile.Load(0, "Data/Mouse Hunt Tiles.bmp"))
		Error(false, "Error loading tile engine texture.");

	// Load general textures 
	if (!m_MenuTex.Load(&m_Graphics, "Data/Mouse Hunt Title.bmp", D3DCOLOR_RGBA(0,0,0,255)))
		Error(false, "Error loading menu texture, unable to display menu.");
	if (!m_PauseTex.Load(&m_Graphics, "Data/Pause Menu.bmp", D3DCOLOR_RGBA(255,255,255,255)))
		Error(false, "Error loading pause menu texture, unable to display pause menu.");
	if (!m_InstructTex.Load(&m_Graphics, "Data/Instructions.bmp", D3DCOLOR_RGBA(0,0,0,255)))
		Error(false, "Error loading instructions texture, unable to display instructions.");
	if (!m_MouseHuntTex.Load(&m_Graphics, "Data/Mouse Hunt.bmp", D3DCOLOR_RGBA(0,0,0,255)))
		Error(false, "Error loading mouse hunt texture.");
	if (!m_FrenzyTex.Load(&m_Graphics, "Data/Frenzy.bmp", D3DCOLOR_RGBA(0,0,0,255)))
		Error(false, "Error loading frenzy texture.");
	if (!m_CatAttackTex.Load(&m_Graphics, "Data/Cat Attack.bmp", D3DCOLOR_RGBA(0,0,0,255)))
		Error(false, "Error loading cat attack texture.");

	// Initialize font object
	if (!m_Font.Create(&m_Graphics, "Arial"))
		Error(false, "Error initializing font object, unable to display text.");

	// Variable initialization 
	m_StartTime = 0;
	m_CurrentNumber = 0;
	m_NumX = m_NumY = 0;
	m_Scaler = 0;
	m_NumMiceOut = 0;
	m_Winner = -1;
	m_CurrentLeader = -1;
	m_NumMiceIn = MICE_TO_DEPLOY;
	m_DeployMice = true;
	m_InAFrenzy = m_CatsAreAttacking = false;
	m_AIDelayCount = 0;
	m_AIDelay = 40;

	// Push menu state onto state stack 
	m_SM.Push(Menu, this);

	return true;
}

bool cGame::GameRun()
{
	// Unlock released keys 
	m_KB.UpdateKeys();

	// Always play music
	m_Music.Play(DSBPLAY_LOOPING);

	// Clear Screen 
	m_Graphics.ClearScreen();		

	// Process current state 
	m_SM.Process(this);
	
	// Flip Backbuffer 
	m_Graphics.Present();

	return true;
}

bool cGame::GameEnd()
{
	// Free Tile engine and font
	m_Tile.Free();
	m_Font.Free();

	// Free Textures 
	if (!m_MenuTex.Free())
		Error(true, "Error releasing texture memory,\nExiting...");
	if (!m_PauseTex.Free())
		Error(true, "Error releasing texture memory,\nExiting...");
	if (!m_InstructTex.Free())
		Error(true, "Error releasing texture memory,\nExiting...");
	if (!m_MouseHuntTex.Free())
		Error(true, "Error releasing texutre memory,\nExiting...");
	if (!m_FrenzyTex.Free())
		Error(true, "Error releasing texutre memory,\nExiting...");
	if (!m_CatAttackTex.Free())
		Error(true, "Error releasing texutre memory,\nExiting...");

	// Release all other graphics objects
	if (!m_Graphics.GraphicsEnd())
		Error(true, "Error releasing DirectX Graphics objects,\nExiting...");

	return true;
}

/////////////////////////////////////////////////
// State Functions
/////////////////////////////////////////////////
void cGame::Menu(void *DataPtr, long Purpose)
{
	// pointer to calling class
	cGame *cc = (cGame*)DataPtr;

	if (Purpose == INIT)
	{
		// Reset menu choice to single player
		cc->m_MenuChoice = SINGLE;
	}
	if (Purpose == FRAME)
	{
		// Per-frame processing 
		cc->UpdateMenu();

		// Per-frame rendering 
		if (cc->m_Graphics.BeginScene())
		{
			// Render menu images 
			cc->DrawMenu();

			// End rendering 
			cc->m_Graphics.EndScene();
		}

	}
	if (Purpose == SHUTDOWN)
	{

	}
}

// Instructions state function 
void cGame::Instruct(void *DataPtr, long Purpose)
{
	// Calling class pointer
	cGame *cc = (cGame*)DataPtr;

	if (Purpose == INIT)
	{
		// Nothing to initialize
	}
	if (Purpose == FRAME)
	{
		// Check for esc key 
		if (cc->m_KB.IsKeyDown(VK_ESCAPE))
		{
			cc->m_SM.Pop(cc);
			cc->m_SM.Push(Menu, cc);
		}

		// Render the instruction screen 
		if (cc->m_Graphics.BeginScene())
		{
			cc->m_InstructTex.Render(0, 0, NULL);

			// End rendering 
			cc->m_Graphics.EndScene();
		}
	}
	if (Purpose == SHUTDOWN)
	{
		// Nothing to clean up
	}
}

// Single player state function 
void cGame::Single(void *DataPtr, long Purpose)
{
	// Calling class pointer 
	cGame *cc = (cGame*)DataPtr;

	if (Purpose == INIT)
	{
		cc->m_SM.Push(Countdown, cc);
		cc->InitObjects();
	}
	if (Purpose == FRAME)
	{
		// Handle input 
		cc->HandleP1Input();

		// Update game objects and other things 
		cc->UpdateGame(true);

		// Render stuff 
		if (cc->m_Graphics.BeginScene())
		{
			// Render game 
			cc->DrawGame();

			cc->m_Graphics.EndScene();
		}

		// Exit on esc key  or if there is a winner
		if (cc->m_Winner >= 0)
		{
			cc->m_SM.Pop(cc);
			cc->m_SM.Push(GameOver, cc);
		}
		else if (cc->m_KB.IsKeyDown(VK_ESCAPE))
			cc->m_SM.Push(Pause, cc);
	}
	if (Purpose == SHUTDOWN)
	{
		cc->ShutdownObjects();
	}
}

// Multiplayer state function 
void cGame::Multi(void *DataPtr, long Purpose)
{
	// Calling class pointer 
	cGame *cc = (cGame*)DataPtr;

	if (Purpose == INIT)
	{
		cc->m_SM.Push(Countdown, cc);
		cc->InitObjects();
	}
	if (Purpose == FRAME)
	{
		// Handle input 
		cc->HandleP1Input();
		cc->HandleP2Input();

		// Update game objects and other things 
		cc->UpdateGame(false);

		// Render stuff 
		if (cc->m_Graphics.BeginScene())
		{
			// Render game 
			cc->DrawGame();

			cc->m_Graphics.EndScene();
		}

		// Exit on esc key  or if there is a winner
		if (cc->m_Winner >= 0)
		{
			cc->m_SM.Pop(cc);
			cc->m_SM.Push(GameOver, cc);
		}
		else if (cc->m_KB.IsKeyDown(VK_ESCAPE))
			cc->m_SM.Push(Pause, cc);
	}
	if (Purpose == SHUTDOWN)
	{
		cc->ShutdownObjects();
	}
}

// Game Over state function
void cGame::GameOver(void *DataPtr, long Purpose)
{
	// pointer to the game class that is calling the function
	cGame *Game = (cGame*)DataPtr;

	if (Purpose == INIT)
	{
		// Play applause sound
		Game->m_Clapping.Play();
		// Init text position
		Game->m_TextX = Game->m_TextY = 80;
		Game->m_TextMoveX = Game->m_TextMoveY = 1;
	}
	if (Purpose == FRAME)
	{
		// Update text position
		Game->UpdateTextPosition();
		
		// Render winning text, etc.
		if (Game->m_Graphics.BeginScene())
		{
			// Print Text
			Game->DrawWinText();

			// End rendering
			Game->m_Graphics.EndScene();
		}
		
		// See if the player is ready to go back to the menu
		if (Game->m_KB.IsKeyDown(VK_SPACE))
		{
			Game->m_SM.Pop(Game);
			Game->m_SM.Push(Menu, Game);
		}
	}
	if (Purpose == SHUTDOWN)
	{
		// Upon exiting, reset the winner variable
		Game->m_Winner = -1;
	}
}

// Wave countdown state function 
void cGame::Countdown(void *DataPtr, long Purpose)
{
	// Initialize calling class pointer 
	cGame *cc = (cGame*)DataPtr;

	if (Purpose == INIT)
	{
		cc->m_CountdownExpired = false;
		cc->m_CurrentNumber = 5;
		cc->m_StartTime = GetTickCount();
		cc->m_Scaler = 0.0f;
		cc->m_NumX = 0;
		cc->m_NumY = 0;
	}
	if (Purpose == FRAME)
	{
		// Update everything
		cc->UpdateCountdown();
		
		// Render everything 
		if (cc->m_Graphics.BeginScene())
		{
			cc->DrawCountdown();

			cc->m_Graphics.EndScene();
		}

		// Check for escape
		if (cc->m_KB.IsKeyDown(VK_ESCAPE))
			cc->m_SM.Pop(cc);
	}
	if (Purpose == SHUTDOWN)
	{
		cc->m_SM.Push(Menu, cc);
	}
}

// Pause state function
void cGame::Pause(void *DataPtr, long Purpose)
{
	// Initialize pointer to main game
	cGame *Game = (cGame*)DataPtr;

	if (Purpose == INIT)
	{
		// Init menu choice
		Game->m_MenuChoice = 1;  // Current choice = Continue Game
	}
	if (Purpose == FRAME)
	{
		// Update menu
		bool ContinueGame = Game->UpdatePauseMenu();

		// Draw everything
		if (Game->m_Graphics.BeginScene())
		{
			// Render game
			Game->DrawGame();

			// Draw pause menu
			Game->m_PauseTex.Render(100, 130);

			// Draw cheese arrow on pause menu
			switch (Game->m_MenuChoice)
			{
			case 0:
				Game->m_Tile.DrawTile(0, 28, 150, 235);
				break;
			case 1:
				Game->m_Tile.DrawTile(0, 28, 150, 190);
				break;
			default:
				Error(false, "Error drawing cheese selector.");
				break;
			}

			// End scene
			Game->m_Graphics.EndScene();
		}

		// Check for esc key
		if (!ContinueGame)
		{
			Game->m_SM.Pop(Game);
			Game->m_SM.Push(Menu, Game);
		}
	}
	if (Purpose == SHUTDOWN)
	{

	}
}

/////////////////////////////////////////////////
// Other Functions 
/////////////////////////////////////////////////
void cGame::InitObjects()
{
	// Initialize cursors 
	for (int i=0; i<4; i++)
	{
		m_Cursors[i] = new cCursor(&m_Tile, i, i+4);
	}
	// Set initial positions
	m_Cursors[0]->SetX(96);
	m_Cursors[0]->SetY(96);
	m_Cursors[1]->SetX(352);
	m_Cursors[1]->SetY(352);
	m_Cursors[2]->SetX(352);
	m_Cursors[2]->SetY(96);
	m_Cursors[3]->SetX(96);
	m_Cursors[3]->SetY(352);
	// Set coordinates of mouse holes
	m_Cursors[0]->SetHoleX(RED_HOLE_X);
	m_Cursors[0]->SetHoleY(RED_HOLE_Y);
	m_Cursors[1]->SetHoleX(BLUE_HOLE_X);
	m_Cursors[1]->SetHoleY(BLUE_HOLE_Y);
	m_Cursors[2]->SetHoleX(GREEN_HOLE_X);
	m_Cursors[2]->SetHoleY(GREEN_HOLE_Y);
	m_Cursors[3]->SetHoleX(YELLOW_HOLE_X);
	m_Cursors[3]->SetHoleY(YELLOW_HOLE_Y);

	// Initialize mice information (1 in 4 chance of a frenzy)
	// Set all mice to dead
	for (int i=0; i<TOTAL_MICE; i++)
		m_MiceAlive[i] = false;

	// Init cats for Cat Attack
	for (int i=0; i<TOTAL_CATS; i++)
	{
		int Temp = rand()%14;
		m_Cats[i] = new cCat(&m_Tile, -450, Temp*32, 1, 0, 32, 32, 37, 0, 50);
	}

	// Reset the array of Occupied spaces
	for (int i=0; i<13; i++)
	{
		for (int j=0; j<13; j++)
			Occupied[i][j] = FALSE;
	}
	// Mark mouse holes and spawn point as occuipied
	Occupied[2][2] = Occupied[10][2] = Occupied[2][10] = Occupied[10][10] = TRUE;
	Occupied[6][6] = TRUE;

	// Initialize the first wave
	StartNewWave();
}

void cGame::UpdateMenu()
{
	// Handle input for menu choices 
	if (m_KB.IsKeyDown(VK_DOWN) && !m_KB.GetLock(VK_DOWN))
	{
		m_KB.SetLock(VK_DOWN);
		m_MenuChoice++;
	}
	if (m_KB.IsKeyDown(VK_UP) && !m_KB.GetLock(VK_UP))
	{
		m_KB.SetLock(VK_UP);
		m_MenuChoice--;
	}

	// Don't let menu choice value exceed limits 
	if (m_MenuChoice > EXIT)
		m_MenuChoice = EXIT;
	if (m_MenuChoice < SINGLE)
		m_MenuChoice = SINGLE;

	// Change state according to input 
	if (m_KB.IsKeyDown(VK_RETURN) && !m_KB.GetLock(VK_RETURN))
	{
		m_KB.SetLock(VK_RETURN);
		switch (m_MenuChoice)
		{
		case SINGLE:
			m_SM.Pop();
			m_SM.Push(Single, this);
			break;
		case MULTI:
			m_SM.Pop();
			m_SM.Push(Multi, this);
			break;
		case INSTRUCT:
			m_SM.Pop();
			m_SM.Push(Instruct, this);
			break;
		case EXIT:
			PostQuitMessage(0);
		default:
			break;
		}
	}
}

bool cGame::UpdatePauseMenu()
{
	// 1=Continue, 0=Quit to menu
	if (m_KB.IsKeyDown(VK_UP))
		m_MenuChoice = 1;      
	if (m_KB.IsKeyDown(VK_DOWN))
		m_MenuChoice = 0;

	// Evaluate choice
	if (m_KB.IsKeyDown(VK_RETURN) && !m_KB.GetLock(VK_RETURN))
	{
		m_KB.SetLock(VK_RETURN);
		if (m_MenuChoice == 1) // Continue
		{
			m_SM.Pop();
			return true;
		}
		if (m_MenuChoice == 0) // Quit to Main Menu
		{
			m_SM.Pop();
			return false;
		}
	}

	return true;
}

void cGame::UpdateGame(bool SinglePlayer)
{
	// Update arrow timers
	UpdateArrows();
	
	// Update AI
	++m_AIDelayCount;
	if (m_AIDelayCount > m_AIDelay)
	{
		m_AIDelayCount = 0;
		if (SinglePlayer)
		{
			UpdateAI(1);
			UpdateAI(2);
			UpdateAI(3);
		}
		else
		{
			UpdateAI(2);
			UpdateAI(3);
		}
	}

	// Update cursor positions, then reset their move values 
	for (int i=0; i<4; i++)
	{	
		// But first, has anyone won?
		if (m_Cursors[i]->GetScore() > WINNING_AMOUNT)
		{
			m_Winner = i;		
			return;
		}

		m_Cursors[i]->Update();
		m_Cursors[i]->SetXMove(0);
		m_Cursors[i]->SetYMove(0);
		if (m_Cursors[i]->GetX() > 416)
			m_Cursors[i]->SetX(416);
		if (m_Cursors[i]->GetX() < 32)
			m_Cursors[i]->SetX(32);
		if (m_Cursors[i]->GetY() > 416)
			m_Cursors[i]->SetY(416);
		if (m_Cursors[i]->GetY() < 32)
			m_Cursors[i]->SetY(32);
	}

	// Deploy mouse if needed
	if (m_InAFrenzy)
	{
		if (GetTickCount() % FRENZY_DEPLOY_DELAY == 0)
		{
			if (m_DeployMice == true)
				DeployMouse(m_NumMiceOut);
		}
	}
	else
	{
		if (GetTickCount() % DEPLOY_DELAY == 0)
		{
			if (m_DeployMice == true)
				DeployMouse(m_NumMiceOut);
		}
	}

	// Update mice that are alive
	for (int i=0; i<TOTAL_MICE; i++)
	{
		if (m_MiceAlive[i])
		{
			HandleMouseCollisions(m_Mice[i]);		
			m_Mice[i]->Update();
		}
	}

	// Update for Cat Attack
	if (m_CatsAreAttacking)
	{
		for (int i=0; i<TOTAL_CATS; i++)
		{
			m_Cats[i]->Update();
			HandleCatCollisions(m_Cats[i]);
			if (m_Cats[i]->GetX() > 640)
			{
				m_CatsAreAttacking = false;
				for (int i=0; i<TOTAL_CATS; i++)
					m_Cats[i]->SetX(-450);
			}
		}
	}

	// Whoever has the highest score currently is the leader
	if (m_Cursors[0]->GetScore() > m_Cursors[1]->GetScore() &&
		m_Cursors[0]->GetScore() > m_Cursors[2]->GetScore() &&
		m_Cursors[0]->GetScore() > m_Cursors[3]->GetScore())
		m_CurrentLeader = 0; // Player 0, (you, red) is currently winning
	else if (m_Cursors[1]->GetScore() > m_Cursors[0]->GetScore() &&
		m_Cursors[1]->GetScore() > m_Cursors[2]->GetScore() &&
		m_Cursors[1]->GetScore() > m_Cursors[3]->GetScore())
		m_CurrentLeader = 1; // Player 1 (Blue) is currently winning
	else if (m_Cursors[2]->GetScore() > m_Cursors[0]->GetScore() &&
		m_Cursors[2]->GetScore() > m_Cursors[1]->GetScore() &&
		m_Cursors[2]->GetScore() > m_Cursors[3]->GetScore())
		m_CurrentLeader = 2; // Player 2 (Green) is currently winning
	else if (m_Cursors[3]->GetScore() > m_Cursors[0]->GetScore() &&
		m_Cursors[3]->GetScore() > m_Cursors[1]->GetScore() &&
		m_Cursors[3]->GetScore() > m_Cursors[2]->GetScore())
		m_CurrentLeader = 3; // Player 1 (Blue) is currently winning
	else 
		m_CurrentLeader = -1;
}

void cGame::UpdateArrows()
{
	// Update timer for arrows
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{		
			if (m_Cursors[i]->m_Arrows[j]->GetDead() == false)
			{
				m_Cursors[i]->m_Arrows[j]->SetTimer(m_Cursors[i]->m_Arrows[j]->GetTimer()+1);
				if (m_Cursors[i]->m_Arrows[j]->GetTimer() > ARROW_LIFESPAN)
				{
					// Reset some arrow properties
					m_Cursors[i]->m_Arrows[j]->SetTimer(0);
					// The arrow is dead			
					m_Cursors[i]->m_Arrows[j]->SetDead(true);
					// The space is no longer occupied
					int X = ((m_Cursors[i]->m_Arrows[j]->GetX())/32)-1;
					int Y = ((m_Cursors[i]->m_Arrows[j]->GetY())/32)-1;
					Occupied[X][Y] = 0;
				}
			}
		}
	}
}

void cGame::UpdateAI(int PlayerNumber)
{
	// Save typespace ('cause im lazy ;))
	int P = PlayerNumber;
	// If the bot doesn't have a target, get one! 
	 // if this is 0, the player puts an arrow in front of the 
								 // currently winning players hole, if 1, then they put an arrow on a hot spot.
	if (m_Cursors[P]->GetTargetX() <= 0 && m_Cursors[P]->GetTargetY() <= 0)
	{
		int Temp = -1;
		m_Cursors[P]->SetActionChoice(rand()%2); // if this is 0, the player puts an arrow in front of the 
												 // currently winning players hole, if 1, then they put an arrow on a hot spot.
		if (m_Cursors[P]->GetActionChoice() == 1)
		{
			Temp = rand()%4;
			m_Cursors[P]->SetTargetX(HotSpots[P-1][Temp][0]);
			m_Cursors[P]->SetTargetY(HotSpots[P-1][Temp][1]);
		}
		else if (m_Cursors[P]->GetActionChoice() == 0 && m_CurrentLeader != P &&  m_CurrentLeader >= 0)
		{
			// Pick a random side of the current leaders hole to 
			// place an arrow directing mice away from there.
			Temp = rand()%4;
			switch (Temp)
			{
			case 0:
				m_Cursors[P]->SetTargetX(m_Cursors[m_CurrentLeader]->GetHoleX()+32);
				m_Cursors[P]->SetTargetY(m_Cursors[m_CurrentLeader]->GetHoleY());
				break;
			case 1:
				m_Cursors[P]->SetTargetX(m_Cursors[m_CurrentLeader]->GetHoleX());
				m_Cursors[P]->SetTargetY(m_Cursors[m_CurrentLeader]->GetHoleY()+32);
				break;
			case 2:
				m_Cursors[P]->SetTargetX(m_Cursors[m_CurrentLeader]->GetHoleX()-32);
				m_Cursors[P]->SetTargetY(m_Cursors[m_CurrentLeader]->GetHoleY());
				break;
			case 3:
				m_Cursors[P]->SetTargetX(m_Cursors[m_CurrentLeader]->GetHoleX());
				m_Cursors[P]->SetTargetY(m_Cursors[m_CurrentLeader]->GetHoleY()-32);
				break;
			default:
				break;
			}
		}
	}
	
	// place an arrow, or get in position
	if (m_Cursors[P]->GetX() == m_Cursors[P]->GetTargetX() && 
		m_Cursors[P]->GetY() == m_Cursors[P]->GetTargetY())
	{
		if (m_Cursors[P]->GetActionChoice() == 1)
		{
			// Place the appropriate arrow
			if (m_Cursors[P]->GetX() > m_Cursors[P]->GetHoleX())
				m_Cursors[P]->PlaceArrow(LEFT);
			else if (m_Cursors[P]->GetX() < m_Cursors[P]->GetHoleX())
				m_Cursors[P]->PlaceArrow(RIGHT);
			else if (m_Cursors[P]->GetY() > m_Cursors[P]->GetHoleY())
				m_Cursors[P]->PlaceArrow(UP);
			else if (m_Cursors[P]->GetY() < m_Cursors[P]->GetHoleY())
				m_Cursors[P]->PlaceArrow(DOWN);
		}
		else if (m_Cursors[P]->GetActionChoice() == 0)
		{
			if (m_Cursors[P]->GetX() > m_Cursors[m_CurrentLeader]->GetHoleX())
				m_Cursors[P]->PlaceArrow(RIGHT);
			else if (m_Cursors[P]->GetX() < m_Cursors[m_CurrentLeader]->GetHoleX())
				m_Cursors[P]->PlaceArrow(LEFT);
			else if (m_Cursors[P]->GetY() > m_Cursors[m_CurrentLeader]->GetHoleY())
				m_Cursors[P]->PlaceArrow(DOWN);
			else if (m_Cursors[P]->GetY() < m_Cursors[m_CurrentLeader]->GetHoleY())
				m_Cursors[P]->PlaceArrow(UP);
		}

		// Reset target
		m_Cursors[P]->SetTargetX(0);
		m_Cursors[P]->SetTargetY(0);
	}
	else if (m_Cursors[P]->GetX() < m_Cursors[P]->GetTargetX())
		m_Cursors[P]->SetXMove(32);
	else if (m_Cursors[P]->GetX() > m_Cursors[P]->GetTargetX())
		m_Cursors[P]->SetXMove(-32);
	else if (m_Cursors[P]->GetY() < m_Cursors[P]->GetTargetY())
		m_Cursors[P]->SetYMove(32);
	else if (m_Cursors[P]->GetY() > m_Cursors[P]->GetTargetY())
		m_Cursors[P]->SetYMove(-32);

}	

void cGame::UpdateTextPosition()
{
	// Add text move values to coordinates, check for collisions
	if (m_TextX < 0)
	{
		m_TextMoveX = 1;
		m_TextX += 1;
	}
	if (m_TextY < 0)
	{
		m_TextMoveY = 1;
		m_TextY += 1;
	}
	if (m_TextX > 410)
	{
		m_TextMoveX = -1;
		m_TextX += -1;
	}
	if (m_TextY > 448)
	{
		m_TextMoveY = -1;
		m_TextY += -1;
	}

	m_TextX += m_TextMoveX;
	m_TextY += m_TextMoveY;
}

void cGame::UpdateCountdown()
{
	if (m_CountdownExpired)
	{
		Sleep(1000);
		m_SM.Pop(this);
	}

	// Update state 
	if (GetTickCount() > (m_StartTime + 1000))
	{
		m_CurrentNumber--;
		m_StartTime = GetTickCount();
		m_Scaler = 0.0f;
		m_NumX = 0;
		m_NumY = 0;
	}
	else
	{
		m_Scaler = m_Scaler + 0.07f;
		if (m_Scaler > 10.0f)
			m_Scaler = 10.0f;
		else
		{
			m_NumX += 2;
			m_NumY += 1;
		}
		if (m_NumX > 320)
			m_NumX = 320;
		if (m_NumY > 240)
			m_NumY = 240;
	}
}

void cGame::DeployMouse(int MouseNumber)
{
	// Has the deployment threshold been reached
	if (m_InAFrenzy == true && MouseNumber >= FRENZY_MICE_TO_DEPLOY)
	{
		m_DeployMice = false;
		m_InAFrenzy = false;
	}
	if (m_InAFrenzy == false && MouseNumber >= MICE_TO_DEPLOY)
	{
		m_DeployMice = false;
		m_InAFrenzy = false;
	}
	int Temp = (rand()%4);
	int TempXMove = 0, TempYMove = 0;
	MouseDirection Heading = MOUSE_NO_DIRECTION;
	switch (Temp)
	{
	case 0:
		TempXMove = 1;
		Heading = MOUSE_RIGHT;
		break;
	case 1:
		TempYMove = 1;
		Heading = MOUSE_DOWN;
		break;
	case 2:
		TempXMove = -1;
		Heading = MOUSE_LEFT;
		break;
	case 3:
		TempYMove = -1;
		Heading = MOUSE_UP;
		break;
	default:
		TempXMove = TempYMove = 0;
		Heading = MOUSE_NO_DIRECTION;
		break;
	}

	if (m_MiceAlive[MouseNumber] == false)
	{
		m_Mice[MouseNumber] = new cMouse(&m_Tile, MouseNumber, MOUSE_HOLE_X, MOUSE_HOLE_Y, TempXMove, TempYMove, 
										 30, 30, 31, 0, 30, Heading);
		m_MiceAlive[MouseNumber] = true;
		m_NumMiceOut++;
	}
}

void cGame::StartNewWave()
{
	// Initialize mice information with a 1 in 4 chance of a frenzy
	m_NumMiceOut = 0;
	m_InAFrenzy = m_CatsAreAttacking = false;

	// Is there a frenzy or cat attack, or is it normal?
	int Temp = rand()%4;

	// Normal mouse deployment of 200 mice
	if (Temp != 0)
	{
		m_NumMiceIn = MICE_TO_DEPLOY;
		m_InAFrenzy = false;
	}
	// Frenzy deployment, 400 mice
	else if (Temp == 0)
	{
		m_NumMiceIn = FRENZY_MICE_TO_DEPLOY;
		m_InAFrenzy = true;
		m_Frenzy.Play();
	}
	// Cat Attack?
	if (Temp == 3)
	{
		m_CatsAreAttacking = true;
		m_CatAttack.Play();
		for (int i=0; i<TOTAL_CATS; i++)
			m_Cats[i]->SetXMove(1);
	}
	m_DeployMice = true;
}

void cGame::HandleMouseCollisions(cMouse *Mouse)
{
	// Check for collisions with holes
	if (Mouse->GetX() == RED_HOLE_X && Mouse->GetY() == RED_HOLE_Y)
	{
		// Increment red's score
		m_Cursors[0]->SetScore(m_Cursors[0]->GetScore() + 1);
		// Destroy Mouse
		m_MiceAlive[Mouse->GetID()] = false;
		m_NumMiceIn--;
		delete Mouse;
	}
	else if (Mouse->GetX() == GREEN_HOLE_X && Mouse->GetY() == GREEN_HOLE_Y)
	{
		// Increment green's score
		m_Cursors[2]->SetScore(m_Cursors[2]->GetScore() + 1);
		// Destroy Mouse
		m_MiceAlive[Mouse->GetID()] = false;
		m_NumMiceIn--;
		delete Mouse;
	}
	else if (Mouse->GetX() == BLUE_HOLE_X && Mouse->GetY() == BLUE_HOLE_Y)
	{
		// Increment blue's score
		m_Cursors[1]->SetScore(m_Cursors[1]->GetScore() + 1);
		// Destroy Mouse
		m_MiceAlive[Mouse->GetID()] = false;
		m_NumMiceIn--;
		delete Mouse;
	}
	else if (Mouse->GetX() == YELLOW_HOLE_X && Mouse->GetY() == YELLOW_HOLE_Y)
	{
		// Increment yellow's score
		m_Cursors[3]->SetScore(m_Cursors[3]->GetScore() + 1);
		// Destroy Mouse
		m_MiceAlive[Mouse->GetID()] = false;
		m_NumMiceIn--;
		delete Mouse;
	}
	else
	{
		// Check for collisions with arrows
		for (int i=0; i<4; i++)
		{
			for (int j=0; j<4; j++)
			{
				if (m_Cursors[i]->m_Arrows[j]->GetX() == Mouse->GetX() && 
					m_Cursors[i]->m_Arrows[j]->GetY() == Mouse->GetY() && 
					m_Cursors[i]->m_Arrows[j]->GetDead() == false)	// Arrow can't be dead
				{
					switch (m_Cursors[i]->m_Arrows[j]->GetType())
					{
					case LEFT:
						Mouse->SetXMove(-1);
						Mouse->SetYMove(0);
						break;
					case RIGHT:
						Mouse->SetXMove(1);
						Mouse->SetYMove(0);
						break;
					case UP:
						Mouse->SetXMove(0);
						Mouse->SetYMove(-1);
						break;
					case DOWN:
						Mouse->SetXMove(0);
						Mouse->SetYMove(1);
						break;
					default:
						break;
					}
				}
			}
		}
	}
	// If all the mice are gone, deploy more
	if (m_NumMiceIn <= 0)
		StartNewWave();
}

void cGame::HandleCatCollisions(cCat *Cat)
{
	for (int i=0; i<TOTAL_MICE; i++)
	{
		if (m_MiceAlive[i])
		{
			if (m_Tile.Collision(Cat->GetX(), Cat->GetY(), 32, 32,
				m_Mice[i]->GetX(), m_Mice[i]->GetY(), 32, 32))
			{
				m_MiceAlive[m_Mice[i]->GetID()] = false;
				m_NumMiceIn--;
				delete m_Mice[i];
				m_EatenMouse.Play();
			}
				
		}
	}
}

void cGame::HandleP1Input()
{
	// See if they want to move
	if (m_KB.IsKeyDown(VK_NUMPAD1) && !m_KB.GetLock(VK_NUMPAD1))
	{
		m_KB.SetLock(VK_NUMPAD1);
		m_Cursors[0]->SetXMove(-32);
	}
	
	if (m_KB.IsKeyDown(VK_NUMPAD3) && !m_KB.GetLock(VK_NUMPAD3))
	{
		m_KB.SetLock(VK_NUMPAD3);
		m_Cursors[0]->SetXMove(32);
	}

	if (m_KB.IsKeyDown(VK_NUMPAD5) && !m_KB.GetLock(VK_NUMPAD5))
	{
		m_KB.SetLock(VK_NUMPAD5);
		m_Cursors[0]->SetYMove(-32);
	}
	
	if (m_KB.IsKeyDown(VK_NUMPAD2) && !m_KB.GetLock(VK_NUMPAD2))
	{
		m_KB.SetLock(VK_NUMPAD2);
		m_Cursors[0]->SetYMove(32);
	}

	// See if they want to place an arrow
	if (m_KB.IsKeyDown(VK_LEFT) && !m_KB.GetLock(VK_LEFT))
	{
		m_KB.SetLock(VK_LEFT);
		m_Cursors[0]->PlaceArrow(LEFT) ? m_Klunk.Play() : m_BeepBeep.Play();
	}
	
	if (m_KB.IsKeyDown(VK_RIGHT) && !m_KB.GetLock(VK_RIGHT))
	{
		m_KB.SetLock(VK_RIGHT);
		m_Cursors[0]->PlaceArrow(RIGHT) ? m_Klunk.Play() : m_BeepBeep.Play();
	}

	if (m_KB.IsKeyDown(VK_UP) && !m_KB.GetLock(VK_UP))
	{
		m_KB.SetLock(VK_UP);
		m_Cursors[0]->PlaceArrow(UP) ? m_Klunk.Play() : m_BeepBeep.Play();
	}
	
	if (m_KB.IsKeyDown(VK_DOWN) && !m_KB.GetLock(VK_DOWN))
	{
		m_KB.SetLock(VK_DOWN);
		m_Cursors[0]->PlaceArrow(DOWN) ? m_Klunk.Play() : m_BeepBeep.Play();
	}
}

void cGame::HandleP2Input()
{
	// Update cursor position
	if (m_KB.IsKeyDown(0x47) && !m_KB.GetLock(0x47)) // G Key 
	{
		m_KB.SetLock(0x47);
		m_Cursors[1]->SetXMove(-32);
	}
	
	if (m_KB.IsKeyDown(0x4A) && !m_KB.GetLock(0x4A)) // J Key 
	{
		m_KB.SetLock(0x4A);
		m_Cursors[1]->SetXMove(32);
	}
	
	if (m_KB.IsKeyDown(0x48) && !m_KB.GetLock(0x48)) // H Key 
	{
		m_KB.SetLock(0x48);
		m_Cursors[1]->SetYMove(32);
	}
	
	if (m_KB.IsKeyDown(0x59) && !m_KB.GetLock(0x59)) // Y Key 
	{
		m_KB.SetLock(0x59);
		m_Cursors[1]->SetYMove(-32);
	}

	// See if they want to place an arrow 
	if (m_KB.IsKeyDown(0x41) && !m_KB.GetLock(0x41)) // A key 
	{
		m_KB.SetLock(0x41);
		m_Cursors[1]->PlaceArrow(LEFT);
	}
	
	if (m_KB.IsKeyDown(0x44) && !m_KB.GetLock(0x44)) // D key 
	{
		m_KB.SetLock(0x44);
		m_Cursors[1]->PlaceArrow(RIGHT);
	}

	if (m_KB.IsKeyDown(0x57) && !m_KB.GetLock(0x57)) // W key 
	{
		m_KB.SetLock(0x57);
		m_Cursors[1]->PlaceArrow(UP);
	}
	
	if (m_KB.IsKeyDown(0x53) && !m_KB.GetLock(0x53)) // S key 
	{
		m_KB.SetLock(0x53);
		m_Cursors[1]->PlaceArrow(DOWN);
	}
}

void cGame::DrawMenu()
{
	// Draw menu screen 
	m_MenuTex.Render(0, 0, NULL);

	// Draw menu selector arrow (Cheese) 
	switch (m_MenuChoice)
	{
	case SINGLE:
		m_Tile.DrawTile(0, 28, MENU_ARROW_X, SINGLE_CHOICE_Y);
		break;
	case MULTI:
		m_Tile.DrawTile(0, 28, MENU_ARROW_X, MULTI_CHOICE_Y);
		break;
	case INSTRUCT:
		m_Tile.DrawTile(0, 28, MENU_ARROW_X, INSTRUCT_CHOICE_Y);
		break;
	case EXIT:
		m_Tile.DrawTile(0, 28, MENU_ARROW_X, EXIT_CHOICE_Y);
		break;
	default:
		break;
	}
}

void cGame::DrawGame()
{
	// Draw tile map 
	DrawTileMap();

	// Draw mouse hunt texture in bottom right corner
	m_MouseHuntTex.Render(480, 416, NULL);
	
	// Draw arrows
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if (m_Cursors[i]->m_Arrows[j]->GetDead() == false)		
				m_Cursors[i]->m_Arrows[j]->DrawArrow();
		}
	}

	// Draw alive mice
	for (int n=0; n<TOTAL_MICE; n++)
	{
		if (m_MiceAlive[n])
			m_Mice[n]->Draw();
	}
	
	// Draw cats
	if (m_CatsAreAttacking)
	{
		for (int i=0; i<TOTAL_CATS; i++)
			m_Cats[i]->Draw();
	}

	// Draw cursors
	for (int i=0; i<4; i++)
		m_Cursors[i]->Draw();

	// DEBUG_AID: Draw '0' on the spaces that are occupied
	/*	for (int i=0; i<13; i++)
		{
			for (int j=0; j<13; j++)
			{
				if (Occupied[i][j])
					m_Tile.DrawTile(0, 48, ((i+1)*32), ((j+1)*32));
			}
		}	
	*/

	// Draw scores
	DrawScores();

	// Draw the multicolored L representing leader by the score of the current leader
	switch (m_CurrentLeader)
	{
	case 0:
		m_Tile.DrawTile(0, 38, 480, 0);
		break;
	case 1:
		m_Tile.DrawTile(0, 38, 480, 96);
		break;
	case 2:
		m_Tile.DrawTile(0, 38, 480, 192);
		break;
	case 3:
		m_Tile.DrawTile(0, 38, 480, 288);
		break;
	default:
		break;
	}

	// If there is a frenzy or a cat attack, draw the appropriate picture
	if (m_InAFrenzy)
		m_FrenzyTex.Render(496, 320);
	if (m_CatsAreAttacking)
		m_CatAttackTex.Render(480, 352);
}

void cGame::DrawScores()
{
	// Start font drawing
	if (m_Font.Begin())
	{
		// Print scores of all four players to screen after converting the ints to strings
		for (int i=0; i<4; i++)
		{
			char Temp[256];
			itoa(m_Cursors[i]->GetScore(), Temp, 10);
			m_Font.Print(Temp, 520, i*96, 0, 0, D3DCOLOR_RGBA(255,255,255,255));
		}
		
		// End font drawing
		m_Font.End();
	}
}

void cGame::DrawTileMap()
{
	for (int rows=0; rows<15; rows++)
	{
		for (int cols=0; cols<20; cols++)
		{
			m_Tile.DrawTile(0, TileMap[rows][cols], cols*32, rows*32);
		}
	}
}

void cGame::DrawWinText()
{
	// Convert winning number to a printable string
	char Winner[2];
	itoa(m_Winner+1, Winner, 10);

	// Print some text
	if (m_Font.Begin())
	{
		m_Font.Print("GAME OVER", 240, 0, 0, 0, D3DCOLOR_RGBA(255,255,255,255));
		m_Font.Print("Press Spacebar", 0, 443, 0, 0, D3DCOLOR_RGBA(255,255,255,255));
		m_Font.Print("Player", m_TextX, m_TextY, 0, 0, D3DCOLOR_RGBA(m_TextX,m_TextX,m_TextY,255));
		m_Font.Print(Winner, m_TextX+110, m_TextY, 0, 0, D3DCOLOR_RGBA(m_TextX,m_TextY,m_TextY,255));
		m_Font.Print("Wins!", m_TextX+150, m_TextY, 0, 0, D3DCOLOR_RGBA(m_TextY,m_TextX,m_TextY,255));
		
		// Stop printing
		m_Font.End();
	}
}

void cGame::DrawCountdown()
{
	if (m_CurrentNumber <= 0)
	{
		m_CountdownExpired = true;
		m_Tile.DrawTile(0, 58, 224, 192, 3.0f, 3.0f);
		m_Tile.DrawTile(0, 59, 320, 192, 3.0f, 3.0f);
	}
	else
		m_Tile.DrawTile(0, m_CurrentNumber+48, m_NumX, m_NumY, m_Scaler, m_Scaler);
}

void cGame::ShutdownObjects()
{
	// Delete cursors 
	for (int i=0; i<4; i++)
		delete m_Cursors[i];

	// Delete remaining mice
	for (int i=0; i<TOTAL_MICE; i++)
	{
		if (m_MiceAlive[i])
			delete m_Mice[i];
	}

	// Delete Cats
	for (int i=0; i<TOTAL_CATS; i++)
		delete m_Cats[i];
}

/////////////////////////////////////////////////
// Main Windows Loop
/////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR CmdLine, int nCmdShow)
{
	cGame Game;
	Game.BaseRun();
}

/////////////////////////////////////////////////
// Game Data
/////////////////////////////////////////////////

// Game TileMap 
short TileMap[15][20] =
{
	//	  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15  16  17  18  19  20 
	{ 29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,  4, 63, 63, 63, 63 }, // 1 
	{ 29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,29, 63, 63, 63, 63, 63 }, // 2 
	{ 29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,29, 63, 63, 63, 63, 63 }, // 3 
	{ 29, 1, 1,24, 1, 1, 1, 1, 1, 1, 1,26, 1, 1,29,  5, 63, 63, 63, 63 }, // 4  
	{ 29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,29, 63, 63, 63, 63, 63 }, // 5  
	{ 29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,29, 63, 63, 63, 63, 63 }, // 6  
	{ 29, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1,29,  6, 63, 63, 63, 63 }, // 7  
	{ 29, 1, 1, 1, 1, 1, 2, 3, 2, 1, 1, 1, 1, 1,29, 63, 63, 63, 63, 63 }, // 8  
	{ 29, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1, 1,29, 63, 63, 63, 63, 63 }, // 9  
	{ 29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,29,  7, 63, 63, 63, 63 }, // 10 
	{ 29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,29, 63, 63, 63, 63, 63 }, // 11 
	{ 29, 1, 1,27, 1, 1, 1, 1, 1, 1, 1,25, 1, 1,29, 63, 63, 63, 63, 63 }, // 12 
	{ 29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,29, 63, 63, 63, 63, 63 }, // 13 
	{ 29, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,29, 63, 63, 63, 63, 63 }, // 14 
	{ 29,29,29,29,29,29,29,29,29,29,29,29,29,29,29, 63, 63, 63, 63, 63 }  // 15 
};
	
// Array of the playing area with a int value for each square
// 1 = occupied, 0 = empty
int Occupied[13][13] = 
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

// Array of "Hot Spot" locations used in AI
// "Hot Spots" are areas of High traffic that mice frequently pass through, 
// that have straight line paths to holes
// [3]players [4] entries per player [2] numbers per coordinate
int HotSpots[3][4][2] =
{
	// Player one (Blue)
	{
		{ 224, 352 },
		{ 352, 224 },
		{ 352, 416 },
		{ 416, 352 }
	},
	// Player two (Green)
	{
		{ 224, 96  },
		{ 352, 32  },
		{ 352, 224 },
		{ 416, 96  }
	},
	// Player three (Yellow)
	{
		{  32, 352 },
		{  96, 224 },
		{  96, 416 }, 
		{ 224, 352 }
	}
};

