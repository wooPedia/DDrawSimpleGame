#pragma once

#include <list>

class MyDDraw;
class CGameObject;
class TGAImage;
class ImageData;

class Hero;
class AirCraft;
class Ammo;
class HitEffect;
class ScoreManager;
struct GameData_Lua;

template <class T> class SingleObjectManager;
template <class T> class MultipleObjectManager;

class MyGame final
{
public:
	MyGame(HWND hWnd);
	~MyGame();

	BOOL Initialize();
	VOID Process();

	VOID OnUpdateWindowPos();

	VOID OnKeyDown(UINT vKey, UINT scanCode);
	VOID OnKeyUp(UINT vKey, UINT scanCode);

// private function
private: 
	VOID Cleanup();

	VOID OnFrameMove(float fElapsedTime);
	VOID DrawScene();

	VOID InitObjectManagers();
	VOID InitSingleObjectManagers();
	VOID InitMultipleObjectManagers();

	VOID DeleteObjectManagers();

	BOOL GetGameData_Lua();
	
	VOID InitImageData();

	VOID MoveObjects();
	VOID MovePlayer();
	VOID MoveAmmo();
	VOID MoveEnemy();

	VOID UpdateAnimation();

	VOID DetectCollision();
	VOID DetectPlayerCollision();
	VOID DetectCollisionPlayerVsAmmo();
	VOID DetectCollisionPlayerVsEnemy();
	VOID DetectCollisionPlayerAmmoVsWall();
	VOID DetectEnemyCollision();
	VOID DetectCollisionEnemyVsAmmo();
	VOID DetectCollisionEnemyVsWall();
	VOID DetectCollisionEnemyAmmoVsWall();

	VOID DeleteDeadObjects();

	VOID ProcessEnemy();
	VOID ShootMissilePerTick();
	VOID FillNewEnemy();

	BOOL IsCollisionObjects(const CGameObject& obj1, const CGameObject& obj2);
	BOOL IsCollisionRectVsRect(const INT_RECT2& rect1, const INT_RECT2& rect2);
	
	VOID DrawBackground(int x, int y, DWORD dwWidth, DWORD dwHeight, DWORD dwColor);
	
	VOID ShootMissile();
	VOID ShootMissileFromEnemy(std::list<std::unique_ptr<CGameObject>>::iterator itEnemy);
	VOID ShootMissileFromEnemy(const AirCraft* pEnemy);

// private variables
private: 
	HWND m_hWnd;
	
	// FPS ฐüทร
	DWORD		  m_dwGameFPS;
	DWORD		  m_dwCurFPS;
	float		  m_fTicksPerGameFrame;
	ULONGLONG	  m_prvGameFrameTick;
	LARGE_INTEGER m_prvCounter;

	BOOL m_bKeyDown_Up;
	BOOL m_bKeyDown_Down;
	BOOL m_bKeyDown_Left;
	BOOL m_bKeyDown_Right;

	DWORD m_dwSurfaceWidth;
	DWORD m_dwSurfaceHeight;

	DWORD m_dwBackColor;

	SingleObjectManager<Hero>*		  m_pPlayerMng;
	MultipleObjectManager<AirCraft>*  m_pEnemyMng;
	MultipleObjectManager<Ammo>*	  m_pPlayerAmmoMng;
	MultipleObjectManager<Ammo>*	  m_pEnemyAmmoMng;
	MultipleObjectManager<HitEffect>* m_pPlayerAmmoHitEffect;
									 
	std::shared_ptr<ImageData*[]>	  m_sptrPlayerAnimation;
	std::shared_ptr<ImageData>		  m_sptrPlayerAmmoImgData;
	std::shared_ptr<ImageData>		  m_sptrEnemyImgData;
	std::shared_ptr<ImageData>		  m_sptrEnemyAmmoImgData;
	std::shared_ptr<ImageData*[]>	  m_sptrPlayerHitEffectAnimation;
};