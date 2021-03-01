#pragma once

/*
	File Name: ImageData.h
	Desc: 래스터 이미지에 대해 0번 압축을 수행
*/

// 연속된 픽셀을 압축한 데이터 타입
// 아래의 예는 3개의 픽셀 스트림을 가짐
// i.e. RRRRGGBB -> (0,4,R), (4,2,G), (6,2,B) 
#pragma pack(push, 4)
struct PIXEL_STREAM
{
	WORD  wBeginPosX; /* 연속된 픽셀의 시작 x좌표 */
	WORD  wPixelNum;  /* 연속된 픽셀의 개수	    */
	DWORD dwPixel;	  /* 픽셀의 컬러값(16진수)   */
};
static_assert(sizeof(PIXEL_STREAM) == 8, "PIXEL_STREAM's size is 8bytes");


// 한 라인(행)에 존재하는 픽셀 스트림을 압축한 데이터 타입
#pragma pack(pop)
struct COMPRESSED_LINE
{
	DWORD dwPixelStreamNum;  /* 현재 라인에 존재하는 픽셀 스트림의 개수    */
	DWORD dwLineSize;		 /* sizeof(PIXEL_STREAM) * dwPixelStreamNum */
	union
	{
		PIXEL_STREAM* pPixelStream; /* 현재 라인의 픽셀 스트림을 가리키는 포인터			  */
		DWORD dwOffset;				/* 다음 라인의 첫 픽셀 스트림으로 가기 위한 오프셋(Bytes) */
	};
};
//static_assert(sizeof(COMPRESSED_LINE) == 12, "COMPRESSED_LINE's size is 12bytes");


// 동일한 이미지에 대한 해당 클래스 객체는 
// 서로 공유하도록. (이미지 데이터 생성 후 용도는 항상 readonly기 때문)
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
	COMPRESSED_LINE* m_pCompressedImage; // COMPRESSED_LINE의 집합 + PIXEL_STREAM의 집합
};
