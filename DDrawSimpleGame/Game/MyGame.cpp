#include "../pch/stdafx.h"
#include <random>

#include "MyGame.h"
#include "../MyDDraw/Header/MyDDraw.h"

#include "Components/Header/AnimationComponent.h"
#include "Components/Header/RenderComponent.h"
#include "Components/Header/PhysicsComponent.h"
#include "Components/Header/PowerComponent.h"
#include "Components/Header/MoveComponent.h"
#include "Components/Header/HealthComponent.h"
#include "Components/Header/LifeStateComponent.h"

#include "GameObjects/Header/GameObject.h"
#include "GameObjects/Header/Hero.h"
#include "GameObjects/Header/AirCraft.h"
#include "GameObjects/Header/Ammo.h"
#include "GameObjects/Header/HitEffect.h"

#include "GameObjectManager/SingleObjectManager.h"
#include "GameObjectManager/MultipleObjectManager.h"

#include "ObjectPool/ObjectPool.h"
#include "ScoreManager/Header/ScoreManager.h"

#include "../DataStructure/game_setting.h"
#include "../DataStructure/GameData_Lua.h"
#include "../DataStructure/typedef.h"

#include "../Util/Header/MyLua.h"
#include "../Util/Header/TGAImage.h"

using namespace std;

extern MyDDraw* g_pDDraw;
extern GameData_Lua* g_pLuaGameData;

////////////////////////////////////////////////////////////
// Name: MyGame()
// Desc: ������
////////////////////////////////////////////////////////////
MyGame::MyGame(HWND hWnd)
	: m_hWnd(hWnd)
	, m_dwGameFPS(60)
	, m_dwCurFPS(0)
	, m_fTicksPerGameFrame(1000.0f / (float)m_dwGameFPS) /* 16.7ms */
	, m_prvGameFrameTick(GetTickCount64())
	, m_prvCounter(QCGetCounter())
	, m_bKeyDown_Up(FALSE)
	, m_bKeyDown_Down(FALSE)
	, m_bKeyDown_Left(FALSE)
	, m_bKeyDown_Right(FALSE)
	, m_dwSurfaceWidth(0)
	, m_dwSurfaceHeight(0)
	, m_dwBackColor(static_cast<DWORD>(eColor::COL_BLACK))
	, m_pPlayerMng(nullptr)
	, m_pEnemyMng(nullptr)
	, m_pPlayerAmmoMng(nullptr)
	, m_pEnemyAmmoMng(nullptr)
	, m_pPlayerAmmoHitEffect(nullptr)
{
}

////////////////////////////////////////////////////////////
// Name: ~MyGame()
// Desc: �Ҹ���
////////////////////////////////////////////////////////////
MyGame::~MyGame()
{
	Cleanup();
}

////////////////////////////////////////////////////////////
// Name: Initialize()
// Desc: DDraw �� ���ӿ� �ʿ��� �����͸� �ʱ�ȭ�մϴ�.
////////////////////////////////////////////////////////////
BOOL MyGame::Initialize()
{
	if (m_hWnd == nullptr || g_pDDraw == nullptr)
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		return FALSE;
	}

	// DDraw �ʱ�ȭ
	if (FALSE == g_pDDraw->InitDDraw(m_hWnd))
	{
		return FALSE;
	}

	// ȭ�� ũ�� get
	m_dwSurfaceWidth  = g_pDDraw->GetSurfaceWidth();
	m_dwSurfaceHeight = g_pDDraw->GetSurfaceHeight();

	g_pLuaGameData = new GameData_Lua;

	// Lua���� ������ ��������
	GetGameData_Lua();

	// �̹��� ������ ����
	InitImageData(); 

	// ������Ʈ �Ŵ��� �ʱ�ȭ
	InitObjectManagers();

	// ���� ���� ��ü �ʱ�ȭ
	ScoreManager::GetInstance()->Initialize(g_pLuaGameData->iMaxScore);
	
	return TRUE;
}

////////////////////////////////////////////////////////////
// Name: Process()
// Desc: Idle Ÿ�ӿ� ����Ǵ� ���� ���μ����Դϴ�.
// �������� ������Ʈ�ϰ� ȭ���� �׸��ϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::Process()
{
	LARGE_INTEGER CurCounter = QCGetCounter();
	float fElpasedTime = QCMeasureElapsedTick(CurCounter, m_prvCounter); // ms����, 1000�ϰ�� 1��
	ULONGLONG CurTick = GetTickCount64();

	// 60FPS�Ƿ� 16.7ms���� 1�������� �׷��� �մϴ�.
	// ���� �� �������� �׸��� �ð��� 16.7ms ���� �����ٸ� ��ŵ�մϴ�.
	if (fElpasedTime > m_fTicksPerGameFrame /* == 16.7 */)
	{
		m_prvGameFrameTick = CurTick;
		m_prvCounter = CurCounter;

		OnFrameMove(fElpasedTime);
	}
	else
	{
		return;
	}

	DrawScene();
}

////////////////////////////////////////////////////////////
// Name: OnFrameMove()
// Desc: �����Ӹ��� ��ȭ�Ǵ� �����͸� ����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::OnFrameMove(float fElapsedTime)
{
	/*
		1. ������Ʈ �̵�
		2. �ִϸ��̼� ������Ʈ
		3. �浹 ó��
		4. DEAD ������ ������Ʈ ����
		5. ���ο� �� ���� �� �Ѿ� �߻�
	*/
	MoveObjects();
	UpdateAnimation();
	DetectCollision();
	DeleteDeadObjects();
	ProcessEnemy();
}

//////////////////////////////////////////////////////////////////
// Name: DrawScene()
// Desc: ������Ʈ�� �����͸� ������� ȭ�鿡 �� ������Ʈ�� �׸��ϴ�. 
//////////////////////////////////////////////////////////////////
VOID MyGame::DrawScene()
{
	if (TRUE == g_pDDraw->BeginDraw())
	{
		DrawBackground(0, 0, m_dwSurfaceWidth, m_dwSurfaceHeight, m_dwBackColor);

		// Draw
		m_pPlayerMng->DrawObjects();
		m_pPlayerAmmoMng->DrawObjects();
		m_pEnemyMng->DrawObjects();
		m_pEnemyAmmoMng->DrawObjects();
		m_pPlayerAmmoHitEffect->DrawObjects();

		g_pDDraw->EndDraw();

		// for FPS, score
		HDC hDC = nullptr;
		if (g_pDDraw->BeginGDI(&hDC))
		{
			g_pDDraw->DrawInfo(hDC);
			g_pDDraw->EndGDI(hDC);
		}

		// ����� ȭ�鿡 ����մϴ�.
		g_pDDraw->DisplayScene();
		g_pDDraw->CheckFPS();
	}
}

////////////////////////////////////////////////////////////
// Name: GetGameData_Lua()
// Desc: ��� ���Ͽ��� �̸� ������ ���� �����͸� �����ɴϴ�.
////////////////////////////////////////////////////////////
BOOL MyGame::GetGameData_Lua()
{
	CMyLua lua;

	if (g_pLuaGameData == nullptr)
		goto lb_false;

	// Tick.lua
	if (false == lua.Lua_LoadFile("LuaFiles/Tick.lua"))
		goto lb_false;

	if (lua.Lua_BeginTable("EnemyTick"))
	{
		lua.Lua_GetValueFromTable<unsigned int>("MoveDelayTick", &g_pLuaGameData->tickEnemyMoveDelay);
		lua.Lua_GetValueFromTable<unsigned int>("DeathDelayTick", &g_pLuaGameData->tickEnemyDeathDelay);
		lua.Lua_GetValueFromTable<unsigned int>("ShootDelayTick", &g_pLuaGameData->tickEnemyShootDelay);

		lua.Lua_EndTable();
	}

	// Score.lua
	if (false == lua.Lua_LoadFile("LuaFiles/Score.lua"))
		goto lb_false;
	
	lua.Lua_GetValue<unsigned int>("MaxScore", &g_pLuaGameData->iMaxScore);
	lua.Lua_GetValue<unsigned int>("ScorePerEnemyKill", &g_pLuaGameData->iScorePerEnemyKill);
	lua.Lua_GetValue<unsigned int>("ScoreWhenHitByEnemy", &g_pLuaGameData->iLossWhenHitByEnemy);
	lua.Lua_GetValue<unsigned int>("ScoreWhenHitByEnemyAmmo", &g_pLuaGameData->iLossWhenHitByEnemyAmmo);

	// Animation.lua
	if (false == lua.Lua_LoadFile("LuaFiles/Animation.lua"))
		goto lb_false;

	if (lua.Lua_BeginTable("PlayerAnimation"))
	{
		lua.Lua_GetValueFromTable<unsigned int>("DefaultTick", &g_pLuaGameData->tickPlayerDefaultAni);
		lua.Lua_EndTable();
	}

	if (lua.Lua_BeginTable("HitEffectAnimation"))
	{
		lua.Lua_GetValueFromTable<unsigned int>("PlayerDefaultAmmoTick", &g_pLuaGameData->tickPlayerAmmoHitEffectAni);
		lua.Lua_EndTable();
	}

	// ObjectInfo.lua
	if(false == lua.Lua_LoadFile("LuaFiles/ObjectInfo.lua"))
		goto lb_false;

	if (lua.Lua_BeginTable("Player"))
	{
		lua.Lua_GetValueFromTable<int>("HP", &g_pLuaGameData->iPlayerHP);
		lua.Lua_GetValueFromTable<int>("Life", &g_pLuaGameData->iPlayerLife);
		lua.Lua_GetValueFromTable<int>("Speed", &g_pLuaGameData->iPlayerSpeed);
		lua.Lua_GetValueFromTable<int>("MaxAmmoSize", &g_pLuaGameData->iPlayerMaxAmmoSize);

		lua.Lua_EndTable();
	}

	if (lua.Lua_BeginTable("PlayerDefaultAmmo"))
	{
		lua.Lua_GetValueFromTable<int>("Power", &g_pLuaGameData->iPlayerAmmoPower);
		lua.Lua_GetValueFromTable<int>("Speed", &g_pLuaGameData->iPlayerAmmoSpeed);

		lua.Lua_EndTable();
	}

	lua.Lua_GetValue<int>("MaxEnemySize", &g_pLuaGameData->iMaxEnemySize);
	if (lua.Lua_BeginTable("Enemy"))
	{
		lua.Lua_GetValueFromTable<int>("HP", &g_pLuaGameData->iEnemyHP);
		lua.Lua_GetValueFromTable<int>("Life", &g_pLuaGameData->iEnemyLife);
		lua.Lua_GetValueFromTable<int>("Speed", &g_pLuaGameData->iEnemySpeed);
		lua.Lua_GetValueFromTable<int>("MaxAmmoSize", &g_pLuaGameData->iEnemyMaxAmmoSize);
		lua.Lua_GetValueFromTable<int>("Power", &g_pLuaGameData->iEnemyPower);

		lua.Lua_EndTable();
	}

	if (lua.Lua_BeginTable("EnemyDefaultAmmo"))
	{
		lua.Lua_GetValueFromTable<int>("Power", &g_pLuaGameData->iEnemyAmmoPower);
		lua.Lua_GetValueFromTable<int>("Speed", &g_pLuaGameData->iEnemyAmmoSpeed);

		lua.Lua_EndTable();
	}

	return true;

lb_false:

#ifdef _DEBUG
	__debugbreak(); 
#endif // _DEBUG
	return false;
}

////////////////////////////////////////////////////////////
// Name: InitObjectManagers()
// Desc: ������Ʈ �Ŵ��� ��ü���� �ʱ�ȭ�մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::InitObjectManagers()
{
	InitSingleObjectManagers();
	InitMultipleObjectManagers();
}

////////////////////////////////////////////////////////////
// Name: InitSingleObjectManagers()
// Desc: �̱� ������Ʈ �Ŵ��� ��ü���� �ʱ�ȭ�մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::InitSingleObjectManagers()
{
	m_pPlayerMng = new SingleObjectManager<Hero>();

	Hero heroSample(
		m_sptrPlayerAnimation,
		eAni_Hero::AH_HERO_NORMAL,
		g_pLuaGameData->iPlayerHP,
		g_pLuaGameData->iPlayerLife,
		g_pLuaGameData->iPlayerSpeed
	);

	m_pPlayerMng->RegisterObjectSample(std::move(heroSample));
	m_pPlayerMng->CreateObjectUsingSample();
}

////////////////////////////////////////////////////////////
// Name: InitMultipleObjectManagers()
// Desc: ��Ƽ�� ������Ʈ �Ŵ��� ��ü���� �ʱ�ȭ�մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::InitMultipleObjectManagers()
{
	/*
		1. ������Ʈ �Ŵ��� �޸� �Ҵ�
		2. ������Ʈ ���� ���
		3. ������Ʈ Ǯ ����(������ ������� Ǯ ����)
	*/

	size_t szMaxEnemyAmmoSize = (uint64_t)g_pLuaGameData->iEnemyMaxAmmoSize * (uint64_t)g_pLuaGameData->iMaxEnemySize;
	size_t szMaxHitEffectSize = (uint64_t)g_pLuaGameData->iEnemyMaxAmmoSize * (uint64_t)g_pLuaGameData->iMaxEnemySize;

	m_pPlayerAmmoMng = new MultipleObjectManager<Ammo>(g_pLuaGameData->iPlayerMaxAmmoSize);
	m_pEnemyMng = new MultipleObjectManager<AirCraft>(g_pLuaGameData->iMaxEnemySize);
	m_pEnemyAmmoMng = new MultipleObjectManager<Ammo>(szMaxEnemyAmmoSize);
	m_pPlayerAmmoHitEffect = new MultipleObjectManager<HitEffect>(szMaxHitEffectSize);

	Ammo playerAmmoSample(
		m_sptrPlayerAmmoImgData,
		g_pLuaGameData->iPlayerAmmoPower,
		g_pLuaGameData->iPlayerAmmoSpeed
	);

	AirCraft enemySample(
		m_sptrEnemyImgData,
		g_pLuaGameData->iEnemyHP,
		g_pLuaGameData->iEnemyLife,
		g_pLuaGameData->iEnemyPower,
		g_pLuaGameData->iEnemySpeed
	);

	Ammo enemyAmmoSample(
		m_sptrEnemyAmmoImgData,
		g_pLuaGameData->iEnemyAmmoPower,
		g_pLuaGameData->iEnemyAmmoSpeed
	);

	HitEffect playerAmmoHitEffectSample(
		m_sptrPlayerHitEffectAnimation,
		eAni_HitEffect::AHE_HERO_NORMAL
	);

	m_pPlayerAmmoMng->RegisterObjectSample(std::move(playerAmmoSample));
	m_pEnemyMng->RegisterObjectSample(std::move(enemySample));
	m_pEnemyAmmoMng->RegisterObjectSample(std::move(enemyAmmoSample));
	m_pPlayerAmmoHitEffect->RegisterObjectSample(std::move(playerAmmoHitEffectSample));

	m_pPlayerAmmoMng->CreateObjectPool();
	m_pEnemyMng->CreateObjectPool();
	m_pEnemyAmmoMng->CreateObjectPool();
	m_pPlayerAmmoHitEffect->CreateObjectPool();
}

////////////////////////////////////////////////////////////
// Name: DeleteObjectManagers()
// Desc: ������Ʈ �Ŵ��� ��ü���� �޸𸮸� �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DeleteObjectManagers()
{
	SAFE_DELETE(m_pPlayerMng);
	SAFE_DELETE(m_pPlayerAmmoMng);
	SAFE_DELETE(m_pEnemyMng);
	SAFE_DELETE(m_pEnemyAmmoMng);
	SAFE_DELETE(m_pPlayerAmmoHitEffect);
}

////////////////////////////////////////////////////////////
// Name: InitImageData()
// Desc: �̹��� ������ ��ü���� �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::InitImageData()
{
	DWORD colorKey = 0;
	const string PATH = "./image/";

	// �÷��̾� �ִϸ��̼� �̹��� ����
	std::shared_ptr<ImageData* []> ppPlayerImageDatas(
		new ImageData*[eAni_Hero::AH_HERO_NORMAL], [](ImageData** pp) {
			for (size_t i = 0; i < eAni_Hero::AH_HERO_NORMAL; ++i) {
				delete pp[i];
			}
			delete[] pp;
		});

	for (size_t i = 0; i < eAni_Hero::AH_HERO_NORMAL; ++i)
	{
		unique_ptr<TGAImage> uptrPlayerTGA = make_unique<TGAImage>();
		if (TRUE == uptrPlayerTGA->Load24BitsTGA(string(PATH + "magaki_" + to_string(i) + ".tga").c_str(), 4))
		{
			ppPlayerImageDatas[i] = new ImageData;
			colorKey = uptrPlayerTGA->GetPixel(0, 0);
			if (FALSE == ppPlayerImageDatas[i]->Create(uptrPlayerTGA->GetRawImage(), uptrPlayerTGA->GetWidth(), uptrPlayerTGA->GetHeight(), colorKey))
			{
				MessageBoxA(NULL, "Failed to create a player image!", "Error", MB_OK);
				exit(-1);
			}
		}
	}
	m_sptrPlayerAnimation = ppPlayerImageDatas;

	// �÷��̾� �Ѿ� Ÿ�� ����Ʈ �̹��� ����
	std::shared_ptr<ImageData* []> ppHitEffectImageDatas(
		new ImageData * [eAni_HitEffect::AHE_HERO_NORMAL], [](ImageData** pp) {
			for (size_t i = 0; i < eAni_HitEffect::AHE_HERO_NORMAL; ++i) {
				delete pp[i];
			}
			delete[] pp;
		});

	for (size_t i = 0; i < eAni_HitEffect::AHE_HERO_NORMAL; ++i)
	{
		unique_ptr<TGAImage> uptrHitEffectTGA = make_unique<TGAImage>();
		if (TRUE == uptrHitEffectTGA->Load24BitsTGA(string(PATH + "magaki_ammo_hit_" + to_string(i) + ".tga").c_str(), 4))
		{
			colorKey = uptrHitEffectTGA->GetPixel(0, 0);
			ppHitEffectImageDatas[i] = new ImageData();
			if (FALSE == ppHitEffectImageDatas[i]->Create(uptrHitEffectTGA->GetRawImage(), uptrHitEffectTGA->GetWidth(), uptrHitEffectTGA->GetHeight(), colorKey))
			{
				MessageBoxA(NULL, "Failed to create a player image!", "Error", MB_OK);
				exit(-1);
			}
		}
	} 
	m_sptrPlayerHitEffectAnimation = ppHitEffectImageDatas;

	// �÷��̾� �Ѿ� �̹��� ����
	unique_ptr<TGAImage> pAmmoTGA = make_unique<TGAImage>();
	if (TRUE == pAmmoTGA->Load24BitsTGA("./image/magaki_ammo_0.tga", 4))
	{
		colorKey = pAmmoTGA->GetPixel(0, 0);
		shared_ptr<ImageData> sptrPlayerAmmoImgData = make_shared<ImageData>();
		if (FALSE == sptrPlayerAmmoImgData->Create(pAmmoTGA->GetRawImage(), pAmmoTGA->GetWidth(), pAmmoTGA->GetHeight(), colorKey))
		{
			MessageBoxA(NULL, "Failed to create a player's ammo image!", "Error", MB_OK);
			exit(-1);
		}
		m_sptrPlayerAmmoImgData = sptrPlayerAmmoImgData;
	}

	// �� �̹��� ����
	unique_ptr<TGAImage> pEnemyTGA = make_unique<TGAImage>();
	if (TRUE == pEnemyTGA->Load24BitsTGA("./image/green_enemy.tga", 4))
	{
		//DWORD colorKey = pEnemyTGA->GetPixel(5, 0);
		colorKey = pEnemyTGA->GetPixel(0, 0);
		shared_ptr<ImageData> sptrEnemyImgData = make_shared<ImageData>();
		if (FALSE == sptrEnemyImgData->Create(pEnemyTGA->GetRawImage(), pEnemyTGA->GetWidth(), pEnemyTGA->GetHeight(), colorKey))
		{
			MessageBoxA(NULL, "Failed to create a enemy image!", "Error", MB_OK);
			exit(-1);
		}
		m_sptrEnemyImgData = sptrEnemyImgData;
	}

	// �� �Ѿ� �̹��� ����
	unique_ptr<TGAImage> pEnemyAmmoTGA = make_unique<TGAImage>();
	if (TRUE == pEnemyAmmoTGA->Load24BitsTGA("./image/enemy_ammo2.tga", 4))
	{
		colorKey = pEnemyAmmoTGA->GetPixel(0, 0);
		shared_ptr<ImageData> sptrEnemyAmmoImgData = make_shared<ImageData>();
		if (FALSE == sptrEnemyAmmoImgData->Create(pEnemyAmmoTGA->GetRawImage(), pEnemyAmmoTGA->GetWidth(), pEnemyAmmoTGA->GetHeight(), colorKey))
		{
			MessageBoxA(NULL, "Failed to create a enemy's ammo image!", "Error", MB_OK);
			exit(-1);
		}
		m_sptrEnemyAmmoImgData = sptrEnemyAmmoImgData;
	}
}

////////////////////////////////////////////////////////////
// Name: MoveObjects()
// Desc: �� ��ü�� �̵���ŵ�ϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::MoveObjects()
{
	MovePlayer();
	MoveAmmo();
	MoveEnemy();
}

////////////////////////////////////////////////////////////
// Name: UpdateAnimation()
// Desc: �ִϸ��̼��� ������Ʈ�մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::UpdateAnimation()
{
	auto itPlayer = m_pPlayerMng->GetBegin();
	if (false == (*itPlayer)->GetHealthComp()->IsDead())
	{
		(*itPlayer)->GetAnimationComp()->UpdateAnimation();
	}
	
	auto itHitEffect = m_pPlayerAmmoHitEffect->GetBegin();
	while (itHitEffect != m_pPlayerAmmoHitEffect->GetEnd())
	{
		if (false == (*itHitEffect)->GetLifeStateComp()->IsExpired())
		{
			(*itHitEffect)->GetAnimationComp()->UpdateAnimation(false);
		}
		++itHitEffect;
	}
}

////////////////////////////////////////////////////////////
// Name: DetectCollision()
// Desc: ��ü ���� �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DetectCollision()
{
	DetectPlayerCollision(); 
	DetectEnemyCollision();  
}

////////////////////////////////////////////////////////////
// Name: DeleteDeadObjects()
// Desc: �Ҹ� ������ ���� ������ ���� ó���մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DeleteDeadObjects()
{
	//m_pPlayerMng->DeleteDeadObjects();
	m_pPlayerAmmoMng->DeleteDeadObjects();
	m_pEnemyMng->DeleteDeadObjects();
	m_pEnemyAmmoMng->DeleteDeadObjects();
	m_pPlayerAmmoHitEffect->DeleteDeadObjects();
}

////////////////////////////////////////////////////////////
// Name: DetectPlayerCollision()
// Desc: �÷��̾�� ���õ� �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DetectPlayerCollision()
{
	DetectCollisionPlayerVsAmmo();
	DetectCollisionPlayerVsEnemy();
	DetectCollisionPlayerAmmoVsWall();
}

////////////////////////////////////////////////////////////
// Name: DetectCollisionPlayerVsAmmo()
// Desc: �÷��̾�� �� �Ѿ� ���� �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DetectCollisionPlayerVsAmmo()
{
	auto itPlayer = m_pPlayerMng->GetBegin();
	auto itEnemyAmmo = m_pEnemyAmmoMng->GetBegin();
	while (itEnemyAmmo != m_pEnemyAmmoMng->GetEnd())
	{
		if (false == (*itEnemyAmmo)->GetLifeStateComp()->IsExpired())
		{
			const Hero* pPlayer = itPlayer->get();
			const Ammo* pEnemyAmmo = itEnemyAmmo->get();

			if(IsCollisionObjects(*pPlayer, *pEnemyAmmo))
			{
				(*itPlayer)->GetPhysicsComp()->NotifyHit(pEnemyAmmo);
				(*itEnemyAmmo)->GetPhysicsComp()->NotifyHit(pPlayer);

				ScoreManager::GetInstance()->LostScore(g_pLuaGameData->iLossWhenHitByEnemyAmmo);
			}
		}
		++itEnemyAmmo;
	}
}

////////////////////////////////////////////////////////////
// Name: DetectCollisionPlayerVsEnemy()
// Desc: �÷��̾�� �� ���� �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DetectCollisionPlayerVsEnemy()
{
	auto itPlayer = m_pPlayerMng->GetBegin();
	auto itEnemy = m_pEnemyMng->GetBegin();
	while (itEnemy != m_pEnemyMng->GetEnd())
	{
		if (false == (*itEnemy)->GetHealthComp()->IsDead())
		{
			const Hero* pPlayer = itPlayer->get();
			const AirCraft* pEnemy = itEnemy->get();
			if (IsCollisionObjects(*pPlayer, *pEnemy))
			{
				(*itPlayer)->GetPhysicsComp()->NotifyHit(pEnemy);
				ScoreManager::GetInstance()->LostScore(g_pLuaGameData->iLossWhenHitByEnemy);
			}
		}
		++itEnemy;
	}
}

////////////////////////////////////////////////////////////
// Name: DetectCollisionPlayerAmmoVsWall()
// Desc: �÷��̾� �Ѿ˰� ������ �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DetectCollisionPlayerAmmoVsWall()
{
	INT_VECTOR2 ivAmmoPos = {};

	auto itAmmo = m_pPlayerAmmoMng->GetBegin();
	while (itAmmo != m_pPlayerAmmoMng->GetEnd())
	{
		if (!(*itAmmo)->GetLifeStateComp()->IsExpired())
		{
			(*itAmmo)->GetPos(&ivAmmoPos);
			int ammoPosEnd = ivAmmoPos.x + (int)((*itAmmo)->GetWidth());
			if (ammoPosEnd > (int)m_dwSurfaceWidth)
			{
				Ammo* pAmmo = (*itAmmo).get();
				(*itAmmo)->GetPhysicsComp()->NotifyHit(pAmmo); // �ڱ� �ڽ��� ������ ��� ���� ���� �ɷ�..
			}
		}
		++itAmmo;
	}
}

////////////////////////////////////////////////////////////
// Name: DetectEnemyCollision()
// Desc: ���� ���õ� �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DetectEnemyCollision()
{
	DetectCollisionEnemyVsAmmo();
	DetectCollisionEnemyVsWall();
	DetectCollisionEnemyAmmoVsWall();
}

////////////////////////////////////////////////////////////
// Name: DetectCollisionEnemyVsAmmo()
// Desc: ���� �÷��̾� �Ѿ��� �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DetectCollisionEnemyVsAmmo()
{
	// ���� �Ѿ��� �浹 �˻�
	auto itEnemy = m_pEnemyMng->GetBegin();
	while (itEnemy != m_pEnemyMng->GetEnd())
	{
		if (false == (*itEnemy)->GetHealthComp()->IsDead())
		{
			auto itPlayerAmmo = m_pPlayerAmmoMng->GetBegin();
			while (itPlayerAmmo != m_pPlayerAmmoMng->GetEnd())
			{
				if (false == (*itPlayerAmmo)->GetLifeStateComp()->IsExpired())
				{
					const AirCraft* pEnemy = itEnemy->get();
					const Ammo* pPlayerAmmo = itPlayerAmmo->get();
					if (IsCollisionObjects(*pEnemy, *pPlayerAmmo))
					{
						(*itEnemy)->GetPhysicsComp()->NotifyHit(pPlayerAmmo);
						(*itPlayerAmmo)->GetPhysicsComp()->NotifyHit(pEnemy);
						
						// �Ѿ� hit ����Ʈ ����
						INT_VECTOR2 ivEnemytPos = {};
						pEnemy->GetPos(&ivEnemytPos);

						m_pPlayerAmmoHitEffect->CreateObject(ivEnemytPos.x, ivEnemytPos.y);
					}
				}
				++itPlayerAmmo;
			}
		}
		++itEnemy;
	}
}

////////////////////////////////////////////////////////////
// Name: DetectCollisionEnemyVsWall()
// Desc: ���� ������ �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DetectCollisionEnemyVsWall()
{
	// ���� ���� �浹 �˻�
	INT_VECTOR2 ivEnemyPos = {};
	auto itEnemy = m_pEnemyMng->GetBegin();
	while (itEnemy != m_pEnemyMng->GetEnd())
	{
		if (!(*itEnemy)->GetHealthComp()->IsDead())
		{
			(*itEnemy)->GetPos(&ivEnemyPos);
			if (ivEnemyPos.x < 0)
			{
				AirCraft* pEnemy = (*itEnemy).get();
				(*itEnemy)->GetPhysicsComp()->NotifyHit(pEnemy);
			}
		}
		++itEnemy;
	}
}

////////////////////////////////////////////////////////////
// Name: DetectCollisionEnemyAmmoVsWall()
// Desc: �� �Ѿ˰� ������ �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
VOID MyGame::DetectCollisionEnemyAmmoVsWall()
{
	INT_VECTOR2 ivAmmoPos = {};

	auto itAmmo = m_pEnemyAmmoMng->GetBegin();
	while (itAmmo != m_pEnemyAmmoMng->GetEnd())
	{
		if (false == (*itAmmo)->GetLifeStateComp()->IsExpired())
		{
			(*itAmmo)->GetPos(&ivAmmoPos);
			if (ivAmmoPos.x < 0)
			{
				Ammo* pAmmo = (*itAmmo).get();
				(*itAmmo)->GetPhysicsComp()->NotifyHit(pAmmo);
			}
		}
		++itAmmo;
	}
}

////////////////////////////////////////////////////////////
// Name: IsCollisionObjects()
// Desc: �� ��ü ���� �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
BOOL MyGame::IsCollisionObjects(const CGameObject& obj1, const CGameObject& obj2)
{
	INT_VECTOR2 obj1_ivPos = {};
	INT_VECTOR2 obj2_ivPos = {};
	
	obj1.GetPos(&obj1_ivPos);
	obj2.GetPos(&obj2_ivPos);

	INT_RECT2 obj1_rect =
	{
		{ obj1_ivPos.x, obj1_ivPos.y },
		{ obj1_ivPos.x + (int)obj1.GetWidth(), obj1_ivPos.y + (int)obj1.GetHeight() }
	};

	INT_RECT2 obj2_rect =
	{
		{ obj2_ivPos.x, obj2_ivPos.y },
		{ obj2_ivPos.x + (int)obj2.GetWidth(), obj2_ivPos.y + (int)obj2.GetHeight() }
	};

	return IsCollisionRectVsRect(obj1_rect, obj2_rect);
}

////////////////////////////////////////////////////////////
// Name: IsCollisionRectVsRect()
// Desc: �� �簢�� ���� �浹 �˻縦 �����մϴ�.
////////////////////////////////////////////////////////////
BOOL MyGame::IsCollisionRectVsRect(const INT_RECT2& rect1, const INT_RECT2& rect2)
{
	const INT_VECTOR2 ivMax1 = rect1.ivMax;
	const INT_VECTOR2 ivMin1 = rect1.ivMin;
	const INT_VECTOR2 ivMax2 = rect2.ivMax;
	const INT_VECTOR2 ivMin2 = rect2.ivMin;

	if (ivMax1.x < ivMin2.x || ivMax1.y < ivMin2.y || ivMin1.y > ivMax2.y || ivMin1.x > ivMax2.x)
	{
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////
// Name: ProcessEnemy()
// Desc: ������ �ֱ����� �Ѿ� �߻� �� ���ο� ���⸦ �����մϴ�. 
////////////////////////////////////////////////////////////
VOID MyGame::ProcessEnemy()
{
	ShootMissilePerTick();
	FillNewEnemy();
}

////////////////////////////////////////////////////////////
// Name: MovePlayer()
// Desc: �÷��̾ �̵���ŵ�ϴ�. 
////////////////////////////////////////////////////////////
VOID MyGame::MovePlayer()
{
	auto itPlayer = m_pPlayerMng->GetBegin();
	INT_VECTOR2 ivCurPos;
	(*itPlayer)->GetPos(&ivCurPos);
	
	if (m_bKeyDown_Right)
	{
		(*itPlayer)->GetMoveComp()->MoveRight(&ivCurPos);
	}
	if (m_bKeyDown_Left)
	{
		(*itPlayer)->GetMoveComp()->MoveLeft(&ivCurPos);
	}
	if (m_bKeyDown_Up)
	{
		(*itPlayer)->GetMoveComp()->MoveUp(&ivCurPos);
	}
	if (m_bKeyDown_Down)
	{
		(*itPlayer)->GetMoveComp()->MoveDown(&ivCurPos);
	}

	(*itPlayer)->SetPos(ivCurPos);
}

////////////////////////////////////////////////////////////
// Name: MoveAmmo()
// Desc: �Ѿ��� �̵���ŵ�ϴ�. 
////////////////////////////////////////////////////////////
VOID MyGame::MoveAmmo()
{
	// �÷��̾� �Ѿ��̵�
	auto itPlayerAmmo = m_pPlayerAmmoMng->GetBegin();
	while (itPlayerAmmo != m_pPlayerAmmoMng->GetEnd())
	{
		INT_VECTOR2 ivCurPos;

		(*itPlayerAmmo)->GetPos(&ivCurPos);
		(*itPlayerAmmo)->GetMoveComp()->MoveRight(&ivCurPos);
		(*itPlayerAmmo)->SetPos(ivCurPos);

		++itPlayerAmmo;
	}

	// �� �Ѿ� �̵�
	auto itEnemyAmmo = m_pEnemyAmmoMng->GetBegin();
	while (itEnemyAmmo != m_pEnemyAmmoMng->GetEnd())
	{
		INT_VECTOR2 ivCurPos;

		(*itEnemyAmmo)->GetPos(&ivCurPos);
		(*itEnemyAmmo)->GetMoveComp()->MoveLeft(&ivCurPos);
		(*itEnemyAmmo)->SetPos(ivCurPos);

		++itEnemyAmmo;
	}
}

////////////////////////////////////////////////////////////
// Name: MoveEnemy()
// Desc: ���⸦ �̵���ŵ�ϴ�. 
////////////////////////////////////////////////////////////
VOID MyGame::MoveEnemy()
{
	static ULONGLONG PreEnemyMoveTick = 0;
	ULONGLONG CurTick = GetTickCount64();

	// ���� �����Ӱ��� Tick ���̰� ������ ���� ������ ���� �������� �ʽ��ϴ�.
	if (CurTick - PreEnemyMoveTick < (ULONGLONG)g_pLuaGameData->tickEnemyMoveDelay)
	{
		return;
	}

	PreEnemyMoveTick = CurTick;

	auto itEnemy = m_pEnemyMng->GetBegin();
	while (itEnemy != m_pEnemyMng->GetEnd())
	{
		INT_VECTOR2 ivCurPos;

		(*itEnemy)->GetPos(&ivCurPos);
		(*itEnemy)->GetMoveComp()->MoveLeft(&ivCurPos);
		(*itEnemy)->SetPos(ivCurPos);

		++itEnemy;
	}
}

////////////////////////////////////////////////////////////
// Name: FillNewEnemy()
// Desc: ���� ������ �� ��ŭ ���⸦ �����մϴ�. 
////////////////////////////////////////////////////////////
VOID MyGame::FillNewEnemy()
{
	int newEnemyNum = g_pLuaGameData->iMaxEnemySize - (int)m_pEnemyMng->GetCurObjectNum();
	assert(newEnemyNum >= 0);

	// ���� ������ y ��ǥ�� �������� �����մϴ�.
	/*
		Width/Height ���� ������ ����
	*/
	INT_VECTOR2 ivEnemyPos = { (int)m_dwSurfaceWidth - (int)(m_pEnemyMng->GetWidth()), 0 };

	static random_device rng;
	static uniform_int_distribution<int> dist1(0, (int)m_dwSurfaceHeight - (int)m_pEnemyMng->GetHeight());

	for (int i = 0; i < newEnemyNum; ++i)
	{
		ivEnemyPos.y = dist1(rng);
		auto itNewEnemy = m_pEnemyMng->CreateObject(ivEnemyPos.x, ivEnemyPos.y);
		ShootMissileFromEnemy(itNewEnemy->get());
	}
}

//////////////////////////////////////////////////////////////////
// Name: DrawBackground()
// Desc: ��׶��带 �׸��ϴ�. 
//////////////////////////////////////////////////////////////////
VOID MyGame::DrawBackground(int x, int y, DWORD dwWidth, DWORD dwHeight, DWORD dwColor)
{
	g_pDDraw->DrawSomething(&x, &y, dwWidth, dwHeight, dwColor);
}

//////////////////////////////////////////////////////////////////
// Name: Cleanup()
// Desc: MyGame Ŭ���� ���� �����͸� �����մϴ�. 
//////////////////////////////////////////////////////////////////
VOID MyGame::Cleanup()
{
	DeleteObjectManagers();
	SAFE_DELETE(g_pLuaGameData);
}

//////////////////////////////////////////////////////////////////
// Name: ShootMissile()
// Desc: �÷��̾��� �Ѿ��� ������ ��ġ�� �����մϴ�.
//////////////////////////////////////////////////////////////////
VOID MyGame::ShootMissile()
{
	if (m_pPlayerAmmoMng->GetCurObjectNum() >= g_pLuaGameData->iPlayerMaxAmmoSize)
	{
		return;
	}
	
	INT_VECTOR2 ivPlayerAmmoPos = {};
	auto itPlayer = m_pPlayerMng->GetBegin();
	(*itPlayer)->GetPos(&ivPlayerAmmoPos);

	ivPlayerAmmoPos.x += (*itPlayer)->GetWidth() + 20;
	ivPlayerAmmoPos.y += (*itPlayer)->GetHeight() / 3;

	auto itPlayerAmmo = m_pPlayerAmmoMng->CreateObject(ivPlayerAmmoPos.x, ivPlayerAmmoPos.y);

	if (itPlayerAmmo == m_pPlayerAmmoMng->GetEnd())
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		return;
	}
}

//////////////////////////////////////////////////////////////////
// Name: ShootMissileFromEnemy()
// Desc: ������ �Ѿ��� ������ ��ġ�� �����մϴ�.
//////////////////////////////////////////////////////////////////
VOID MyGame::ShootMissileFromEnemy(std::list<std::unique_ptr<CGameObject>>::iterator itEnemy)
{
	// itEnemy�� ��� �Ѿ� ����
	INT_VECTOR2 ivEnemyAmmoPos = {};
	(*itEnemy)->GetPos(&ivEnemyAmmoPos);

	//ivEnemyAmmoPos.x -= 20;
	ivEnemyAmmoPos.y += (*itEnemy)->GetHeight() / 2;

	auto itCreatedAmmo = m_pEnemyAmmoMng->CreateObject(ivEnemyAmmoPos.x, ivEnemyAmmoPos.y);

	if (itCreatedAmmo == m_pEnemyAmmoMng->GetEnd())
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		return;
	}
}

//////////////////////////////////////////////////////////////////
// Name: ShootMissileFromEnemy()
// Desc: ������ �Ѿ��� ������ ��ġ�� �����մϴ�.
//////////////////////////////////////////////////////////////////
VOID MyGame::ShootMissileFromEnemy(const AirCraft* pEnemy)
{
	// Enemy�� ��� �Ѿ� ����
	INT_VECTOR2 ivEnemyAmmoPos = {};
	pEnemy->GetPos(&ivEnemyAmmoPos);

	//ivEnemyAmmoPos.x -= 20;
	ivEnemyAmmoPos.y += pEnemy->GetHeight() / 2;

	m_pEnemyAmmoMng->CreateObject(ivEnemyAmmoPos.x, ivEnemyAmmoPos.y);
}

//////////////////////////////////////////////////////////////////
// Name: ShootMissilePerTick()
// Desc: �����κ��� �ֱ������� �߻�Ǵ� �Ѿ��� �����մϴ�.
//////////////////////////////////////////////////////////////////
VOID MyGame::ShootMissilePerTick()
{
	// Ư�� Tick���� �Ѿ��� �߻��մϴ�.
	static ULONGLONG PreEnemyShootTick = 0;
	ULONGLONG CurTick = GetTickCount64();

	if (CurTick - PreEnemyShootTick < g_pLuaGameData->tickEnemyShootDelay)
	{
		return;
	}

	PreEnemyShootTick = CurTick;

	auto itEnemy = m_pEnemyMng->GetBegin();
	while (itEnemy != m_pEnemyMng->GetEnd())
	{
		ShootMissileFromEnemy(itEnemy->get());
		++itEnemy;
	}
}

//////////////////////////////////////////////////////////////////
// Name: OnUpdateWindowPos()
// Desc: ������ ������ �����մϴ�.
//////////////////////////////////////////////////////////////////
VOID MyGame::OnUpdateWindowPos()
{
	if (g_pDDraw)
	{
		g_pDDraw->OnUpdateWindowPos();
	}
}

//////////////////////////////////////////////////////////////////
// Name: OnKeyDown()
// Desc: ����Ű �� ���� Ű�� ���� ������ �����մϴ�.
//////////////////////////////////////////////////////////////////
VOID MyGame::OnKeyDown(UINT vKey, UINT scanCode)
{
	switch (vKey)
	{
		case VK_LEFT:
		{
			m_bKeyDown_Left = TRUE;
			break;
		}

		case VK_RIGHT:
		{
			m_bKeyDown_Right = TRUE;
			break;
		}

		case VK_UP:
		{
			m_bKeyDown_Up = TRUE;
			break;
		}

		case VK_DOWN:
		{
			m_bKeyDown_Down = TRUE;
			break;
		}

		case VK_SPACE:
		{
			ShootMissile();
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////
// Name: OnKeyUp()
// Desc: ����Ű �� ���� Ű�� ���� ������ �����մϴ�.
//////////////////////////////////////////////////////////////////
VOID MyGame::OnKeyUp(UINT vKey, UINT scanCode)
{
	switch (vKey)
	{
		case VK_LEFT:
		{
			m_bKeyDown_Left = FALSE;
			break;
		}

		case VK_RIGHT:
		{
			m_bKeyDown_Right = FALSE;
			break;
		}

		case VK_UP:
		{
			m_bKeyDown_Up = FALSE;
			break;
		}

		case VK_DOWN:
		{
			m_bKeyDown_Down = FALSE;
			break;
		}
	}
}