#include "../Header/GameObject.h"
#include "../../../Util/Header/ImageData.h"
#include "../../../DataStructure/game_setting.h"

#include "../../Components/Header/AnimationComponent.h"
#include "../../Components/Header/RenderComponent.h"
#include "../../Components/Header/PhysicsComponent.h"
#include "../../Components/Header/PowerComponent.h"
#include "../../Components/Header/MoveComponent.h"
#include "../../Components/Header/HealthComponent.h"
#include "../../Components/Header/LifeStateComponent.h"

CGameObject::CGameObject()
	: m_ivOldPos{}
	, m_ivPos{}
	, m_pRenderComp(nullptr)
	, m_pAnimationComp(nullptr)
	, m_pPhysicsComp(nullptr)
	, m_pMoveComp(nullptr)
	, m_pPowerComp(nullptr)
	, m_pHealthComp(nullptr)
	, m_pLifeStateComp(nullptr)
{
}

CGameObject::CGameObject(int x, int y)
	: m_ivOldPos{x, y}
	, m_ivPos{x, y}
	, m_pRenderComp(nullptr)
	, m_pAnimationComp(nullptr)
	, m_pPhysicsComp(nullptr)
	, m_pMoveComp(nullptr)
	, m_pPowerComp(nullptr)
	, m_pHealthComp(nullptr)
	, m_pLifeStateComp(nullptr)
{
}

CGameObject::~CGameObject()
{
	DeleteComponents();
}

VOID CGameObject::SetPos(IN const INT_VECTOR2& p_ivInPos)
{
	//m_ivOldPos = m_ivPos;
	m_ivPos.x = p_ivInPos.x;
	m_ivPos.y = p_ivInPos.y;
}

VOID CGameObject::SetPos(int x, int y)
{
	m_ivOldPos = m_ivPos;

	m_ivPos.x = x;
	m_ivPos.y = y;
}

void CGameObject::DeleteComponents()
{
	SAFE_DELETE(m_pRenderComp);
	SAFE_DELETE(m_pAnimationComp);
	SAFE_DELETE(m_pPhysicsComp);
	SAFE_DELETE(m_pMoveComp);
	SAFE_DELETE(m_pPowerComp);
	SAFE_DELETE(m_pHealthComp);
	SAFE_DELETE(m_pLifeStateComp);
}

DWORD CGameObject::GetWidth() const
{
	if (m_pRenderComp)
	{
		return m_pRenderComp->GetWidth();
	}
	else if (m_pAnimationComp)
	{
		return m_pAnimationComp->GetWidth();
	}
	else
	{
		__debugbreak(); // 두 컴포넌트를 가지지 않는데 해당 함수를 호출했다면 이상
		return 1;
	}
}

DWORD CGameObject::GetHeight() const
{
	if (m_pRenderComp)
	{
		return m_pRenderComp->GetHeight();
	}
	else if (m_pAnimationComp)
	{
		return m_pAnimationComp->GetHeight();
	}
	else
	{
		__debugbreak(); // 두 컴포넌트를 가지지 않는데 해당 함수를 호출했다면 이상
		return 1;
	}
}
