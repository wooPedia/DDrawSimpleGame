#pragma once

#include "../../../pch/stdafx.h"

struct INT_VECTOR2;
class TGAImage;
class ImageData;
template <class T> class GameObjectManager;

class RenderComponent;
class AnimationComponent;
class PhysicsComponent;
class MoveComponent;
class PowerComponent;
class HealthComponent;
class LifeStateComponent;

class CGameObject
{
public:
	CGameObject();
	CGameObject(int x, int y);
	virtual ~CGameObject();

	inline VOID GetPos(OUT INT_VECTOR2* p_ivOutPos) const { *p_ivOutPos = m_ivPos; }
	VOID SetPos(IN const INT_VECTOR2& p_ivInPos);
	VOID SetPos(int x, int y);

	inline RenderComponent*		GetRenderComp()	   const { return m_pRenderComp; }
	inline AnimationComponent*  GetAnimationComp() const { return m_pAnimationComp; }
	inline PhysicsComponent*    GetPhysicsComp()   const { return m_pPhysicsComp; }
	inline MoveComponent*	    GetMoveComp()	   const { return m_pMoveComp; }
	inline PowerComponent*	    GetPowerComp()	   const { return m_pPowerComp; }
	inline HealthComponent*	    GetHealthComp()	   const { return m_pHealthComp; }
	inline LifeStateComponent*	GetLifeStateComp() const { return m_pLifeStateComp; }

	virtual void ResetData(const CGameObject* sample) { ; }
	virtual void DeleteComponents();

	DWORD GetWidth() const;
	DWORD GetHeight() const;

	inline bool HasImage() const { return m_pRenderComp || m_pAnimationComp; }

protected:
	INT_VECTOR2 m_ivOldPos;
	INT_VECTOR2 m_ivPos;

	// Components
	// ���� Ŭ�������� �ʿ��� ������Ʈ�鸸 �Ҵ��ؼ� ���
	RenderComponent*	 m_pRenderComp;		 // 1���� �̹����� ������ ��ü
	AnimationComponent*  m_pAnimationComp;	 // �ִϸ��̼� �̹����� ������ ��ü	
	PhysicsComponent*	 m_pPhysicsComp;	 // �浹�� ������ ��ü
	MoveComponent*		 m_pMoveComp;		 // ������ �� �ִ� ��ü
	PowerComponent*		 m_pPowerComp;		 // ���ݷ��� ������ ��ü
	HealthComponent*	 m_pHealthComp;		 // ü���� ������ ��ü
	LifeStateComponent*  m_pLifeStateComp;	 // ������� ��ȿ/��ȿ ���¸� ������ ��ü i.e. �Ѿ�
};