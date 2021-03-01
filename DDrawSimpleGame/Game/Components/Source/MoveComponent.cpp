#include "../Header/MoveComponent.h"

MoveComponent::MoveComponent(CGameObject* owner)
	: BaseComponent(owner)
	, m_iSpeed(0)
{
}

MoveComponent::MoveComponent(int speed /* = 0 */)
	: m_iSpeed(speed)
{
}

MoveComponent::~MoveComponent()
{
}

MoveComponent::MoveComponent(const MoveComponent& other)
	: BaseComponent(nullptr)
	, m_iSpeed(other.m_iSpeed)
{
}

MoveComponent& MoveComponent::operator=(const MoveComponent& rhs)
{
	if (this != &rhs)
	{
		m_iSpeed = rhs.m_iSpeed;
	}

	return *this;
}

void MoveComponent::MoveUp(OUT int* curPosx, OUT int* curPosy)
{
	*curPosy -= m_iSpeed;
}

void MoveComponent::MoveDown(OUT int* curPosx, OUT int* curPosy)
{
	*curPosy += m_iSpeed;
}

void MoveComponent::MoveRight(OUT int* curPosx, OUT int* curPosy)
{
	*curPosx += m_iSpeed;
}

void MoveComponent::MoveLeft(OUT int* curPosx, OUT int* curPosy)
{
	*curPosx -= m_iSpeed;
}

void MoveComponent::MoveUp(OUT INT_VECTOR2* ivCurPos)
{
	ivCurPos->y -= m_iSpeed;
}

void MoveComponent::MoveDown(OUT INT_VECTOR2* ivCurPos)
{
	ivCurPos->y += m_iSpeed;
}

void MoveComponent::MoveRight(OUT INT_VECTOR2* ivCurPos)
{
	ivCurPos->x += m_iSpeed;
}

void MoveComponent::MoveLeft(OUT INT_VECTOR2* ivCurPos)
{
	ivCurPos->x -= m_iSpeed;
}
