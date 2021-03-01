#include "../Header/HitEffect.h"

#include "../../Components/Header/AnimationComponent.h"
#include "../../../Util/Header/ImageData.h"
#include "../../../DataStructure/game_setting.h"

#include <memory>

HitEffect::HitEffect(int x /* = 0 */, int y /* = 0 */)
	: CGameObject(x, y)
{
}

HitEffect::HitEffect(std::shared_ptr<ImageData* []> sptrAnimation, size_t animationSize, eState eState)
	: CGameObject()
{
	m_pAnimationComp = new AnimationComponent(sptrAnimation, animationSize, eAni_Tick_Effect::ATE_DEFAULT_TICK);
	m_pLifeStateComp = new LifeStateComponent(eState);

	m_pAnimationComp->SetOwner(this);
	m_pLifeStateComp->SetOwner(this);
}

HitEffect::~HitEffect()
{
}

HitEffect::HitEffect(const HitEffect& other)
	: CGameObject(other.m_ivPos.x, other.m_ivPos.y)
{
	m_ivOldPos = other.m_ivOldPos;

	m_pAnimationComp = new AnimationComponent(this);
	m_pLifeStateComp = new LifeStateComponent(this);

	*m_pAnimationComp = *other.m_pAnimationComp;
	*m_pLifeStateComp = *other.m_pLifeStateComp;
}

HitEffect& HitEffect::operator=(HitEffect&& rhs) noexcept
{
	if (this != &rhs)
	{
		DeleteComponents();

		m_pAnimationComp = rhs.m_pAnimationComp;
		m_pLifeStateComp = rhs.m_pLifeStateComp;

		m_pAnimationComp->SetOwner(this);
		m_pLifeStateComp->SetOwner(this);

		rhs.m_ivOldPos = { 0, 0 };
		rhs.m_ivPos    = { 0, 0 };
		rhs.m_pAnimationComp = nullptr;
		rhs.m_pLifeStateComp = nullptr;
	}

	return *this;
}

void HitEffect::ResetData(const CGameObject* sample)
{
	m_ivOldPos = { 0, 0 };
	m_ivPos = { 0, 0 };

	m_pAnimationComp->ResetAnimation();
	m_pLifeStateComp->SetState(eState::STATE_VALID);
}
