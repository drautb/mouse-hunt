/////////////////////////////////////////////////
// Object.h
// Header File For Object Class
// Developed By: Ben Draut
// Last Updated: 5/19/06
/////////////////////////////////////////////////

#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

// Enumeration for arrow types 
enum ArrowTypes
{
	NO_DIRECTION = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

enum MouseDirection
{
	MOUSE_NO_DIRECTION = 0,
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_UP,
	MOUSE_DOWN
};

// Class prototypes 
class cCursor;
class cArrow;
class cMouse;

/////////////////////////////////////////////////
// cCursor Class
/////////////////////////////////////////////////
class cCursor
{
private:
	// ID Number 
	int m_ID;

	// Parent Tile object 
	cTile *m_Tile;

	// Other Member Variables 
	int m_X, m_Y;
	int m_XMove, m_YMove;
	int m_Frame;
	int m_CurrentArrow;  
	int m_Score;

	// Values concerning AI
	int m_Target[2];    // XY coordinates of target position
	int m_Hole[2];      // XY coordinates of players mouse hole
	int m_ActionChoice; // Integer representing current AI Action

public:
	// Constructors/Destructor
	cCursor(cTile *Tile, int ID, int Frame = 0, int X = 0, int Y = 0, int XMove = 0, int YMove = 0);
	cCursor();
	~cCursor();

	// Functions 
	void Update();
	void Draw();
	bool PlaceArrow(ArrowTypes Type);

	// Accessor methods 
	cTile *GetTile()const{return m_Tile;}
	int GetID()const{return m_ID;}
	int GetX()const{return m_X;}
	int GetY()const{return m_Y;}
	int GetXMove()const{return m_XMove;}
	int GetYMove()const{return m_YMove;}
	int GetFrame()const{return m_Frame;}
	int GetCurrentArrow()const{return m_CurrentArrow;}
	int GetScore()const{return m_Score;}
	int GetTargetX()const{return m_Target[0];}
	int GetTargetY()const{return m_Target[1];}
	int GetHoleX()const{return m_Hole[0];}
	int GetHoleY()const{return m_Hole[1];}
	int GetActionChoice()const{return m_ActionChoice;}
	
	void SetTile(cTile *NewTile){m_Tile = NewTile;}
	void SetID(int NewID){m_ID = NewID;}
	void SetX(int NewX){m_X = NewX;}
	void SetY(int NewY){m_Y = NewY;}
	void SetXMove(int NewXMove){m_XMove = NewXMove;}
	void SetYMove(int NewYMove){m_YMove = NewYMove;}
	void SetFrame(int NewFrame){m_Frame = NewFrame;}
	void SetCurrentArrow(int CurrentArrow){m_CurrentArrow = CurrentArrow;}
	void SetScore(int NewScore){m_Score = NewScore;}
	void SetTargetX(int NewTargetX){m_Target[0] = NewTargetX;}
	void SetTargetY(int NewTargetY){m_Target[1] = NewTargetY;}
	void SetHoleX(int NewHoleX){m_Hole[0] = NewHoleX;}
	void SetHoleY(int NewHoleY){m_Hole[1] = NewHoleY;}
	void SetActionChoice(int NewAction){m_ActionChoice = NewAction;}

	// Vector of four arrows that the cursor can drop 
	cArrow *m_Arrows[4];
};

/////////////////////////////////////////////////
// cArrow Class
/////////////////////////////////////////////////
class cArrow
{
private:
	// Parent cursor object 
	cCursor *m_ParentCursor;

	// Position, timer, and frame 
	int m_X, m_Y;
	int m_Frame;
	int m_Timer;
	bool m_Dead;

	// Type of arrow 
	ArrowTypes m_Type;

public:
	// Constructor/Destructor 
	cArrow();
	cArrow(cCursor *ParentCursor, ArrowTypes Type, bool Dead);
	~cArrow(){};

	// Functions 
	void DrawArrow();

	// Accessor methods 
	ArrowTypes GetType()const{return m_Type;}
	int GetX()const{return m_X;}
	int GetY()const{return m_Y;}
	int GetFrame()const{return m_Frame;}
	int GetTimer()const{return m_Timer;}
	bool GetDead()const{return m_Dead;}

	void SetX(int NewX){m_X = NewX;}
	void SetY(int NewY){m_Y = NewY;}
	void SetType(ArrowTypes NewType){m_Type = NewType;}
	void SetFrame(int NewFrame){m_Frame = NewFrame;}
	void SetTimer(int NewTimer){m_Timer = NewTimer;}
	void SetDead(bool Dead){m_Dead = Dead;}
};

/////////////////////////////////////////////////
// cMouse Class 
/////////////////////////////////////////////////
class cMouse
{
private:
	// Mouse ID Number
	int m_ID;

	// Parent Tile object 
	cTile *m_Tile;

	// Position, animation, etc.
	int m_X, m_Y, m_XMove, m_YMove;
	int m_FirstFrame, m_Frame, m_FrameLimit;
	int m_AnimTimer, m_AnimDelay;

	// Heading 
	MouseDirection m_Heading;

public:
	// Constructors/Destructor 
	cMouse(cTile *Tile, int ID, int X=0, int Y=0, int XMove=0, int YMove=0, int FirstFrame=0,
		   int Frame=0, int FrameLimit=0, int AnimTimer=0, 
		   int AnimDelay=0, MouseDirection Heading=MOUSE_NO_DIRECTION);
	cMouse();
	~cMouse(){}

	// Functions
	void Update();
	void Draw();
	void HandleWallCollisions();

	// Accessor Methods 
	cTile *GetTile()const{return m_Tile;}
	int GetID()const{return m_ID;}
	int GetX()const{return m_X;}
	int GetY()const{return m_Y;}
	int GetXMove()const{return m_XMove;}
	int GetYMove()const{return m_YMove;}
	int GetFirstFrame()const{return m_FirstFrame;}
	int GetFrame()const{return m_Frame;}
	int GetFrameLimit()const{return m_FrameLimit;}
	int GetAnimTimer()const{return m_AnimTimer;}
	int GetAnimDelay()const{return m_AnimDelay;}

	void SetTile(cTile *Tile){m_Tile = Tile;}
	void SetID(int ID){m_ID = ID;}
	void SetX(int NewX){m_X = NewX;}
	void SetY(int NewY){m_Y = NewY;}
	void SetXMove(int XMove){m_XMove = XMove;}
	void SetYMove(int YMove){m_YMove = YMove;}
	void SetFirstFrame(int FirstFrame){m_FirstFrame = FirstFrame;}
	void SetFrame(int Frame){m_Frame = Frame;}
	void SetFrameLimit(int Limit){m_FrameLimit = Limit;}
	void SetAnimTimer(int Timer){m_AnimTimer = Timer;}
	void SetAnimDelay(int Delay){m_AnimDelay = Delay;}
};

/////////////////////////////////////////////////
// cCat class
/////////////////////////////////////////////////
class cCat
{
private:
	// Parent tile object
	cTile *m_Tile;

	// Data members
	int m_X, m_Y, m_XMove, m_YMove;
	int m_FirstFrame, m_Frame, m_FrameLimit;
	int m_AnimTimer, m_AnimDelay;

public:
	// Constructor/Destructor
	cCat(cTile *Tile, int X=0, int Y=0, int XMove=0, int YMove=0, int FirstFrame=0, int Frame=0, int FrameLimit=0,
		 int AnimTimer=0, int AnimDelay=0);
	~cCat(){}

	// Functions
	void Update();
	void Draw();

	// Accessor methods
	cTile *GetTile()const{return m_Tile;}
	int GetX()const{return m_X;}
	int GetY()const{return m_Y;}
	int GetXMove()const{return m_XMove;}
	int GetYMove()const{return m_YMove;}
	int GetFirstFrame()const{return m_FirstFrame;}
	int GetFrame()const{return m_Frame;}
	int GetFrameLimit()const{return m_FrameLimit;}
	int GetAnimTimer()const{return m_AnimTimer;}
	int GetAnimDelay()const{return m_AnimDelay;}

	void SetTile(cTile *Tile){m_Tile = Tile;}
	void SetX(int NewX){m_X = NewX;}
	void SetY(int NewY){m_Y = NewY;}
	void SetXMove(int XMove){m_XMove = XMove;}
	void SetYMove(int YMove){m_YMove = YMove;}
	void SetFirstFrame(int FirstFrame){m_FirstFrame = FirstFrame;}
	void SetFrame(int Frame){m_Frame = Frame;}
	void SetFrameLimit(int Limit){m_FrameLimit = Limit;}
	void SetAnimTimer(int Timer){m_AnimTimer = Timer;}
	void SetAnimDelay(int Delay){m_AnimDelay = Delay;}
};

#endif



















