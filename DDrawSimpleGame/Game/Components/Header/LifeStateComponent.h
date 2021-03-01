/*
	LifeStateComponent.h

	체력과 생명은 없지만 유효/무효한 상태를 
	가지는 오브젝트를 위한 컴포넌트
*/

#pragma once

#include "BaseComponent.h"

class CGameObject;

enum eState
{
	STATE_EXPIRED = 0,
	STATE_VALID	  = 1,
};

class LifeStateComponent : public BaseComponent
{
public:
	LifeStateComponent(CGameObject* owner);
	LifeStateComponent(eState state = eState::STATE_VALID);
	virtual ~LifeStateComponent();

	LifeStateComponent(const LifeStateComponent& other);
	LifeStateComponent& operator=(const LifeStateComponent& rhs);

	virtual void Initialize(void* pArg) override {}

	inline void SetState(eState state) { m_eState = state; }
	inline bool IsExpired() const { return m_eState == eState::STATE_EXPIRED; }

protected:
	eState m_eState;
};