#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")

#pragma once

#include <ddraw.h>

class ImageData;

// ΩÃ±€≈Ê ∆–≈œ
class MyDDraw final
{
public:
	MyDDraw();
	//MyDDraw(HWND hWnd);
	~MyDDraw();

	static MyDDraw* GetInstance();

	BOOL InitDDraw(HWND hWnd);
	VOID OnUpdateWindowPos();
	BOOL CreateBackBuffer(DWORD dwWidth, DWORD dwHeight);

	BOOL BeginDraw();
	BOOL EndDraw();
	BOOL LockBackBuffer();
	BOOL UnlockBackBuffer();
	VOID DisplayScene();

	VOID DrawSomething(int* posX, int* posY, DWORD dwObjWidth, DWORD dwObjHeight, DWORD dwObjColor);
	VOID DrawSomething(int* posX, int* posY, DWORD dwObjWidth, DWORD dwObjHeight, char* pRawImage);
	VOID DrawImageData(OUT int* posX, OUT int* posY, const ImageData& imgData);
	BOOL ClipArea(int* outPosX, int* outPosY, const DWORD dwObjWidth, const DWORD dwObjHeight);
	VOID ClearBackBuffer(DWORD dwSquarePosY);

	inline DWORD GetSurfaceWidth()  const { return m_dwWidth; }
	inline DWORD GetSurfaceHeight() const { return m_dwHeight; }

	// For FPS
	VOID UpdateInfoTxt();
	BOOL CheckFPS();
	VOID DrawInfo(HDC hDC);
	VOID WriteText(const WCHAR* wchTxt, DWORD dwLen, int x, int y, DWORD dwColor, HDC hDC);
	BOOL GetFontSize(int* piOutWidth, int* piOutHeight, const WCHAR* wchString, DWORD dwStrLen, HDC hDC);

	BOOL BeginGDI(HDC* pOutDC);
	VOID EndGDI(HDC hDC);
	BOOL SetColorKey(DWORD dwColorKey);

private:
	static MyDDraw* m_pDDraw;

	LPDIRECTDRAW7		 m_pDD7;		/* DDRAW7 instance				*/
	LPDIRECTDRAWSURFACE7 m_pDDPrimary;	/* Primary surface				*/
	LPDIRECTDRAWSURFACE7 m_pDDBack;		/* Secondary surface(offscreen) */
	LPDIRECTDRAWCLIPPER  m_pDDClipper;	/* clipper                      */

	char* m_pLockedBackBuffer;
	DWORD m_dwLockedBackBufferPitch;
	DWORD m_dwWidth;
	DWORD m_dwHeight;

	RECT m_rcWindow;
	HWND m_hWnd;

	DWORD	m_dwFrameCount;
	DWORD	m_dwFPS;
	ULONGLONG m_LastDrawTick;
	WCHAR	m_wchInfoTxt[64];
	DWORD	m_dwInfoTxtLen;
};
