#include "../Header/AnimationComponent.h"
#include "../Header/LifeStateComponent.h"
#include "../Header/../../GameObjects/Header/GameObject.h"
#include "../../../Util/Header/ImageData.h"
#include "../../../MyDDraw/Header/MyDDraw.h"

extern MyDDraw* g_pDDraw;

AnimationComponent::AnimationComponent(CGameObject* owner)
	: BaseComponent(owner)
	, m_sptrAnimation(nullptr)
	, m_szAnimationSize(0)
	, m_szCurImageIndex(0)
	, m_ulUpdateTick(0)
	, m_ulPrvUpdateTick(0)
{
}

AnimationComponent::AnimationComponent(std::shared_ptr<ImageData*[]> sptrAnimation, size_t animationSize, ULONGLONG updateTick)
	: m_sptrAnimation(sptrAnimation)
	, m_szAnimationSize(animationSize)
	, m_szCurImageIndex(0)
	, m_ulUpdateTick(updateTick)
	, m_ulPrvUpdateTick(0)
{
}

AnimationComponent::~AnimationComponent()
{
}

AnimationComponent::AnimationComponent(const AnimationComponent& other)
	: BaseComponent(nullptr)
	, m_sptrAnimation(other.m_sptrAnimation)
	, m_szAnimationSize(other.m_szAnimationSize)
	, m_szCurImageIndex(other.m_szCurImageIndex) /* 0으로 해야하나? */
	, m_ulUpdateTick(other.m_ulUpdateTick)
	, m_ulPrvUpdateTick(0)
{
	// 새로 복사 생성되는 오브젝트의 owner는 
	// 오브젝트에서 별개로 설정을 해주어야 함
}

AnimationComponent& AnimationComponent::operator=(const AnimationComponent& rhs)
{
	if (this != &rhs)
	{
		m_sptrAnimation	  = rhs.m_sptrAnimation;
		m_szAnimationSize = rhs.m_szAnimationSize;
		m_szCurImageIndex = rhs.m_szCurImageIndex;
		m_ulUpdateTick	  = rhs.m_ulUpdateTick;
		m_ulPrvUpdateTick = 0;

		// Owner는 그대로 두는 게 맞다.
	}

	return *this;
}

void AnimationComponent::Draw(OUT int* x, OUT int* y)
{
	g_pDDraw->DrawImageData(x, y, *m_sptrAnimation[m_szCurImageIndex]);
}

void AnimationComponent::Draw(OUT INT_VECTOR2* ivOutPos)
{
	g_pDDraw->DrawImageData(&ivOutPos->x, &ivOutPos->y, *m_sptrAnimation[m_szCurImageIndex]);
}

void AnimationComponent::UpdateAnimation(bool bRepeated /* = true */)
{
	ULONGLONG CurTick = GetTickCount64();
	ULONGLONG DeltaTick = CurTick - m_ulPrvUpdateTick;

	if (DeltaTick >= m_ulUpdateTick)
	{
		m_ulPrvUpdateTick = CurTick;

		if (m_szCurImageIndex >= m_szAnimationSize - 1)
		{
			if (bRepeated) // 반복적으로 수행할 애니메이션일 경우
			{
				m_szCurImageIndex = 0;
			}
			else
			{
				if (m_pOwner->GetLifeStateComp())
				{
					m_pOwner->GetLifeStateComp()->SetState(eState::STATE_EXPIRED);
					return;
				}
				else
				{
					// 임시
					__debugbreak();
				}
			}
		}

		++m_szCurImageIndex;
	}
}
