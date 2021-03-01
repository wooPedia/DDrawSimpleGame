#include "../../pch/stdafx.h"
#include "../Header/TGAImage.h"

TGAImage::TGAImage()
	: m_dwWidth(0)
	, m_dwHeight(0)
	, m_pRawImage(nullptr)
	, m_dwBytePerPixel(0)
{
}

TGAImage::~TGAImage()
{
	Destroy();
}


DWORD TGAImage::GetPixel(size_t x, size_t y) const
{
	if (m_pRawImage)
	{
		return *(DWORD*)(m_pRawImage + (x * 4) + (y * m_dwWidth * 4));
	}

	return 0x00000000;
}



BOOL TGAImage::Load24BitsTGA(const char* szFileName, DWORD dwBytesPerPixel)
{
	FILE* fp = NULL;

	fopen_s(&fp, szFileName, "rb");
	if (fp == NULL)
	{
		return FALSE;
	}

	BOOL bResult = LoadTGAImage(fp, dwBytesPerPixel);
	fclose(fp);

	return bResult;
}

VOID TGAImage::Destroy()
{
	if (m_pRawImage)
	{
		delete[] m_pRawImage;
		m_pRawImage = nullptr;
	}
}

BOOL TGAImage::LoadTGAImage(FILE* fp, DWORD dwBytesPerPixel)
{
	TGA_Header header = {};
	fread(&header, sizeof(header), 1, fp);
	
	// src 持失
	const UINT8 srcBPP = (header.Bits / 8);
	DWORD srcBufSize = srcBPP * header.Width * header.Height;
	char* pSrc = new char[srcBufSize];
	ZeroMemory(pSrc, srcBufSize);

	fread(pSrc, sizeof(BYTE), srcBufSize, fp);

	// dest 持失
	const DWORD destBPP = dwBytesPerPixel;
	const DWORD destBufSize = header.Width * header.Height * destBPP;
	char* pDest = new char[destBufSize];
	
	// copy a pixel dest from src.
	CopyPixel(pDest, pSrc, destBPP, srcBPP, header);

	delete[] pSrc;
	pSrc = nullptr;

	m_pRawImage = pDest;
	m_dwBytePerPixel = destBPP;
	m_dwWidth = header.Width;
	m_dwHeight = header.Height;

	return TRUE;
}

VOID TGAImage::CopyPixel(char* pDest, char* pSrc, DWORD destBPP, DWORD srcBPP, const TGA_Header& header)
{
	DWORD line = 0;
	if (header.Descriptor == 0)
	{
		line = header.Height;
	}

	DWORD widthInBytes = header.Width * destBPP;
	if (destBPP == 2)
	{
		if (header.Descriptor == 0)
		{
			for (DWORD y = 0; y < header.Height; ++y)
			{
				--line;
				for (DWORD x = 0; x < header.Width; ++x)
				{
					*(WORD*)(pDest + (line * header.Width * destBPP) + (x * destBPP)) =
						pSrc[y * header.Width * 3 + x * 3 + 0] >> 3 |
						pSrc[y * header.Width * 3 + x * 3 + 1] >> 2 << 5 |
						pSrc[y * header.Width * 3 + x * 3 + 2] >> 3 << 11;
				}
			}
		}
		else
		{
			for (DWORD y = 0; y < header.Height; ++y)
			{
				for (DWORD x = 0; x < header.Width; ++x)
				{
					*(WORD*)(pDest + (line * header.Width * destBPP) + (x * destBPP)) =
						pSrc[y * header.Width * 3 + x * 3 + 0] >> 3 |
						pSrc[y * header.Width * 3 + x * 3 + 1] >> 2 << 5 |
						pSrc[y * header.Width * 3 + x * 3 + 2] >> 3 << 11;
				}
				++line;
			}
		}
	}
	else if (destBPP == 4)
	{
		if (header.Descriptor == 0)
		{
			for (DWORD y = 0; y < header.Height; ++y)
			{
				--line;
				for (DWORD x = 0; x < header.Width; ++x)
				{
					pDest[line * widthInBytes + x * destBPP + 0] = pSrc[y * header.Width * srcBPP + x * srcBPP + 0];
					pDest[line * widthInBytes + x * destBPP + 1] = pSrc[y * header.Width * srcBPP + x * srcBPP + 1];
					pDest[line * widthInBytes + x * destBPP + 2] = pSrc[y * header.Width * srcBPP + x * srcBPP + 2];
					pDest[line * widthInBytes + x * destBPP + 3] = 0x00;
				}
			}
		}
		else
		{
			for (DWORD y = 0; y < header.Height; ++y)
			{
				for (DWORD x = 0; x < header.Width; ++x)
				{
					pDest[line * widthInBytes + x * destBPP + 0] = pSrc[y * header.Width * srcBPP + x * srcBPP + 0];
					pDest[line * widthInBytes + x * destBPP + 1] = pSrc[y * header.Width * srcBPP + x * srcBPP + 1];
					pDest[line * widthInBytes + x * destBPP + 2] = pSrc[y * header.Width * srcBPP + x * srcBPP + 2];
					pDest[line * widthInBytes + x * destBPP + 3] = 0x00;
				}
				++line;
			}
		}
	}
}