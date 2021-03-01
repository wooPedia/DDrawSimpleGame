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
	// �ڵ鷯���� �����ϴ� ���� �ùٸ��� ���� ����
	// ���� �ڵ鷯�� �״�� �д�.
	// �ʿ� �� �ش� ������Ʈ�� ������ Ŭ������ ���� �����ڿ��� ���� �������־�� ��
}

PhysicsComponent& PhysicsComponent::operator=(const PhysicsComponent& rhs)
{
	if (this != &rhs)
	{
		m_bCollisionable = rhs.m_bCollisionable;

		// �ڵ鷯���� �����ϴ� ���� �ùٸ��� ���� ����
		// ���� �ڵ鷯�� �״�� �д�.
		// �ʿ� �� �ش� ������Ʈ�� ������ Ŭ������ ���� ���� �����ڿ��� ���� �������־�� ��
	}

	return *this;
}
