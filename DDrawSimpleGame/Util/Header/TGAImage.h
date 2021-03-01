#pragma once

#pragma pack(push, 1)
struct TGA_Header
{
	char IdLength;
	char ColorMapType;
	char ImageType;
	WORD ColorMapFirst;
	WORD ColorMapLast;
	char ColorMapBits;
	WORD FirstX;
	WORD FirstY;
	WORD Width;
	WORD Height;
	char Bits;
	char Descriptor;
};
static_assert(sizeof(TGA_Header) == 18, "TGA_Header's size must be 18bytes");


#pragma pack(pop)
class TGAImage final
{
public:
	TGAImage();
	~TGAImage();

	inline DWORD GetWidth() const { return m_dwWidth; }
	inline DWORD GetHeight() const { return m_dwHeight; }
	inline char* GetRawImage() const { return m_pRawImage; }
	DWORD GetPixel(size_t x, size_t y) const;
	BOOL Load24BitsTGA(const char* szFileName, DWORD dwBytesPerPixel);

private:
	VOID Destroy();
	BOOL LoadTGAImage(FILE* fp, DWORD dwBytesPerPixel);
	VOID CopyPixel(char* pDest, char* pSrc, DWORD destBPP, DWORD srcBPP, const TGA_Header& header);

	DWORD m_dwWidth;
	DWORD m_dwHeight;
	char* m_pRawImage;
	DWORD m_dwBytePerPixel;
};