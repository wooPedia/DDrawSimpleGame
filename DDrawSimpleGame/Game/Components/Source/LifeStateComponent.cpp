#include "../Header/LifeStateComponent.h"

LifeStateComponent::LifeStateComponent(CGameObject* owner)
	: BaseComponent(owner)
	, m_eState(eState::STATE_VALID)
{
}

LifeStateComponent::LifeStateComponent(eState state /* = eState::STATE_VALID */)
	: m_eState(state)
{
}

LifeStateComponent::~LifeStateComponent()
{
}

LifeStateComponent::LifeStateComponent(const LifeStateComponent& other)
	: BaseComponent(nullptr)
	, m_eState(other.m_eState)
{
}

LifeStateComponent& LifeStateComponent::operator=(const LifeStateComponent& rhs)
{
	if (this != &rhs)
	{
		m_eState = rhs.m_eState;
	}

	return *this;
}
