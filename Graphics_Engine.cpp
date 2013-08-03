/////////////////////////////////////////////////
// Graphics_Engine.cpp
// Graphics Aspect of Game Engine Source File
// Developed By: Ben Draut
/////////////////////////////////////////////////

// Include Everything
#include "Global_Engine.h"

/////////////////////////////////////////////////
// cGraphics Stuff
/////////////////////////////////////////////////
bool cGraphics::GraphicsInit(HWND hWnd, bool Fullscreen, WORD Width, WORD Height)
{
	m_D3D = NULL;
	m_D3DDevice = NULL;

	// Create Direct3D Object 
	m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_D3D)
		return false;

	// Set Direct3D Presentation Parameters 
	D3DPRESENT_PARAMETERS D3Dpp;
	ZeroMemory(&D3Dpp, sizeof(D3DPRESENT_PARAMETERS));
	
	if (Fullscreen)
		D3Dpp.Windowed = FALSE;
	else
	{
		D3Dpp.Windowed = TRUE;
		D3Dpp.EnableAutoDepthStencil = TRUE;
		D3Dpp.AutoDepthStencilFormat = D3DFMT_D16;
	} 
	
	D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	D3Dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	D3Dpp.BackBufferCount = 1;
	D3Dpp.BackBufferWidth = Width;
	D3Dpp.BackBufferHeight = Height;
	D3Dpp.hDeviceWindow = hWnd;

	// Create Direct3D Device
	HRESULT result = m_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					    &D3Dpp, &m_D3DDevice);
	if (result != D3D_OK)
	{
		if (result == D3DERR_INVALIDCALL)
			Error(false, "Error creating Direct3D device, invalid function call.");
		if (result == D3DERR_NOTAVAILABLE)
			Error(false, "Error creating Direct3D device, device does not support call.");
		if (result == D3DERR_OUTOFVIDEOMEMORY)
			Error(false, "Error creating Direct3D device, device out of video memory.");

		return false;
	}

	// Create Sprite Object 
	result = D3DXCreateSprite(m_D3DDevice, &m_D3DXSprite);
	if (result != D3D_OK)
	{
		if (result == D3DERR_INVALIDCALL)
			Error(false, "Error creating D3DXSprite object, invalid call");
		if (result == E_OUTOFMEMORY)
			Error(false, "Error creating D3DXSprite object, not enough memory.");

		return false;
	}

	return true;
}

bool cGraphics::GraphicsEnd()
{
	// Release Sprite Object 
	ReleaseCOM(m_D3DXSprite);
	// Release Direct3D Object and Device 
	ReleaseCOM(m_D3DDevice);
	ReleaseCOM(m_D3D);

	return true;
}

bool cGraphics::BeginScene()
{
	// Begin The Scene to Start Rendering 
	if (!m_D3DDevice->BeginScene())
		return false;

	return true;
}

void cGraphics::EndScene()
{
	// End Scene to Stop Rendering 
	m_D3DDevice->EndScene();	
}

void  cGraphics::ClearScreen()
{
	// Clear The Display 
	m_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,0,0,255), 1.0f, 0);
}

void cGraphics::Present()
{
	// Flip the Backbuffer
	m_D3DDevice->Present(NULL, NULL, NULL, NULL);
}

/////////////////////////////////////////////////
// cTexture Stuff 
/////////////////////////////////////////////////
bool cTexture::Load(cGraphics *Graphics, char *filename, D3DCOLOR color)
{
	// Init New graphics object 
	m_Graphics = Graphics;

	// Get image info 
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename, &info);
	if (result != D3D_OK)
		Error(false, "Error loading texture, could not get image info, invalid function call");

	// Load the Texture
	result = D3DXCreateTextureFromFileEx(m_Graphics->GetD3DDevice(), filename, info.Width, 
												 info.Height, 1, D3DPOOL_DEFAULT, D3DFMT_UNKNOWN, 
												 D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, color,
												 &info, NULL, &m_Texture);
	if (result != D3D_OK)
	{
		switch (result)
		{
		case D3DERR_NOTAVAILABLE:
			Error(false, "Error loading texture, device does not support technique.");
		case D3DERR_OUTOFVIDEOMEMORY:
			Error(false, "Error loading texture, not enough video memory.");
		case D3DERR_INVALIDCALL:
			Error(false, "Error loading texture, invalid function call.");
		case D3DXERR_INVALIDDATA:
			Error(false, "Error loading texture, invalid data.");
		case E_OUTOFMEMORY:
			Error(false, "Error loading texture, could not allocate sufficient memory.");
		default:
			Error(false, "Error loading texture.");
		}
		return false;
	}

	return true;
}

bool cTexture::Render(short x, short y, RECT *SrcRect, float ScaleX, float ScaleY)
{
	// Blit the image 
	HRESULT result = m_Graphics->GetSprite()->Draw(m_Texture, SrcRect, &D3DXVECTOR2(ScaleX, ScaleY), NULL, NULL, 
												   &D3DXVECTOR2((float)x, (float)y), 0xFFFFFFFF);
	if (result != D3D_OK)
	{
		Error(false, "Error drawing texture, invalid function call.");
		return false;
	}

	return true;
}

bool cTexture::Free()
{
	// Release the texture 
	ReleaseCOM(m_Texture);

	return true;
}

/////////////////////////////////////////////////
// cTile Stuff
/////////////////////////////////////////////////
bool cTile::Create(cGraphics *Graphics, short NumTextures)
{
	// Allocate Memory/Init Variables
	m_Graphics = Graphics;
	m_NumTextures = NumTextures;
	m_Textures = new cTexture[m_NumTextures];
	m_Widths = new short[m_NumTextures];
	m_Heights = new short[m_NumTextures];
	m_Cols = new short[m_NumTextures];

	Rect.top = Rect.left = Rect.bottom = Rect.right = 0;

	// Error Handling
	if (m_Graphics == NULL)
		return false;
	if (m_Textures == NULL)
		return false;

	return true;
}

void cTile::Free()
{
	m_Graphics = NULL;

	// Free all textures, and delete arrays
	for (short i=0; i<m_NumTextures; i++)
	{
		m_Textures[i].Free();
	}
	delete [] m_Textures;
	delete [] m_Widths;
	delete [] m_Heights;
	delete [] m_Cols;
	m_Textures = NULL;
	m_Widths = m_Heights = m_Cols = NULL;
	m_NumTextures = 0;
}

bool cTile::Load(short TexNum, char *Filename, short TileWidth, short TileHeight,
				 short Cols, D3DCOLOR Alpha)
{
	// Load the texture, and init the variables 
	if (!m_Textures[TexNum].Load(m_Graphics, Filename, Alpha))
		return false;

	m_Widths[TexNum] = TileWidth;
	m_Heights[TexNum] = TileHeight;
	m_Cols[TexNum] = Cols;

	return true;
}

void cTile::FreeTexture(short Texture)
{
	m_Textures[Texture].Free();
}

void cTile::DrawTile(short TexNum, short TileNum, short DestX, short DestY,
					 float ScaleX, float ScaleY)
{
	// Init source rectangle
	Rect.left = (TileNum % m_Cols[TexNum])*m_Widths[TexNum];
	Rect.top = (TileNum / m_Cols[TexNum])*m_Heights[TexNum];
	Rect.right = Rect.left + m_Widths[TexNum];
	Rect.bottom = Rect.top + m_Heights[TexNum];
	// Draw Tile //
	m_Textures[TexNum].Render(DestX, DestY, &Rect, ScaleX, ScaleY);
}

int cTile::Collision(long Object1X, long Object1Y, long Object1Width, long Object1Height,
						 long Object2X, long Object2Y, long Object2Width, long Object2Height)			   
{
	RECT rect1;
	RECT rect2;
	
	rect1.top = Object1Y;
	rect1.left = Object1X;
	rect1.right = Object1X += Object1Width;
	rect1.bottom = Object1Y += Object1Height;

	rect2.top = Object2Y;
	rect2.left = Object2X;
	rect2.right = Object2X += Object2Width;
	rect2.bottom = Object2Y += Object2Height;
	
	RECT dest;
    return IntersectRect(&dest, &rect1, &rect2);
}

/////////////////////////////////////////////////
// cFont Stuff
/////////////////////////////////////////////////
bool cFont::Create(cGraphics *Graphics, char *FontName, int HeightInPixels, bool Bold, bool Italic)
{
	// Setup font characteristics
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	
	// Name and height
	strcpy(lf.lfFaceName, FontName);
	lf.lfHeight = -HeightInPixels;
	
	// Bold or not?
	if (Bold)
		lf.lfWeight = 700;
	else
		lf.lfWeight = 0;

	// Italic or not?
	if (Italic)
		lf.lfItalic = 1;
	else 
		lf.lfItalic = 0;

	// Create the new font object
	if (FAILED(D3DXCreateFontIndirect(Graphics->GetD3DDevice(), &lf, &m_Font)))
		return false;

	return true;
}

void cFont::Free()
{
	ReleaseCOM(m_Font);
}

bool cFont::Begin()
{
	HRESULT Result = m_Font->Begin();
	if (Result == D3D_OK)
		return true;

	return false;
}

void cFont::End()
{
	m_Font->End();
}

void cFont::Print(char *Text, int XPos, int YPos, int Width, int Height, D3DCOLOR TextColor)
{
	if(!Width)
		Width = 65535;
	if(!Height)
		Height = 65536;

	RECT Rect;
	Rect.left   = XPos;
	Rect.top    = YPos;
	Rect.right  = Rect.left + Width;
	Rect.bottom = Rect.top + Height;
	m_Font->DrawText(Text, -1, &Rect, 0, TextColor);
}








