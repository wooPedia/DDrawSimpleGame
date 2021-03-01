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
	// 하위 클래스에서 필요한 컴포넌트들만 할당해서 사용
	RenderComponent*	 m_pRenderComp;		 // 1개의 이미지를 가지는 객체
	AnimationComponent*  m_pAnimationComp;	 // 애니메이션 이미지를 가지는 객체	
	PhysicsComponent*	 m_pPhysicsComp;	 // 충돌이 가능한 객체
	MoveComponent*		 m_pMoveComp;		 // 움직일 수 있는 객체
	PowerComponent*		 m_pPowerComp;		 // 공격력을 가지는 객체
	HealthComponent*	 m_pHealthComp;		 // 체력을 가지는 객체
	LifeStateComponent*  m_pLifeStateComp;	 // 생명없이 유효/무효 상태를 가지는 객체 i.e. 총알
};