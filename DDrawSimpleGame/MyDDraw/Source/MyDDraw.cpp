#include "../../pch/stdafx.h"
#include <cassert>

#include "../Header/MyDDraw.h"
#include "../../Util/Header/ImageData.h"
#include "../../Game/ScoreManager/Header/ScoreManager.h"

MyDDraw* MyDDraw::m_pDDraw = nullptr;
MyDDraw* g_pDDraw = nullptr;

MyDDraw::MyDDraw()
	: m_pDD7(nullptr)
	, m_pDDPrimary(nullptr)
	, m_pDDBack(nullptr)
	, m_pDDClipper(nullptr)
	, m_pLockedBackBuffer(nullptr)
	, m_dwLockedBackBufferPitch(0)
	, m_dwWidth(0)
	, m_dwHeight(0)
	, m_rcWindow{}
	, m_hWnd(NULL)
	, m_dwFrameCount(0)
	, m_dwFPS(0)
	, m_LastDrawTick(0)
	, m_wchInfoTxt{}
	, m_dwInfoTxtLen(0)
{
}

//MyDDraw::MyDDraw(HWND hWnd)
//	: m_pDD7(nullptr)
//	, m_pDDPrimary(nullptr)
//	, m_pDDBack(nullptr)
//	, m_pDDClipper(nullptr)
//	, m_pLockedBackBuffer(nullptr)
//	, m_dwLockedBackBufferPitch(0)
//	, m_dwWidth(0)
//	, m_dwHeight(0)
//	, m_rcWindow{}
//	, m_hWnd(hWnd)
//	, m_dwFrameCount(0)
//	, m_dwFPS(0)
//	, m_LastDrawTick(0)
//	, m_wchInfoTxt{}
//	, m_dwInfoTxtLen(0)
//{
//
//}

MyDDraw::~MyDDraw()
{
	SAFE_DELETE(m_pDDraw);
}

MyDDraw* MyDDraw::GetInstance()
{
	if (m_pDDraw == nullptr)
	{
		m_pDDraw = new MyDDraw;
		g_pDDraw = m_pDDraw;
	}

	return m_pDDraw;
}

////////////////////////////////////////////////////////////
// Name: InitDDraw()
// Desc: DDraw를 초기화합니다.
////////////////////////////////////////////////////////////
BOOL MyDDraw::InitDDraw(HWND hWnd)
{
	assert(hWnd != nullptr);

	m_hWnd = hWnd;
	HRESULT hr = NULL;

	hr = DirectDrawCreateEx(NULL, (LPVOID*)&m_pDD7, IID_IDirectDraw7, NULL);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to call CreateDirectDrawEX()", L"Error", MB_OK);
		return FALSE;
	}

	hr = m_pDD7->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to call SetCooperativeLevel()", L"Error", MB_OK);
		return FALSE;
	}

	DDSURFACEDESC2 ddsd = {};
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	hr = m_pDD7->CreateSurface(&ddsd, &m_pDDPrimary, nullptr);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to create primary surface", L"Error", MB_OK);
		return FALSE;
	}

	hr = m_pDD7->CreateClipper(0, &m_pDDClipper, nullptr);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to create clipper", L"Error", MB_OK);
		return FALSE;
	}

	m_pDDClipper->SetHWnd(0, m_hWnd);
	m_pDDPrimary->SetClipper(m_pDDClipper);

	OnUpdateWindowPos();

	DWORD dwWidth = m_rcWindow.right  - m_rcWindow.left;
	DWORD dwHeiht = m_rcWindow.bottom - m_rcWindow.top;
	if (FALSE == CreateBackBuffer(dwWidth, dwHeiht))
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		return FALSE;
	}

	return TRUE;
}

VOID MyDDraw::OnUpdateWindowPos()
{
	GetClientRect(m_hWnd, &m_rcWindow);
	::ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.left);
	::ClientToScreen(m_hWnd, (POINT*)&m_rcWindow.right);
}

////////////////////////////////////////////////////////////
// Name: CreateBackBuffer()
// Desc: 장면을 그릴 백버퍼를 생성합니다.
////////////////////////////////////////////////////////////
BOOL MyDDraw::CreateBackBuffer(DWORD dwWidth, DWORD dwHeight)
{
	DDSURFACEDESC2 ddsd = {};
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = dwWidth;
	ddsd.dwHeight = dwHeight;

	HRESULT hr = m_pDD7->CreateSurface(&ddsd, &m_pDDBack, nullptr);
	if (FAILED(hr))
	{
		MessageBox(m_hWnd, L"Failed to create back buffer", L"Error", MB_OK);
		return FALSE;
	}

	m_dwWidth  = dwWidth;
	m_dwHeight = dwHeight;

	return TRUE;
}

////////////////////////////////////////////////////////////
// Name: BeginDraw()
// Desc: 백버퍼에 장면을 그리기 전 필요한 작업을 수행합니다.
////////////////////////////////////////////////////////////
BOOL MyDDraw::BeginDraw()
{
	// 백버퍼를 잠그고 Drawing 시작을 알립니다.
	if (FALSE == LockBackBuffer())
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////
// Name: EndDraw()
// Desc: 장면 Drawing의 끝을 알립니다.
////////////////////////////////////////////////////////////
BOOL MyDDraw::EndDraw()
{
	if (FALSE == UnlockBackBuffer())
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////
// Name: LockBackBuffer()
// Desc: 장면을 그리기 위해 다른 곳에서 해당 영역에 
// 접근하지 못하도록 백버퍼를 잠그고 해당 영역에 대한 주소를 반환합니다.
// 이 주소는 Unlock 후 무효한 주소가 됩니다.
////////////////////////////////////////////////////////////
BOOL MyDDraw::LockBackBuffer()
{
	assert(m_pDDBack != nullptr);

	if (m_pDDBack)
	{
		DDSURFACEDESC2 ddsc = {};
		ddsc.dwSize = sizeof(DDSURFACEDESC2);

		HRESULT hr = m_pDDBack->Lock(nullptr, &ddsc, DDLOCK_WAIT, nullptr);
		if (FAILED(hr))
		{
			return FALSE;
		}

		m_pLockedBackBuffer = static_cast<char*>(ddsc.lpSurface);
		m_dwLockedBackBufferPitch = ddsc.lPitch;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////
// Name: UnlockBackBuffer()
// Desc: 백버퍼에 대한 잠금을 해제합니다. 
// Lock을 통해 얻었던 주소는 무효한 주소가 되며 write를 할 수 없는 상태가 됩니다.
////////////////////////////////////////////////////////////
BOOL MyDDraw::UnlockBackBuffer()
{
	assert(m_pDDBack != nullptr);

	if (m_pDDBack)
	{
		HRESULT hr = m_pDDBack->Unlock(nullptr);
		if (FAILED(hr))
		{
			return FALSE;
		}
	}

	m_pLockedBackBuffer = nullptr;
	m_dwLockedBackBufferPitch = 0;

	return TRUE;
}

////////////////////////////////////////////////////////////
// Name: DisplayScene()
// Desc: 백버퍼에 그린 장면을 전면 버퍼에 고속 복사하여
// 실제 화면에 나타냅니다.
////////////////////////////////////////////////////////////
VOID MyDDraw::DisplayScene()
{
	if (DD_OK != m_pDDPrimary->Blt(&m_rcWindow, m_pDDBack, nullptr, DDBLT_WAIT, nullptr))
	{
	#ifdef _DEBUG
		__debugbreak();
	#endif
		return;
	}
}

////////////////////////////////////////////////////////////
// Name: DrawSomething()
// Desc: 이미지 데이터가 아닌 픽셀 리터럴을 그립니다. 
////////////////////////////////////////////////////////////
VOID MyDDraw::DrawSomething(int* posX, int* posY, DWORD dwObjWidth, DWORD dwObjHeight, DWORD dwObjColor)
{
	const DWORD dwSrc = dwObjColor;

	char* pDest    = m_pLockedBackBuffer + (4 * (*posX)) + (m_dwLockedBackBufferPitch * (*posY));
	char* pOldDest = pDest;

	for (size_t y = 0; y < dwObjHeight; ++y)
	{
		for (size_t x = 0; x < dwObjWidth; ++x)
		{
			// 4바이트씩 write합니다.
			((DWORD*)pDest)[x] = dwSrc; 
		}
		pDest = pOldDest + m_dwLockedBackBufferPitch; // 다음 행으로 넘어갑니다.
		pOldDest = pDest;
	}
}

////////////////////////////////////////////////////////////
// Name: DrawSomething()
// Desc: 이미지 데이터가 아닌 픽셀 리터럴을 그립니다. 
////////////////////////////////////////////////////////////
VOID MyDDraw::DrawSomething(int* posX, int* posY, DWORD dwObjWidth, DWORD dwObjHeight, char* pRawImage)
{
	char* pSrc = pRawImage;
	char* pOldSrc = pSrc;

	char* pDest = m_pLockedBackBuffer + (4 * (*posX)) + (m_dwLockedBackBufferPitch * (*posY));
	char* pOldDest = pDest;

	for (size_t y = 0; y < dwObjHeight; ++y)
	{
		for (size_t x = 0; x < dwObjWidth; ++x)
		{
			// 4바이트씩 write합니다.
			((DWORD*)pDest)[x] = ((DWORD*)pSrc)[x];
		}

		pSrc = pOldSrc + (dwObjWidth * 4);
		pOldSrc = pSrc;
		pDest = pOldDest + m_dwLockedBackBufferPitch; // 다음 행으로 넘어갑니다.
		pOldDest = pDest;
	}
}

////////////////////////////////////////////////////////////
// Name: DrawImageData()
// Desc: 이미지 데이터를 그립니다.
////////////////////////////////////////////////////////////
VOID MyDDraw::DrawImageData(OUT int* posX, OUT int* posY, const ImageData& imgData)
{
	DWORD dwImgWidth  = imgData.GetWidth();
	DWORD dwImgHeight = imgData.GetHeight();

	if (FALSE == ClipArea(posX, posY, dwImgWidth, dwImgHeight))
	{
		return;
	}

	// (posX, posY) 좌표에 pImgData를 그립니다.
	char* pBeginOfCurLine = m_pLockedBackBuffer + (4 * (*posX)) + (m_dwLockedBackBufferPitch * (*posY));
	char* pOldBegin = pBeginOfCurLine;

	COMPRESSED_LINE* const pCompImage = imgData.GetCompressedImagePtr();

	// 행 단위로 write합니다.
	for (DWORD y = 0; y < dwImgHeight; ++y)
	{
		// 현재 행에 존재하는 픽셀 스트림의 수만큼 write합니다.
		COMPRESSED_LINE* pCurLine = pCompImage + y;
		PIXEL_STREAM* pPixelStream = pCurLine->pPixelStream;
		for (DWORD i = 0; i < pCurLine->dwPixelStreamNum; ++i)
		{
			DWORD dwPixel = pPixelStream[i].dwPixel;
			DWORD dwPixelNum = pPixelStream[i].wPixelNum;

			// 연속된 동일한 색상을 write
			char* pDest = pBeginOfCurLine;
			pDest += (pPixelStream[i].wBeginPosX * 4);
			for (DWORD j = 0; j < dwPixelNum; ++j)
			{
				((DWORD*)pDest)[j] = dwPixel;
			}
		}

		pBeginOfCurLine = pOldBegin + m_dwLockedBackBufferPitch;
		pOldBegin = pBeginOfCurLine;
	}
}

////////////////////////////////////////////////////////////
// Name: ClipArea()
// Desc: 그려지게 될 영역이 화면을 벗어나지 않도록 영역을 조정합니다.
////////////////////////////////////////////////////////////
BOOL MyDDraw::ClipArea(int* outPosX, int* outPosY, const DWORD dwObjWidth, const DWORD dwObjHeight)
{
	int xBeginPoint = *outPosX;
	int yBeginPoint = *outPosY;
	int xEndPoint   = *outPosX + dwObjWidth;
	int yEndPoint   = *outPosY + dwObjHeight;

	if (xBeginPoint < 0)
	{
		*outPosX = 0;
	}
	if (xEndPoint > (int)m_dwWidth)
	{
		*outPosX = m_dwWidth - dwObjWidth;
	}
	if (yBeginPoint < 0)
	{
		*outPosY = 0;
	}
	if (yEndPoint > (int)m_dwHeight)
	{
		*outPosY = m_dwHeight - dwObjHeight;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////
// Name: ClearBackBuffer()
// Desc: 백버퍼를 empty 상태로 만듭니다.
////////////////////////////////////////////////////////////
VOID MyDDraw::ClearBackBuffer(DWORD dwSquarePosY)
{
	if (m_pLockedBackBuffer == nullptr)
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		return;
	}

	for (size_t y = 0; y < dwSquarePosY; ++y)
	{
		ZeroMemory(m_pLockedBackBuffer + (y * m_dwLockedBackBufferPitch), 4 * m_dwWidth);
	}
}

BOOL MyDDraw::SetColorKey(DWORD dwColorKey)
{
	DDCOLORKEY ddck = {};
	ddck.dwColorSpaceLowValue = dwColorKey;
	ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
	m_pDDBack->SetColorKey(DDCKEY_SRCBLT, &ddck);

	return TRUE;
}

////////////////////////////////////////////////
// 아래부턴 GDI를 이용한 텍스트 출력을 위한 함수들
////////////////////////////////////////////////

void MyDDraw::UpdateInfoTxt()
{
	m_dwInfoTxtLen = swprintf_s(m_wchInfoTxt, L"FPS : %u", m_dwFPS);
}

BOOL MyDDraw::CheckFPS()
{
	BOOL	bUpdated = FALSE;

	ULONGLONG CurTick = GetTickCount64();
	if (CurTick - m_LastDrawTick > 1000)
	{
		DWORD	dwOldPFS = m_dwFPS;

		m_dwFPS = m_dwFrameCount;
		m_LastDrawTick = CurTick;
		m_dwFrameCount = 0;
		if (m_dwFPS != dwOldPFS)
		{
			UpdateInfoTxt();
			bUpdated = TRUE;
		}
		else
		{
			int a = 0;
		}
	}
	m_dwFrameCount++;
	return bUpdated;
}

void MyDDraw::DrawInfo(HDC hDC)
{
	const WCHAR* wchTxt = m_wchInfoTxt;
	DWORD dwLen = m_dwInfoTxtLen;

	WCHAR wchScoreTxt[16];
	_itow_s(ScoreManager::GetInstance()->GetScore(), wchScoreTxt, 16,  10);
	DWORD dwScoreTextLen = lstrlenW(wchScoreTxt);
	int scoreTextX = m_dwWidth / 2;

	WriteText(wchTxt, dwLen, 0, 0, 0xffff0000, hDC);
	WriteText(wchScoreTxt, dwScoreTextLen, scoreTextX, 0, 0x00ff0000, hDC);
}

void MyDDraw::WriteText(const WCHAR* wchTxt, DWORD dwLen, int x, int y, DWORD dwColor, HDC hDC)
{
	SetBkMode(hDC, TRANSPARENT);

	RECT	textRect, texRectSide[4];
	int		iWidth = 0;
	int		iHeight = 0;
	GetFontSize(&iWidth, &iHeight, wchTxt, dwLen, hDC);

	textRect.left = x;
	textRect.top = y;
	textRect.right = textRect.left + iWidth;
	textRect.bottom = textRect.top + iHeight;

	texRectSide[0].left = textRect.left - 1;
	texRectSide[0].top = textRect.top - 1;
	texRectSide[0].right = textRect.right - 1;
	texRectSide[0].bottom = textRect.bottom - 1;

	texRectSide[1].left = textRect.left + 1;
	texRectSide[1].top = textRect.top - 1;
	texRectSide[1].right = textRect.right + 1;
	texRectSide[1].bottom = textRect.bottom - 1;

	texRectSide[2].left = textRect.left + 1;
	texRectSide[2].top = textRect.top + 1;
	texRectSide[2].right = textRect.right + 1;
	texRectSide[2].bottom = textRect.bottom + 1;

	texRectSide[3].left = textRect.left - 1;
	texRectSide[3].top = textRect.top + 1;
	texRectSide[3].right = textRect.right - 1;
	texRectSide[3].bottom = textRect.bottom + 1;

	SetTextColor(hDC, 0x00000000);
	for (DWORD i = 0; i < 4; i++)
	{
		DrawText(hDC, wchTxt, -1, &textRect, DT_LEFT | DT_WORDBREAK);
	}

	DWORD r = (dwColor & 0x00ff0000) >> 16;
	DWORD g = (dwColor & 0x0000ff00) >> 8;
	DWORD b = (dwColor & 0x000000ff);

	COLORREF color = RGB(r, g, b);
	SetTextColor(hDC, color);
	DrawText(hDC, wchTxt, -1, &textRect, DT_LEFT | DT_WORDBREAK);
}

BOOL MyDDraw::GetFontSize(int* piOutWidth, int* piOutHeight, const WCHAR* wchString, DWORD dwStrLen, HDC hDC)
{
	BOOL	bResult = FALSE;
	*piOutWidth = 1;
	*piOutHeight = 1;

	SIZE	strSize;
	BOOL bGetSize = GetTextExtentPoint32W(hDC, wchString, dwStrLen, &strSize);


	if (bGetSize)
	{
		*piOutWidth = strSize.cx;
		*piOutHeight = strSize.cy;
		bResult = TRUE;
	}
	return bResult;
}

BOOL MyDDraw::BeginGDI(HDC* pOutDC)
{
	BOOL	bResult = FALSE;
	HDC	hDC = nullptr;

	HRESULT hr = m_pDDBack->GetDC(&hDC);
	if (FAILED(hr))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto lb_return;
	}
	bResult = TRUE;
	*pOutDC = hDC;

lb_return:
	return bResult;
}

void MyDDraw::EndGDI(HDC hDC)
{
	m_pDDBack->ReleaseDC(hDC);
}
