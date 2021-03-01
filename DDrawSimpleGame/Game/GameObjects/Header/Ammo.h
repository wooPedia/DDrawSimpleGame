#pragma once

#include "GameObject.h"
#include "../../Components/Header/LifeStateComponent.h"

class ImageData;

class Ammo : public CGameObject
{
public:
	Ammo(int x = 0, int y = 0);
	Ammo(
		std::shared_ptr<ImageData> sptrImageData,
		int power,
		int speed,
		bool bCollisionable = true,
		eState eState = eState::STATE_VALID
	);
	virtual ~Ammo();

	Ammo(const Ammo& other);
	Ammo& operator=(Ammo&& rhs) noexcept;

	void collision_handler(const CGameObject* other);

	virtual void ResetData(const CGameObject* sample) override;

private:
};