#include "../Header/HealthComponent.h"
#include "../Header/PowerComponent.h"

#include "../../GameObjects/Header/GameObject.h"

HealthComponent::HealthComponent(CGameObject* owner)
	: BaseComponent(owner)
	, m_iHP(0)
	, m_iLife(0)
{
}

HealthComponent::HealthComponent(int hp /* = 0 */, int life /* = 0 */)
	: m_iHP(hp)
	, m_iLife(life)
{
}

HealthComponent::~HealthComponent()
{
}

HealthComponent::HealthComponent(const HealthComponent& other)
	: BaseComponent(nullptr)
	, m_iHP(other.m_iHP)
	, m_iLife(other.m_iLife)
{
}

HealthComponent& HealthComponent::operator=(const HealthComponent& rhs)
{
	if (this != &rhs)
	{
		m_iHP = rhs.m_iHP;
		m_iLife = rhs.m_iLife;
	}

	return *this;
}

void HealthComponent::HitByOther(IN const CGameObject& other)
{
	if (other.GetPowerComp())
	{
		m_iHP -= other.GetPowerComp()->GetPower();
	}
	else
	{
#ifdef _DEBUG
		// 공격력을 가지지 않은 오브젝트에 타격을 받을 수 없다.(충돌과 타격은 다름)
		__debugbreak();
#endif // _DEBUG
	}
}