#pragma once

struct GameData_Lua;

GameData_Lua* g_pLuaGameData = nullptr;

struct GameData_Lua
{
	using tick = unsigned int;

	// Tick.lua
	tick tickEnemyMoveDelay;			  // ���� �̵��ϴ� ������
	tick tickEnemyDeathDelay;			  // ���� �� ���� ó�� ������
	tick tickEnemyShootDelay;			  // �Ѿ� �߻� ������

	// Score.lua
	unsigned int iMaxScore;				  // ���� �� �ִ� �ִ� ����
	unsigned int iScorePerEnemyKill;	  // 1ų �� ��� ���ھ�
	unsigned int iLossWhenHitByEnemy;	  // ���� �ε����� �� �Ҵ� ���ھ�
	unsigned int iLossWhenHitByEnemyAmmo; // ���� �Ѿ˿� �¾��� �� �Ҵ� ���ھ�

	// Animation.lua
	tick tickPlayerDefaultAni;			  // �÷��̾��� �⺻ �ִϸ��̼� tick
	tick tickPlayerAmmoHitEffectAni;	  // �÷��̾� �Ѿ� hit ����Ʈ tick

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