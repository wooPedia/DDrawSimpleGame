#pragma once

#include "GameObject.h"

class ImageData;

class AirCraft : public CGameObject
{
public:
	AirCraft(int x = 0, int y = 0);
	AirCraft(
		std::shared_ptr<ImageData> sptrImageData,
		int HP,
		int life,
		int power,
		int speed,
		bool bCollisionable = true
	);
	virtual ~AirCraft();

	AirCraft(const AirCraft& other);
	AirCraft& operator=(AirCraft&& rhs) noexcept;

	void collision_handler(const CGameObject* other);

	virtual void ResetData(const CGameObject* sample) override;

protected:
};