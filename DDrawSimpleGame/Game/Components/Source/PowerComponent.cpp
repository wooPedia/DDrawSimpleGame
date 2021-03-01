#include "../Header/PowerComponent.h"

PowerComponent::PowerComponent(CGameObject* owner)
	: BaseComponent(owner)
	, m_iPower(0)
{
}

PowerComponent::PowerComponent(int power /* = 0 */)
	: m_iPower(power)
{
}

PowerComponent::~PowerComponent()
{
}

// copy constructor
PowerComponent::PowerComponent(const PowerComponent& other)
	: BaseComponent(nullptr)
	, m_iPower(other.m_iPower)
{
}

// copy assignment operator
PowerComponent& PowerComponent::operator=(const PowerComponent& rhs)
{
	if (this != &rhs)
	{
		m_iPower = rhs.m_iPower;
	}

	return *this;
}
