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
// Desc: �ش� �̹������� �������� ó���� �÷��� �����ϰ� 
// ���� ������� �����Ͽ� �̹��� �����͸� �����մϴ�.
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
	char* const pPixelStreams = new char[pixelStreamBufferSize]; // �������� �ȼ� ��Ʈ���� ������ ������ ���� �ּ�(����)
	ZeroMemory(pPixelStreams, pixelStreamBufferSize);

	char* pCurPixelStreams = pPixelStreams;	// ���� write�� ��ġ
	DWORD createdPixelStreamSize = 0;

	// �� ����(��)�� ���� �ȼ� ��Ʈ���� �����մϴ�.
	COMPRESSED_LINE* pCompressedLine = new COMPRESSED_LINE[dwHeight];
	const size_t compressedLinesSize = sizeof(COMPRESSED_LINE) * dwHeight;
	ZeroMemory(pCompressedLine, compressedLinesSize);
	for (size_t y = 0; y < dwHeight; ++y)
	{
		// pRawImage ��ġ�κ��� dwWidth��ŭ�� ����(1Line)�� �����Ͽ� �ȼ� ��Ʈ���� pCurPixelStreams��ġ�� �����մϴ�.
		// ������ �ȼ� ��Ʈ�� ������ ��ȯ
		DWORD pixelStreamNum = CreatePerLine(pCurPixelStreams, pixelStreamBufferSize, (DWORD*)pRawImage, dwWidth, dwColorKey);
		DWORD lineSize = sizeof(PIXEL_STREAM) * pixelStreamNum;

		// 0���� COMPRESSED_LINE���� y�� COMPRESSED_LINE�� �Ѿ�� ���� �޸� offset(byte ����)
		assert(pCurPixelStreams >= pPixelStreams);
		DWORD offsetForNextLine = (DWORD)(pCurPixelStreams - pPixelStreams);

		// write �� read�� ���� ��ġ�� �Ѿ�ϴ�.
		pCurPixelStreams += lineSize;
		createdPixelStreamSize += lineSize;
		pRawImage += (dwWidth * 4);

		pCompressedLine[y].dwPixelStreamNum = pixelStreamNum;
		pCompressedLine[y].dwLineSize = lineSize;
		pCompressedLine[y].dwOffset = offsetForNextLine;
	}
	
	// COMPRESSED_LINE�� ���հ� pPixelStreams�� ������ �ϳ��� �޸𸮷� ��Ĩ�ϴ�.
	DWORD newMemorySize = compressedLinesSize + createdPixelStreamSize;
	m_pCompressedImage = (COMPRESSED_LINE*)malloc(newMemorySize);

	// PIXEL_STREAM�� ������ �����մϴ�
	char* pDest = (char*)(m_pCompressedImage) + compressedLinesSize;
	for (size_t y = 0; y < dwHeight; ++y)
	{
		char* pSrc = pPixelStreams + pCompressedLine[y].dwOffset;
		memcpy(pDest, pSrc, pCompressedLine[y].dwLineSize);

		pCompressedLine[y].pPixelStream = (PIXEL_STREAM*)pDest;
		pDest += pCompressedLine[y].dwLineSize;
	}

	// COMPRESSED_LINE�� ������ ����
	memcpy(m_pCompressedImage, pCompressedLine, compressedLinesSize);
	
	delete[] pPixelStreams;
	delete[] pCompressedLine;

	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Name: CreatePerLine()
// Desc: �ش� �̹����� �� ��(line)�� ���� �÷�Ű�� ������ �ȼ� �����͸� �����մϴ�.
////////////////////////////////////////////////////////////////////////////////
DWORD ImageData::CreatePerLine(OUT char* pDest, int maxMemSize, const DWORD* pRawImage, DWORD dwWidth, DWORD dwColorKey)
{
	PIXEL_STREAM* pPixelStreams = reinterpret_cast<PIXEL_STREAM*>(pDest);

	WORD x = 0;
	DWORD pixelStreamCount = 0;
	while (x < dwWidth)
	{
		// �̹����� Ž���ϸ鼭 �÷�Ű�� �ٸ� ������ ���� �ȼ��� ��ġ�� ã���ϴ�.
		while (pRawImage[x] == dwColorKey)
		{
			++x;
			if (x >= dwWidth)
			{
				assert((x > dwWidth) == false);
				return pixelStreamCount;
			}
		}

		// �ٸ� ������ ã�Ҵٸ� �ش� ��ġ�� �����ϰ�
		// �ش� ��ġ���� �󸶳� ���ӵǴ�(������) �ȼ��� �������� �����մϴ�.
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