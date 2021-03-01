#pragma once

enum eColor
{
	COL_BLACK = 0x00000000,
	COL_WHITE = 0x00FFFFFF,
	COL_RED   = 0x00FF0000,
	COL_GREEN = 0x0000FF00,
	COL_BLUE  = 0x000000FF,
};


////////////////////////////////////////////////
// 애니메이션 이미지를 가지는 오브젝트일 경우 
// 아래에 애니메이션 이미지의 크기를 정의
///////////////////////////////////////////////

enum eAni_Hero
{
	AH_HERO_NORMAL = 8,	// 플레이어 기본 동작
};

enum eAni_HitEffect
{
	AHE_HERO_NORMAL = 4,	// 플레이어 총알 타격 시 이펙트
};