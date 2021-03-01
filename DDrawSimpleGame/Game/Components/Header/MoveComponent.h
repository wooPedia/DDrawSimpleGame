/*
	MoveComponent.h

	움직일 수 있는 오브젝트를 위한 컴포넌트
*/

#pragma once

#include "BaseComponent.h"

class CGameObject;

class MoveComponent : public BaseComponent
{
public:
	MoveComponent(CGameObject* owner);
	MoveComponent(int speed = 0);
	virtual ~MoveComponent();

	MoveComponent(const MoveComponent& other);
	MoveComponent& operator=(const MoveComponent& rhs);

	virtual void Initialize(void* pArg) override {}

	inline void SetSpeed(int speed) { m_iSpeed = speed; }
	inline int GetSpeed() const { return m_iSpeed; }

	virtual void MoveUp(OUT    int* curPosx, OUT int* curPosy);
	virtual void MoveDown(OUT  int* curPosx, OUT int* curPosy);
	virtual void MoveRight(OUT int* curPosx, OUT int* curPosy);
	virtual void MoveLeft(OUT  int* curPosx, OUT int* curPosy);

	virtual void MoveUp(OUT INT_VECTOR2*    ivCurPos);
	virtual void MoveDown(OUT INT_VECTOR2*  ivCurPos);
	virtual void MoveRight(OUT INT_VECTOR2* ivCurPos);
	virtual void MoveLeft(OUT INT_VECTOR2*  ivCurPos);

protected:
	int m_iSpeed;
};