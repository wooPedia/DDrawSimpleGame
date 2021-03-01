#pragma once

#include "GameObject.h"
#include "../../Components/Header/LifeStateComponent.h"

enum eAni_Tick_Effect : ULONGLONG
{
	ATE_DEFAULT_TICK = 60,
};

class ImageData;

class HitEffect : public CGameObject
{
public:
	HitEffect(int x = 0, int y = 0);
	HitEffect(
		std::shared_ptr<ImageData*[]> sptrAnimation,
		size_t animationSize,
		eState eState = eState::STATE_VALID
	);
	virtual ~HitEffect();

	HitEffect(const HitEffect& other);
	HitEffect& operator=(HitEffect&& rhs) noexcept;

	virtual void ResetData(const CGameObject* sample) override;
};