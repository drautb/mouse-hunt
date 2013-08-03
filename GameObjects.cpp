/////////////////////////////////////////////////
// Object.cpp
// Source File for Object class
// Developed By: Ben Draut
// Last Updated: 5/19/06
/////////////////////////////////////////////////

// Include everything 
#include "Global_Engine.h"

/////////////////////////////////////////////////
// cCursor Function Definitions 
/////////////////////////////////////////////////
cCursor::cCursor(cTile *Tile, int ID, int Frame, int X, int Y, int XMove, int YMove)
{
	// Set default values 
	m_Tile = Tile;
	m_ID = ID;
	m_Frame = Frame;
	m_X = X;
	m_Y = Y;
	m_XMove = XMove;
	m_YMove = YMove;
	m_Score = 0;
	m_CurrentArrow = 0;
	for (int i=0; i<4; i++)
	{
		m_Arrows[i] = new cArrow(this, NO_DIRECTION, true);
	}

	// Init a dummy target
	m_Target[0] = 0;
	m_Target[1] = 0;
}

cCursor::cCursor()
{
	// Set default values 
	m_Score = 0;
	m_Frame = 0;
	m_X = m_Y = m_XMove = m_YMove = 0;
	m_CurrentArrow = 0;
	m_Tile = NULL;
}

cCursor::~cCursor()
{
	// Delete leftover arrows 
	for (int i=0; i<4; i++)
		delete m_Arrows[i];
}

void cCursor::Update()
{
	// Update animation and position 
	m_X += m_XMove;
	m_Y += m_YMove;
}

void cCursor::Draw()
{
	// Draw the object 
	m_Tile->DrawTile(0, m_Frame, m_X, m_Y);
}

bool cCursor::PlaceArrow(ArrowTypes Type)
{
	// Replace the oldest arrow with a new one, if the space is not occupied with an arrow, or mouse hole
	int X, Y;
	X = (m_X/32)-1;
	Y = (m_Y/32)-1;
	if (Occupied[X][Y] == FALSE)
	{
		Occupied[X][Y] = TRUE; // Space is now occupied
		
		// Deleted arrows space is open if the arrow is not already dead. (check docs for why)
		if (!m_Arrows[m_CurrentArrow]->GetDead())
		{
			X = (m_Arrows[m_CurrentArrow]->GetX()/32)-1;
			Y = (m_Arrows[m_CurrentArrow]->GetY()/32)-1;
			if (X >= 0 && X <= 13 && Y >= 0 && Y <= 13)
				Occupied[X][Y] = FALSE;
		}

		delete m_Arrows[m_CurrentArrow];
		m_Arrows[m_CurrentArrow] = new cArrow(this, Type, false);
		m_CurrentArrow++;
		if (m_CurrentArrow > 3)
			m_CurrentArrow = 0;
		return true;
	}
	else 
		return false;
}

/////////////////////////////////////////////////
// cArrow definitions 
/////////////////////////////////////////////////
cArrow::cArrow()
{
	m_X = m_Y = -32;
	m_Frame = 0;
	m_Type = NO_DIRECTION;
	m_Timer = 0;
	m_Dead = true;
}

cArrow::cArrow(cCursor *ParentCursor, ArrowTypes Type, bool Dead)
{
	m_ParentCursor = ParentCursor;
	m_Type = Type;
	m_X = m_ParentCursor->GetX();
	m_Y = m_ParentCursor->GetY();
	m_Timer = 0;
	m_Dead = Dead;

	// Set fram based on type and who its from 
	int FrameStart = 0;
	int FrameOffset = 0;
	
	switch (m_Type)
	{
	case LEFT:
		FrameStart = 11;
		break;
	case RIGHT:
		FrameStart = 9;
		break;
	case UP:
		FrameStart = 8;
		break;
	case DOWN:
		FrameStart = 10;
		break;
	default:
		m_Frame = 63;
		return;
		break;
	}	

	switch (m_ParentCursor->GetID())
	{
	case 0:
		FrameOffset = 4;
		break;
	case 1:
		FrameOffset = 8;
		break;
	case 2:
		FrameOffset = 0;
		break;
	case 3:
		FrameOffset = 12;
		break;
	default:
		break;
	}	

	m_Frame = FrameStart + FrameOffset;
}

void cArrow::DrawArrow()
{
	m_ParentCursor->GetTile()->DrawTile(0, m_Frame, m_X, m_Y);
}

/////////////////////////////////////////////////
// cMouse Definitions 
/////////////////////////////////////////////////
cMouse::cMouse(cTile *Tile, int ID, int X, int Y, int XMove, int YMove, int FirstFrame, int Frame, int FrameLimit,
			   int AnimTimer, int AnimDelay, MouseDirection Heading)
{
	// Set member data //
	m_Tile = Tile;
	m_ID = ID;
	m_X = X;
	m_Y = Y;
	m_XMove = XMove;
	m_YMove = YMove;
	m_FirstFrame = FirstFrame;
	m_Frame = Frame;
	m_FrameLimit = FrameLimit;
	m_AnimTimer = AnimTimer;
	m_AnimDelay = AnimDelay;
	m_Heading = Heading;
}

cMouse::cMouse()
{
	// Set default member data 
	m_Tile = NULL;
	m_ID = 0;
	m_X = 0;
	m_Y = 0;
	m_XMove = 0;
	m_YMove = 0;
	m_FirstFrame = 0;
	m_Frame = 0;
	m_FrameLimit = 0;
	m_AnimTimer = 0;
	m_AnimDelay = 0;
	m_Heading = MOUSE_NO_DIRECTION;
}

void cMouse::Update()
{
	// Check for collisions, arrows, walls, etc. before moving
	if (m_X % 32 == 0 && m_Y % 32 == 0)
	{
		HandleWallCollisions();
	}

	// Update position values, animation timers, frame, etc.
	m_X += m_XMove;
	m_Y += m_YMove;
	
	m_AnimTimer += 1;
	if (m_AnimTimer > m_AnimDelay)
	{
		m_AnimTimer = 0;
		m_Frame++;
		if (m_Frame > m_FrameLimit)
			m_Frame = m_FirstFrame;
	}
}

void cMouse::Draw()
{
	// Draw the mouse at its location with the proper frame 
	m_Tile->DrawTile(0, m_Frame, m_X, m_Y);
}

void cMouse::HandleWallCollisions()
{
	// Left wall
	if (m_X-1 < 32 && m_XMove == -1)
	{
		if (m_Y-1 > 32)
		{
			m_XMove = 0;
			m_YMove = -1;
		}
		else if (m_Y+1 < 416)
		{
			m_XMove = 0;
			m_YMove = 1;
		}
		else
		{
			m_XMove = 1;
			m_YMove = 0;
		}
	}
	// Right wall
	else if (m_X+1 > 416 && m_XMove == 1)
	{
		if (m_Y+1 < 416)
		{
			m_XMove = 0;
			m_YMove = 1;
		}
		else if (m_Y-1 > 32)
		{
			m_XMove = 0;
			m_YMove = -1;
		}
		else
		{
			m_XMove = -1;
			m_YMove = 0;
		}
	}	
	// Top wall
	else if (m_Y-1 < 32 && m_YMove == -1)
	{
		if (m_X+1 < 416)
		{
			m_XMove = 1;
			m_YMove = 0;
		}
		else if (m_X-1 > 32)
		{
			m_XMove = -1;
			m_YMove = 0;
		}
		else
		{
			m_XMove = 0;
			m_YMove = 1;	
		}
	}	
	// Bottom wall
	else if (m_Y+1 > 416 && m_YMove == 1)
	{
		if (m_X-1 > 32)
		{
			m_XMove = -1;
			m_YMove = 0;
		}
		else if (m_X+1 < 416)
		{
			m_XMove = 1;
			m_YMove = 0;
		}
		else
		{
			m_XMove = 0;
			m_YMove = -1;	
		}
	}	
}

/////////////////////////////////////////////////
// cCat class stuff
/////////////////////////////////////////////////
cCat::cCat(cTile *Tile, int X, int Y, int XMove, int YMove, int FirstFrame, int Frame, int FrameLimit,
		   int AnimTimer, int AnimDelay)
{
	// Initialize members
	m_Tile = Tile;
	m_X = X;
	m_Y = Y;
	m_XMove = XMove;
	m_YMove = YMove;
	m_FirstFrame = FirstFrame;
	m_Frame = Frame;
	m_FrameLimit = FrameLimit;
	m_AnimTimer = AnimTimer;
	m_AnimDelay = AnimDelay;
}

void cCat::Update()
{
	// Update position
	m_X += m_XMove;
	m_Y += m_YMove;

	// Update animation
	++m_AnimTimer;
	if (m_AnimTimer > m_AnimDelay)
	{
		m_AnimTimer = 0;
		m_Frame++;
		if (m_Frame > m_FrameLimit)
			m_Frame = m_FirstFrame;
	}
}

void cCat::Draw()
{
	// Draw the cat
	m_Tile->DrawTile(0, m_Frame, m_X, m_Y);
}













