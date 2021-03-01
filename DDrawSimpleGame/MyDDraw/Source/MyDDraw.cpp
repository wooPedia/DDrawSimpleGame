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
// Desc: DDraw�� �ʱ�ȭ�մϴ�.
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
// Desc: ����� �׸� ����۸� �����մϴ�.
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
// Desc: ����ۿ� ����� �׸��� �� �ʿ��� �۾��� �����մϴ�.
////////////////////////////////////////////////////////////
BOOL MyDDraw::BeginDraw()
{
	// ����۸� ��װ� Drawing ������ �˸��ϴ�.
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
// Desc: ��� Drawing�� ���� �˸��ϴ�.
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
// Desc: ����� �׸��� ���� �ٸ� ������ �ش� ������ 
// �������� ���ϵ��� ����۸� ��װ� �ش� ������ ���� �ּҸ� ��ȯ�մϴ�.
// �� �ּҴ� Unlock �� ��ȿ�� �ּҰ� �˴ϴ�.
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
// Desc: ����ۿ� ���� ����� �����մϴ�. 
// Lock�� ���� ����� �ּҴ� ��ȿ�� �ּҰ� �Ǹ� write�� �� �� ���� ���°� �˴ϴ�.
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
// Desc: ����ۿ� �׸� ����� ���� ���ۿ� ��� �����Ͽ�
// ���� ȭ�鿡 ��Ÿ���ϴ�.
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
// Desc: �̹��� �����Ͱ� �ƴ� �ȼ� ���ͷ��� �׸��ϴ�. 
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
			// 4����Ʈ�� write�մϴ�.
			((DWORD*)pDest)[x] = dwSrc; 
		}
		pDest = pOldDest + m_dwLockedBackBufferPitch; // ���� ������ �Ѿ�ϴ�.
		pOldDest = pDest;
	}
}

////////////////////////////////////////////////////////////
// Name: DrawSomething()
// Desc: �̹��� �����Ͱ� �ƴ� �ȼ� ���ͷ��� �׸��ϴ�. 
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
			// 4����Ʈ�� write�մϴ�.
			((DWORD*)pDest)[x] = ((DWORD*)pSrc)[x];
		}

		pSrc = pOldSrc + (dwObjWidth * 4);
		pOldSrc = pSrc;
		pDest = pOldDest + m_dwLockedBackBufferPitch; // ���� ������ �Ѿ�ϴ�.
		pOldDest = pDest;
	}
}

////////////////////////////////////////////////////////////
// Name: DrawImageData()
// Desc: �̹��� �����͸� �׸��ϴ�.
////////////////////////////////////////////////////////////
VOID MyDDraw::DrawImageData(OUT int* posX, OUT int* posY, const ImageData& imgData)
{
	DWORD dwImgWidth  = imgData.GetWidth();
	DWORD dwImgHeight = imgData.GetHeight();

	if (FALSE == ClipArea(posX, posY, dwImgWidth, dwImgHeight))
	{
		return;
	}

	// (posX, posY) ��ǥ�� pImgData�� �׸��ϴ�.
	char* pBeginOfCurLine = m_pLockedBackBuffer + (4 * (*posX)) + (m_dwLockedBackBufferPitch * (*posY));
	char* pOldBegin = pBeginOfCurLine;

	COMPRESSED_LINE* const pCompImage = imgData.GetCompressedImagePtr();

	// �� ������ write�մϴ�.
	for (DWORD y = 0; y < dwImgHeight; ++y)
	{
		// ���� �࿡ �����ϴ� �ȼ� ��Ʈ���� ����ŭ write�մϴ�.
		COMPRESSED_LINE* pCurLine = pCompImage + y;
		PIXEL_STREAM* pPixelStream = pCurLine->pPixelStream;
		for (DWORD i = 0; i < pCurLine->dwPixelStreamNum; ++i)
		{
			DWORD dwPixel = pPixelStream[i].dwPixel;
			DWORD dwPixelNum = pPixelStream[i].wPixelNum;

			// ���ӵ� ������ ������ write
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
// Desc: �׷����� �� ������ ȭ���� ����� �ʵ��� ������ �����մϴ�.
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
// Desc: ����۸� empty ���·� ����ϴ�.
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
// �Ʒ����� GDI�� �̿��� �ؽ�Ʈ ����� ���� �Լ���
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
