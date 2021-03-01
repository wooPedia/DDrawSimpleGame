/*
	PhysicsComponent.h

	충돌과 같은 물리적 기능이 필요한 오브젝트를 위한 컴포넌트
*/

#pragma once

#include <functional>
#include "BaseComponent.h"

class CGameObject;

class PhysicsComponent : public BaseComponent
{
	using eventHandler = std::function<void(const CGameObject*)>; // 충돌 처리 함수 시그내쳐

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
	bool m_bCollisionable;				// 해당 값이 false이면 해당 물체는 충돌 처리되지 않음
	eventHandler m_fpCollisionHandler;  // 충돌 시 동작이 정의된 콜백 함수
										// 해당 컴포넌트를 가지는 오브젝트는
										// 충돌 처리 핸들러를 반드시 구현해야 함
};


////////////////////////////////////////////////////////////
// Name: NotifyHit()
// Desc: 충돌이 일어났을 때 해당 함수를 호출하면 
// 미리 등록된 충돌처리 핸들러가 호출됩니다.
////////////////////////////////////////////////////////////
bool PhysicsComponent::NotifyHit(IN const CGameObject* other /* 충돌한 상대 */)
{
	assert(m_fpCollisionHandler != nullptr);

	if (m_bCollisionable)
	{
		m_fpCollisionHandler(other);

		return true;
	}

	return false;
}