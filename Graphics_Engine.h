/////////////////////////////////////////////////
// Graphics_Engine.h
// Graphics Aspect of Game Engine Header File
// Developed By: Ben Draut
/////////////////////////////////////////////////

#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

/////////////////////////////////////////////////
// Main Graphics Class
/////////////////////////////////////////////////
class cGraphics
{
private:
	// Pointers to D3D Object and Device
	LPDIRECT3D9			m_D3D;
	LPDIRECT3DDEVICE9	m_D3DDevice;
	// Sprite Object 
	LPD3DXSPRITE		m_D3DXSprite;

public:
	// Constructor/Destructor
	cGraphics(){}
	~cGraphics(){}

	// Functions 
	bool GraphicsInit(HWND hWnd, bool Fullscreen, WORD Width, WORD Height);
	bool GraphicsEnd();
	bool BeginScene();
	void EndScene();
	void ClearScreen();
	void Present();

	// Accessor Methods 
	LPDIRECT3D9			GetD3D()const{return m_D3D;}
	LPDIRECT3DDEVICE9	GetD3DDevice()const{return m_D3DDevice;}
	LPD3DXSPRITE		GetSprite()const{return m_D3DXSprite;}
};

/////////////////////////////////////////////////
// cTexture Class
/////////////////////////////////////////////////
class cTexture
{
private:
	// Texture Object 		
	LPDIRECT3DTEXTURE9	m_Texture;
	// Pointer to parent Graphics object
	cGraphics *m_Graphics;

public:
	// Constructor/Destructor 
	cTexture(){}
	~cTexture(){}

	// Functions 
	bool Load(cGraphics *m_Graphics, char *filename, D3DCOLOR color);
	bool Render(short x, short y, RECT *SrcRect=NULL, float ScaleX=1.0f, float ScaleY=1.0f);
	bool Free();

	// Accessor Methods 
	LPDIRECT3DTEXTURE9 GetTexture()const{return m_Texture;}
};

/////////////////////////////////////////////////
// cTile Class
/////////////////////////////////////////////////
class cTile
{
private:
	// Parent Graphics
	cGraphics *m_Graphics;
	// Arrays of textures, widths, heights, etc.
	cTexture *m_Textures;
	short m_NumTextures;

	short *m_Widths;
	short *m_Heights;
	short *m_Cols;

	// Rectangle used for drawing, etc. 
	RECT Rect;

public:
	// Constructor/Destructor 
	cTile(){}
	~cTile(){}

	// Functions 
	bool Create(cGraphics *Graphics, short NumTextures);
	void Free();
	bool Load(short TexNum, char *Filename, short TileWidth = 32, short TileHeight = 32,
			  short Cols = 8, D3DCOLOR Alpha = D3DCOLOR_RGBA(0,0,0,255));
	void FreeTexture(short Texture=-1);
	void DrawTile(short TexNum, short TileNum, short DestX, short DestY, 
				  float ScaleX=1.0f, float ScaleY=1.0f);
	int Collision(long Object1X, long Object1Y, long Object1Width, long Object1Height,
				  long Object2X, long Object2Y, long Object2Width, long Object2Height);
	
	// Accessor Methods
	short GetWidth(short TexNum)const{return m_Widths[TexNum];}
	short GetHeight(short TexNum)const{return m_Heights[TexNum];}
};

/////////////////////////////////////////////////
// cFont Class
/////////////////////////////////////////////////
class cFont
{
private:
	// Font object
	ID3DXFont *m_Font;

public:
	// Constructor/Destructor
	cFont(){}
	~cFont(){}

	// Functions
	bool Create(cGraphics *Graphics, char *FontName="Times New Roman", 
				int HeightInPixels=32, bool Bold=false, bool Italic=false);
	void Free();
	bool Begin();
	void End();
	void Print(char *Text, int XPos, int YPos, int Width=0, int Height=0, D3DCOLOR TextColor=0xFFFFFFFF);

	// Accessor Method
	ID3DXFont *GetFont()const{return m_Font;}
};

#endif