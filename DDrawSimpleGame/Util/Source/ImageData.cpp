#include "../../pch/stdafx.h"
#include "../Header/ImageData.h"

// Constructor
ImageData::ImageData()
	: m_dwWidth(0)
	, m_dwHeight(0)
	, m_pCompressedImage(nullptr)
{
}

// Destructor
ImageData::~ImageData()
{
	delete[] m_pCompressedImage;
	m_pCompressedImage = nullptr;
}

////////////////////////////////////////////////////////////
// Name: Create()
// Desc: 해당 이미지에서 투명으로 처리할 컬러를 제외하고 
// 남은 색상들을 압축하여 이미지 데이터를 생성합니다.
////////////////////////////////////////////////////////////
BOOL ImageData::Create(const char* pRawImage, DWORD dwWidth, DWORD dwHeight, DWORD dwColorKey)
{
	if (pRawImage == nullptr)
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		return FALSE;
	}

	const DWORD pixelStreamBufferSize = sizeof(PIXEL_STREAM) * dwWidth * dwHeight;
	char* const pPixelStreams = new char[pixelStreamBufferSize]; // 여러개의 픽셀 스트림을 저장할 공간의 시작 주소(고정)
	ZeroMemory(pPixelStreams, pixelStreamBufferSize);

	char* pCurPixelStreams = pPixelStreams;	// 현재 write할 위치
	DWORD createdPixelStreamSize = 0;

	// 각 라인(행)에 대해 픽셀 스트림을 생성합니다.
	COMPRESSED_LINE* pCompressedLine = new COMPRESSED_LINE[dwHeight];
	const size_t compressedLinesSize = sizeof(COMPRESSED_LINE) * dwHeight;
	ZeroMemory(pCompressedLine, compressedLinesSize);
	for (size_t y = 0; y < dwHeight; ++y)
	{
		// pRawImage 위치로부터 dwWidth만큼의 영역(1Line)을 조사하여 픽셀 스트림을 pCurPixelStreams위치에 생성합니다.
		// 생성한 픽셀 스트림 개수를 반환
		DWORD pixelStreamNum = CreatePerLine(pCurPixelStreams, pixelStreamBufferSize, (DWORD*)pRawImage, dwWidth, dwColorKey);
		DWORD lineSize = sizeof(PIXEL_STREAM) * pixelStreamNum;

		// 0행의 COMPRESSED_LINE에서 y행 COMPRESSED_LINE로 넘어가기 위한 메모리 offset(byte 단위)
		assert(pCurPixelStreams >= pPixelStreams);
		DWORD offsetForNextLine = (DWORD)(pCurPixelStreams - pPixelStreams);

		// write 및 read할 다음 위치로 넘어갑니다.
		pCurPixelStreams += lineSize;
		createdPixelStreamSize += lineSize;
		pRawImage += (dwWidth * 4);

		pCompressedLine[y].dwPixelStreamNum = pixelStreamNum;
		pCompressedLine[y].dwLineSize = lineSize;
		pCompressedLine[y].dwOffset = offsetForNextLine;
	}
	
	// COMPRESSED_LINE의 집합과 pPixelStreams의 집합을 하나의 메모리로 합칩니다.
	DWORD newMemorySize = compressedLinesSize + createdPixelStreamSize;
	m_pCompressedImage = (COMPRESSED_LINE*)malloc(newMemorySize);

	// PIXEL_STREAM의 집합을 저장합니다
	char* pDest = (char*)(m_pCompressedImage) + compressedLinesSize;
	for (size_t y = 0; y < dwHeight; ++y)
	{
		char* pSrc = pPixelStreams + pCompressedLine[y].dwOffset;
		memcpy(pDest, pSrc, pCompressedLine[y].dwLineSize);

		pCompressedLine[y].pPixelStream = (PIXEL_STREAM*)pDest;
		pDest += pCompressedLine[y].dwLineSize;
	}

	// COMPRESSED_LINE의 집합을 저장
	memcpy(m_pCompressedImage, pCompressedLine, compressedLinesSize);
	
	delete[] pPixelStreams;
	delete[] pCompressedLine;

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Name: CreatePerLine()
// Desc: 해당 이미지의 한 행(line)에 대해 컬러키를 제외한 픽셀 데이터를 생성합니다.
////////////////////////////////////////////////////////////////////////////////
DWORD ImageData::CreatePerLine(OUT char* pDest, int maxMemSize, const DWORD* pRawImage, DWORD dwWidth, DWORD dwColorKey)
{
	PIXEL_STREAM* pPixelStreams = reinterpret_cast<PIXEL_STREAM*>(pDest);

	WORD x = 0;
	DWORD pixelStreamCount = 0;
	while (x < dwWidth)
	{
		// 이미지를 탐색하면서 컬러키와 다른 색상을 가진 픽셀의 위치를 찾습니다.
		while (pRawImage[x] == dwColorKey)
		{
			++x;
			if (x >= dwWidth)
			{
				assert((x > dwWidth) == false);
				return pixelStreamCount;
			}
		}

		// 다른 색상을 찾았다면 해당 위치를 저장하고
		// 해당 위치부터 얼마나 연속되는(동일한) 픽셀을 가지는지 조사합니다.
		DWORD curColor = pRawImage[x];
		pPixelStreams[pixelStreamCount].wBeginPosX = x;
		pPixelStreams[pixelStreamCount].dwPixel = curColor;

		WORD continuousColorNum = 0;
		while ((x < dwWidth) && (pRawImage[x] == curColor))
		{
			++x;
			++continuousColorNum;
		}

		pPixelStreams[pixelStreamCount].wPixelNum = continuousColorNum;
		++pixelStreamCount;
	}

	return pixelStreamCount;
}