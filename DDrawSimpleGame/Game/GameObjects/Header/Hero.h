#pragma once

#include "GameObject.h"

enum eAni_Tick_Hero : ULONGLONG
{
	ATH_DEFAULT_TICK = 200,
};

class ImageData;

class Hero : public CGameObject
{
public:
	Hero(int x = 0, int y = 0);
	Hero(
		std::shared_ptr<ImageData*[]> sptrAnimation, 
		size_t animationSizeint, 
		int HP, 
		int life, 
		int speed, 
		bool bCollisionable = true
	);
	virtual ~Hero();

	Hero(const Hero& other);
	Hero& operator=(Hero&& rhs) noexcept;

	void collision_handler(const CGameObject* other);
	virtual void ResetData(const CGameObject* sample) override;

private:
};