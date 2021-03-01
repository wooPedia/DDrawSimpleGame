/*
	HealthComponent.h

	체력과 생명을 위한 컴포넌트
*/

#pragma once

#include "BaseComponent.h"

class CGameObject;

class HealthComponent : public BaseComponent
{
public:
	HealthComponent(CGameObject* owner);
	HealthComponent(int hp = 0, int life = 0);
	virtual ~HealthComponent();

	HealthComponent(const HealthComponent& other);
	HealthComponent& operator=(const HealthComponent& rhs);

	virtual void Initialize(void* pArg) override {}

	inline void SetHP(int hp) { m_iHP = hp; }
	inline int  GetHP() const { return m_iHP; }

	inline void SetLife(int life) { m_iLife = life; }
	inline int GetLife() const	  { return m_iLife; }

	inline void LostLife() { if (m_iLife > 0) --m_iLife; }
	
	inline bool IsDead() const { return m_iHP <= 0; }

	void HitByOther(IN const CGameObject& other);

protected:
	int m_iHP;
	int m_iLife; // 현재는 사용하지 않음
};