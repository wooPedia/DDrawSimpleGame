#include "../Header/Hero.h"

#include "../../Components/Header/AnimationComponent.h"
#include "../../Components/Header/PhysicsComponent.h"
#include "../../Components/Header/MoveComponent.h"
#include "../../Components/Header/HealthComponent.h"

#include "../../../DataStructure/helper_function.h"

Hero::Hero(int x /* = 0 */, int y /* = 0 */)
	: CGameObject(x, y)
{
}

Hero::Hero(std::shared_ptr<ImageData*[]> sptrAnimation, 
	size_t animationSize, 
	int HP, 
	int life, 
	int speed, 
	bool bCollisionable /* = true */
)
	: CGameObject()
{
	m_pAnimationComp = new AnimationComponent(sptrAnimation, animationSize, eAni_Tick_Hero::ATH_DEFAULT_TICK);
	m_pPhysicsComp   = new PhysicsComponent(bCollisionable);
	m_pMoveComp		 = new MoveComponent(speed);
	m_pHealthComp	 = new HealthComponent(HP, life);

	m_pAnimationComp->SetOwner(this);
	m_pPhysicsComp->SetOwner(this);
	m_pMoveComp->SetOwner(this);
	m_pHealthComp->SetOwner(this);

	m_pPhysicsComp->SetEventHandler(make_memberfunc<Hero>(&Hero::collision_handler, this));
}

Hero::~Hero()
{
}

Hero::Hero(const Hero& other) 
	: CGameObject(other.m_ivPos.x, other.m_ivPos.y)
{
	m_ivOldPos = other.m_ivOldPos;

	m_pAnimationComp  = new AnimationComponent(this);
	m_pPhysicsComp    = new PhysicsComponent(this);
	m_pMoveComp		  = new MoveComponent(this);
	m_pHealthComp     = new HealthComponent(this);

	*m_pAnimationComp = *other.m_pAnimationComp;
	*m_pPhysicsComp   = *other.m_pPhysicsComp;
	*m_pMoveComp      = *other.m_pMoveComp;
	*m_pHealthComp    = *other.m_pHealthComp;

	m_pPhysicsComp->SetEventHandler(make_memberfunc<Hero>(&Hero::collision_handler, this));
}

Hero& Hero::operator=(Hero&& rhs) noexcept
{
	if (this != &rhs)
	{
		DeleteComponents();

		m_ivOldPos = rhs.m_ivOldPos;
		m_ivPos	   = rhs.m_ivPos;

		m_pAnimationComp = rhs.m_pAnimationComp;
		m_pPhysicsComp   = rhs.m_pPhysicsComp;
		m_pMoveComp	     = rhs.m_pMoveComp;
		m_pHealthComp	 = rhs.m_pHealthComp;

		m_pAnimationComp->SetOwner(this);
		m_pPhysicsComp->SetOwner(this);
		m_pMoveComp->SetOwner(this);
		m_pHealthComp->SetOwner(this);

		rhs.m_ivOldPos		 = { 0, 0 };
		rhs.m_ivPos			 = { 0, 0 };
		rhs.m_pAnimationComp = nullptr;
		rhs.m_pPhysicsComp   = nullptr;
		rhs.m_pMoveComp      = nullptr;
		rhs.m_pHealthComp    = nullptr;

		m_pPhysicsComp->SetEventHandler(make_memberfunc<Hero>(&Hero::collision_handler, this));
	}

	return *this;
}

////////////////////////////////////////////////////////////
// Name: collision_handler()
// Desc: �ش� ��ü�� �浹 �� ȣ��Ǵ� �ڵ鷯�μ�, 
// �浹 �� ������ ������ �����մϴ�.
////////////////////////////////////////////////////////////
void Hero::collision_handler(const CGameObject* other)
{
	if (other->GetPowerComp())
	{
		m_pHealthComp->HitByOther(*other);

		if (m_pHealthComp->IsDead())
		{
			// �׾��� ��� ������ ������ ����
			// ����� �ƹ� ó���� ���� ����(�÷��̾�)
		}
	}
}

////////////////////////////////////////////////////////////
// Name: ResetData()
// Desc: ������ ����� �ʱ� ���·� �ǵ����ϴ�.
////////////////////////////////////////////////////////////
void Hero::ResetData(const CGameObject* sample)
{
	// CGameObject* Ÿ�� �״�� ����ϸ� ���� Ŭ������ �߰���
	// ����� ������ �� ���� ������ ĳ�����Ͽ� ���
	// (����� �ʿ������..)
	const Hero* pSample = reinterpret_cast<const Hero*>(sample);

	m_ivOldPos = { 0, 0 };
	m_ivPos = { 0, 0 };

	m_pPhysicsComp->SetCollisionable(true);
	m_pHealthComp->SetHP(pSample->GetHealthComp()->GetHP());
	m_pHealthComp->SetLife(pSample->GetHealthComp()->GetLife());
}
