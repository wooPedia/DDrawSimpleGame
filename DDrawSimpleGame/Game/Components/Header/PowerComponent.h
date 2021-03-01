/*
	PowerComponent.h

	���ݷ��� �ʿ��� ������Ʈ�� ���� ������Ʈ
*/

#pragma once

#include "BaseComponent.h"

class CGameObject;

class PowerComponent : public BaseComponent
{
public:
	PowerComponent(CGameObject* owner);
	PowerComponent(int power = 0);
	virtual ~PowerComponent();

	PowerComponent(const PowerComponent& other);
	PowerComponent& operator=(const PowerComponent& rhs);

	virtual void Initialize(void* pArg) override {}

	inline void SetPower(int power) { m_iPower = power; }
	inline int GetPower() const { return m_iPower; }

protected:
	int m_iPower;
};