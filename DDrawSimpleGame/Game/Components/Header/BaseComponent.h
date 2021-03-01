/*
	BaseComponent.h

	��� ������Ʈ�� �⺻�� �Ǵ� ���̽� ������Ʈ.
	��� ������Ʈ���� �ش� Ŭ������ ��ӹ޾ƾ� �մϴ�.
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
	CGameObject* m_pOwner; // �ش� ������Ʈ�� ������ ������Ʈ
};