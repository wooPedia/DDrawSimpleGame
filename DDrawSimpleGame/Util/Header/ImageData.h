#pragma once

/*
	File Name: ImageData.h
	Desc: ������ �̹����� ���� 0�� ������ ����
*/

// ���ӵ� �ȼ��� ������ ������ Ÿ��
// �Ʒ��� ���� 3���� �ȼ� ��Ʈ���� ����
// i.e. RRRRGGBB -> (0,4,R), (4,2,G), (6,2,B) 
#pragma pack(push, 4)
struct PIXEL_STREAM
{
	WORD  wBeginPosX; /* ���ӵ� �ȼ��� ���� x��ǥ */
	WORD  wPixelNum;  /* ���ӵ� �ȼ��� ����	    */
	DWORD dwPixel;	  /* �ȼ��� �÷���(16����)   */
};
static_assert(sizeof(PIXEL_STREAM) == 8, "PIXEL_STREAM's size is 8bytes");


// �� ����(��)�� �����ϴ� �ȼ� ��Ʈ���� ������ ������ Ÿ��
#pragma pack(pop)
struct COMPRESSED_LINE
{
	DWORD dwPixelStreamNum;  /* ���� ���ο� �����ϴ� �ȼ� ��Ʈ���� ����    */
	DWORD dwLineSize;		 /* sizeof(PIXEL_STREAM) * dwPixelStreamNum */
	union
	{
		PIXEL_STREAM* pPixelStream; /* ���� ������ �ȼ� ��Ʈ���� ����Ű�� ������			  */
		DWORD dwOffset;				/* ���� ������ ù �ȼ� ��Ʈ������ ���� ���� ������(Bytes) */
	};
};
//static_assert(sizeof(COMPRESSED_LINE) == 12, "COMPRESSED_LINE's size is 12bytes");


// ������ �̹����� ���� �ش� Ŭ���� ��ü�� 
// ���� �����ϵ���. (�̹��� ������ ���� �� �뵵�� �׻� readonly�� ����)
class ImageData final
{
public:
	ImageData();
	~ImageData();

	ImageData(const ImageData&) = default;
	ImageData& operator=(const ImageData&) = default;

	BOOL Create(const char* pRawImage, DWORD dwWidth, DWORD dwHeight, DWORD dwColorKey);
	inline COMPRESSED_LINE* GetCompressedImagePtr() const { return m_pCompressedImage; }
	inline DWORD GetWidth() const { return m_dwWidth; }
	inline DWORD GetHeight() const { return m_dwHeight; }

private:
	DWORD CreatePerLine(OUT char* pDest, int maxMemSize, const DWORD* pRawImage, DWORD dwWidth, DWORD dwColorKey);

	DWORD m_dwWidth;
	DWORD m_dwHeight;
	COMPRESSED_LINE* m_pCompressedImage; // COMPRESSED_LINE�� ���� + PIXEL_STREAM�� ����
};
