#include "../Header/PhysicsComponent.h"

PhysicsComponent::PhysicsComponent(CGameObject* owner)
	: BaseComponent(owner)
	, m_bCollisionable(true)
{
}

PhysicsComponent::PhysicsComponent(bool bCollisionable /* = true */)
	: m_bCollisionable(bCollisionable)
{
}

PhysicsComponent::~PhysicsComponent()
{
	m_fpCollisionHandler = nullptr;
}

PhysicsComponent::PhysicsComponent(const PhysicsComponent& other)
	: BaseComponent(nullptr)
	, m_bCollisionable(other.m_bCollisionable)
{
	// 핸들러까지 복사하는 것은 올바르지 않은 방향
	// 따라서 핸들러는 그대로 둔다.
	// 필요 시 해당 컴포넌트를 가지는 클래스의 복사 생성자에서 따로 설정해주어야 함
}

PhysicsComponent& PhysicsComponent::operator=(const PhysicsComponent& rhs)
{
	if (this != &rhs)
	{
		m_bCollisionable = rhs.m_bCollisionable;

		// 핸들러까지 복사하는 것은 올바르지 않은 방향
		// 따라서 핸들러는 그대로 둔다.
		// 필요 시 해당 컴포넌트를 가지는 클래스의 복사 대입 연산자에서 따로 설정해주어야 함
	}

	return *this;
}
