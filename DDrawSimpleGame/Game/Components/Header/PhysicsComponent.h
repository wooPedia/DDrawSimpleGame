/*
	PhysicsComponent.h

	�浹�� ���� ������ ����� �ʿ��� ������Ʈ�� ���� ������Ʈ
*/

#pragma once

#include <functional>
#include "BaseComponent.h"

class CGameObject;

class PhysicsComponent : public BaseComponent
{
	using eventHandler = std::function<void(const CGameObject*)>; // �浹 ó�� �Լ� �ñ׳���

public:
	PhysicsComponent(CGameObject* owner);
	PhysicsComponent(bool bCollisionable = true);
	virtual ~PhysicsComponent();

	PhysicsComponent(const PhysicsComponent& other);
	PhysicsComponent& operator=(const PhysicsComponent& rhs);

	virtual void Initialize(void* pArg) override  {}
	inline void SetCollisionable(bool enable) { m_bCollisionable = enable; }
	inline bool IsCollisionable() const { return m_bCollisionable; }

	inline void SetEventHandler(eventHandler handler) { m_fpCollisionHandler = handler; }
	
	inline bool NotifyHit(IN const CGameObject* other);
	
protected:
	bool m_bCollisionable;				// �ش� ���� false�̸� �ش� ��ü�� �浹 ó������ ����
	eventHandler m_fpCollisionHandler;  // �浹 �� ������ ���ǵ� �ݹ� �Լ�
										// �ش� ������Ʈ�� ������ ������Ʈ��
										// �浹 ó�� �ڵ鷯�� �ݵ�� �����ؾ� ��
};


////////////////////////////////////////////////////////////
// Name: NotifyHit()
// Desc: �浹�� �Ͼ�� �� �ش� �Լ��� ȣ���ϸ� 
// �̸� ��ϵ� �浹ó�� �ڵ鷯�� ȣ��˴ϴ�.
////////////////////////////////////////////////////////////
bool PhysicsComponent::NotifyHit(IN const CGameObject* other /* �浹�� ��� */)
{
	assert(m_fpCollisionHandler != nullptr);

	if (m_bCollisionable)
	{
		m_fpCollisionHandler(other);

		return true;
	}

	return false;
}