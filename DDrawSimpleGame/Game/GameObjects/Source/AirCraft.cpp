#include "../Header/AirCraft.h"

#include "../../Components/Header/RenderComponent.h"
#include "../../Components/Header/PhysicsComponent.h"
#include "../../Components/Header/MoveComponent.h"
#include "../../Components/Header/PowerComponent.h"
#include "../../Components/Header/HealthComponent.h"

#include "../../../DataStructure/helper_function.h"

#include <memory>

AirCraft::AirCraft(int x /* = 0 */, int y /* = 0 */)
	: CGameObject(x, y)
{
}

AirCraft::AirCraft(
	std::shared_ptr<ImageData> sptrImageData,
	int HP,
	int life,
	int power,
	int speed,
	bool bCollisionable /* = true */
)
{
	m_pRenderComp  = new RenderComponent(sptrImageData);
	m_pPhysicsComp = new PhysicsComponent(bCollisionable);
	m_pMoveComp    = new MoveComponent(speed);
	m_pPowerComp   = new PowerComponent(power);
	m_pHealthComp  = new HealthComponent(HP, life);

	m_pRenderComp->SetOwner(this);
	m_pPhysicsComp->SetOwner(this);
	m_pMoveComp->SetOwner(this);
	m_pPowerComp->SetOwner(this);
	m_pHealthComp->SetOwner(this);

	m_pPhysicsComp->SetEventHandler(make_memberfunc(&AirCraft::collision_handler, this));
}

AirCraft::AirCraft(const AirCraft& other)
	: CGameObject(other.m_ivPos.x, other.m_ivPos.y)
{
	m_ivOldPos = other.m_ivOldPos;
	m_ivPos    = other.m_ivPos;

	m_pRenderComp  = new RenderComponent(this);
	m_pPhysicsComp = new PhysicsComponent(this);
	m_pMoveComp	   = new MoveComponent(this);
	m_pPowerComp   = new PowerComponent(this);
	m_pHealthComp  = new HealthComponent(this);

	*m_pRenderComp  = *other.m_pRenderComp;
	*m_pPhysicsComp = *other.m_pPhysicsComp;
	*m_pMoveComp	= *other.m_pMoveComp;
	*m_pPowerComp	= *other.m_pPowerComp;
	*m_pHealthComp  = *other.m_pHealthComp;

	m_pPhysicsComp->SetEventHandler(make_memberfunc(&AirCraft::collision_handler, this));
}

AirCraft& AirCraft::operator=(AirCraft&& rhs) noexcept
{
	if (this != &rhs)
	{
		DeleteComponents();

		this->m_ivOldPos		= rhs.m_ivOldPos;
		this->m_ivPos			= rhs.m_ivPos;

		this->m_pRenderComp		= rhs.m_pRenderComp;
		this->m_pPhysicsComp    = rhs.m_pPhysicsComp;
		this->m_pHealthComp     = rhs.m_pHealthComp;
		this->m_pMoveComp		= rhs.m_pMoveComp;
		this->m_pPowerComp		= rhs.m_pPowerComp;

		m_pRenderComp->SetOwner(this);
		m_pPhysicsComp->SetOwner(this);
		m_pMoveComp->SetOwner(this);
		m_pPowerComp->SetOwner(this);
		m_pHealthComp->SetOwner(this);

		rhs.m_ivOldPos       = { 0, 0 };
		rhs.m_ivPos          = { 0, 0 };

		rhs.m_pRenderComp    = nullptr;
		rhs.m_pPhysicsComp   = nullptr;
		rhs.m_pHealthComp    = nullptr;
		rhs.m_pMoveComp		 = nullptr;
		rhs.m_pPowerComp	 = nullptr;

		m_pPhysicsComp->SetEventHandler(make_memberfunc(&AirCraft::collision_handler, this));
	}

	return *this;
}

AirCraft::~AirCraft()
{
}

////////////////////////////////////////////////////////////
// Name: collision_handler()
// Desc: 해당 객체가 충돌 시 호출되는 핸들러로서, 
// 충돌 시 수행할 동작을 정의합니다.
////////////////////////////////////////////////////////////
void AirCraft::collision_handler(const CGameObject* other)
{
	if (other->GetPowerComp())
	{
		m_pHealthComp->HitByOther(*other);
	}
}

////////////////////////////////////////////////////////////
// Name: ResetData()
// Desc: 데이터 멤버를 초기 상태로 되돌립니다.
////////////////////////////////////////////////////////////
void AirCraft::ResetData(const CGameObject* sample)
{
	const AirCraft* pSample = reinterpret_cast<const AirCraft*>(sample);

	m_ivOldPos = { 0, 0 };
	m_ivPos = { 0, 0 };

	m_pPhysicsComp->SetCollisionable(true);
	m_pHealthComp->SetHP(pSample->GetHealthComp()->GetHP());
	m_pHealthComp->SetLife(pSample->GetHealthComp()->GetLife());
}