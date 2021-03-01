#include "../Header/Ammo.h"

#include "../../Components/Header/RenderComponent.h"
#include "../../Components/Header/PhysicsComponent.h"
#include "../../Components/Header/MoveComponent.h"
#include "../../Components/Header/PowerComponent.h"

#include <memory>

Ammo::Ammo(int x /* = 0 */, int y /* = 0 */)
	: CGameObject(x, y)
{
}

Ammo::Ammo(
	std::shared_ptr<ImageData> sptrImageData, 
	int power, 
	int speed, 
	bool bCollisionable /* = true */, 
	eState eState	    /* = eState::STATE_VALID */
)
{
	m_pRenderComp = new RenderComponent(sptrImageData);
	m_pPhysicsComp = new PhysicsComponent(bCollisionable);
	m_pMoveComp = new MoveComponent(speed);
	m_pPowerComp = new PowerComponent(power);
	m_pLifeStateComp = new LifeStateComponent(eState);

	m_pRenderComp->SetOwner(this);
	m_pPhysicsComp->SetOwner(this);
	m_pMoveComp->SetOwner(this);
	m_pPowerComp->SetOwner(this);
	m_pLifeStateComp->SetOwner(this);

	m_pPhysicsComp->SetEventHandler(std::bind(&Ammo::collision_handler, this, std::placeholders::_1));
}

Ammo::Ammo(const Ammo& other)
	: CGameObject(other.m_ivPos.x, other.m_ivPos.y)
{
		m_ivOldPos		  = other.m_ivOldPos;
		
		m_pRenderComp	  = new RenderComponent(this);
		m_pPhysicsComp	  = new PhysicsComponent(this);
		m_pMoveComp		  = new MoveComponent(this);
		m_pPowerComp	  = new PowerComponent(this);
		m_pLifeStateComp  = new LifeStateComponent(this);

		*m_pRenderComp    = *other.m_pRenderComp;
		*m_pPhysicsComp   = *other.m_pPhysicsComp;
		*m_pMoveComp	  = *other.m_pMoveComp;
		*m_pPowerComp	  = *other.m_pPowerComp;
		*m_pLifeStateComp = *other.m_pLifeStateComp;

		m_pPhysicsComp->SetEventHandler(std::bind(&Ammo::collision_handler, this, std::placeholders::_1));
}

Ammo& Ammo::operator=(Ammo&& rhs) noexcept
{
	if (this != &rhs)
	{
		DeleteComponents();

		m_ivOldPos = rhs.m_ivOldPos;
		m_ivPos	   = rhs.m_ivPos;
		m_pRenderComp		 = rhs.m_pRenderComp;
		m_pPhysicsComp		 = rhs.m_pPhysicsComp;
		m_pLifeStateComp	 = rhs.m_pLifeStateComp;
		m_pMoveComp			 = rhs.m_pMoveComp;
		m_pPowerComp		 = rhs.m_pPowerComp;

		m_pRenderComp->SetOwner(this);
		m_pPhysicsComp->SetOwner(this);
		m_pMoveComp->SetOwner(this);
		m_pPowerComp->SetOwner(this);
		m_pLifeStateComp->SetOwner(this);

		rhs.m_ivOldPos		 = {0, 0};
		rhs.m_ivPos			 = {0, 0};
		rhs.m_pRenderComp	 = nullptr;
		rhs.m_pPhysicsComp	 = nullptr;
		rhs.m_pLifeStateComp = nullptr;
		rhs.m_pMoveComp		 = nullptr;
		rhs.m_pPowerComp	 = nullptr;

		m_pPhysicsComp->SetEventHandler(std::bind(&Ammo::collision_handler, this, std::placeholders::_1));
	}

	return *this;
}

Ammo::~Ammo()
{
}

////////////////////////////////////////////////////////////
// Name: collision_handler()
// Desc: �ش� ��ü�� �浹 �� ȣ��Ǵ� �ڵ鷯�μ�, 
// �浹 �� ������ ������ �����մϴ�.
////////////////////////////////////////////////////////////
void Ammo::collision_handler(const CGameObject* other)
{
	// other�� �ڱ� �ڽ��̶�� ���� �ε��� �ɷ�..
	if (this == other) 
	{
		// ���� �ε����� ���� �ϰ� ���� ������ ����
	}

	m_pLifeStateComp->SetState(eState::STATE_EXPIRED); // ���� �ε����� �ش� ������ �ʿ�

	// �ʿ��ϴٸ� ���⿡ ����Ʈ ����
}

////////////////////////////////////////////////////////////
// Name: ResetData()
// Desc: ������ ����� �ʱ� ���·� �ǵ����ϴ�.
////////////////////////////////////////////////////////////
void Ammo::ResetData(const CGameObject* sample)
{
	m_ivOldPos = { 0, 0 };
	m_ivPos = { 0, 0 };

	m_pPhysicsComp->SetCollisionable(true);
	m_pLifeStateComp->SetState(eState::STATE_VALID);
}
