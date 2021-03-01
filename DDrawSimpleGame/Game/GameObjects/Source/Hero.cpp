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
// Desc: 해당 객체가 충돌 시 호출되는 핸들러로서, 
// 충돌 시 수행할 동작을 정의합니다.
////////////////////////////////////////////////////////////
void Hero::collision_handler(const CGameObject* other)
{
	if (other->GetPowerComp())
	{
		m_pHealthComp->HitByOther(*other);

		if (m_pHealthComp->IsDead())
		{
			// 죽었을 경우 수행할 동작을 정의
			// 현재는 아무 처리도 하지 않음(플레이어)
		}
	}
}

////////////////////////////////////////////////////////////
// Name: ResetData()
// Desc: 데이터 멤버를 초기 상태로 되돌립니다.
////////////////////////////////////////////////////////////
void Hero::ResetData(const CGameObject* sample)
{
	// CGameObject* 타입 그대로 사용하면 하위 클래스에 추가한
	// 멤버에 접근할 수 없기 때문에 캐스팅하여 사용
	// (현재는 필요없지만..)
	const Hero* pSample = reinterpret_cast<const Hero*>(sample);

	m_ivOldPos = { 0, 0 };
	m_ivPos = { 0, 0 };

	m_pPhysicsComp->SetCollisionable(true);
	m_pHealthComp->SetHP(pSample->GetHealthComp()->GetHP());
	m_pHealthComp->SetLife(pSample->GetHealthComp()->GetLife());
}
