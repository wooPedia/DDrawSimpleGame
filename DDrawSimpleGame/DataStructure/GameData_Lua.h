#pragma once

struct GameData_Lua;

GameData_Lua* g_pLuaGameData = nullptr;

struct GameData_Lua
{
	using tick = unsigned int;

	// Tick.lua
	tick tickEnemyMoveDelay;			  // 적이 이동하는 딜레이
	tick tickEnemyDeathDelay;			  // 죽은 적 삭제 처리 딜레이
	tick tickEnemyShootDelay;			  // 총알 발사 딜레이

	// Score.lua
	unsigned int iMaxScore;				  // 얻을 수 있는 최대 점수
	unsigned int iScorePerEnemyKill;	  // 1킬 당 얻는 스코어
	unsigned int iLossWhenHitByEnemy;	  // 적과 부딪혔을 때 잃는 스코어
	unsigned int iLossWhenHitByEnemyAmmo; // 적의 총알에 맞았을 때 잃는 스코어

	// Animation.lua
	tick tickPlayerDefaultAni;			  // 플레이어의 기본 애니메이션 tick
	tick tickPlayerAmmoHitEffectAni;	  // 플레이어 총알 hit 이펙트 tick

	// ObjectInfo.lua
	int iPlayerHP;
	int iPlayerLife;
	int iPlayerSpeed;
	int iPlayerMaxAmmoSize;
		
	int iPlayerAmmoPower;
	int iPlayerAmmoSpeed;
		
	int iMaxEnemySize;
	int iEnemyHP;
	int iEnemyLife;
	int iEnemySpeed;
	int iEnemyMaxAmmoSize;
	int iEnemyPower;
		
	int iEnemyAmmoPower;
	int iEnemyAmmoSpeed;
};