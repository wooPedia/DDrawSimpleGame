/*
	BaseComponent.h

	모든 컴포넌트의 기본이 되는 베이스 컴포넌트.
	모든 컴포넌트들은 해당 클래스를 상속받아야 합니다.
*/

#pragma once

#include "../../../pch/stdafx.h"

class CGameObject;

class BaseComponent
{
public:
	BaseComponent(CGameObject* owner = nullptr);
	virtual ~BaseComponent();

	virtual void Initialize(void* pArg) = 0;

	inline void SetOwner(CGameObject* owner) { m_pOwner = owner; }
	inline const CGameObject* GetOwner() const { return m_pOwner; }

protected:
	CGameObject* m_pOwner; // 해당 컴포넌트를 소유한 오브젝트
};