/*
	AnimationComponent.h

	애니메이션이 필요한 요소를 위한 컴포넌트
*/

#pragma once

#include "BaseComponent.h"
#include "../../../Util/Header/ImageData.h"

class CGameObject;

class AnimationComponent : public BaseComponent
{
public:
	AnimationComponent(CGameObject* owner);
	AnimationComponent(std::shared_ptr<ImageData*[]> sptrAnimation, size_t animationSize, ULONGLONG updateTick);
	virtual ~AnimationComponent();

	AnimationComponent(const AnimationComponent& other);
	AnimationComponent& operator=(const AnimationComponent& rhs);

	virtual void Initialize(void* pArg) override {}

	virtual void Draw(OUT int* x, OUT int* y);
	virtual void Draw(OUT INT_VECTOR2* ivOutPos);

	inline void SetAnimationData(std::shared_ptr<ImageData*[]> sptrAnimation) 
	{ m_sptrAnimation = sptrAnimation; }

	void UpdateAnimation(bool bRepeated = true);
	inline void ResetAnimation();

	inline DWORD GetWidth() const { return m_sptrAnimation[m_szCurImageIndex]->GetWidth(); }
	inline DWORD GetHeight() const { return m_sptrAnimation[m_szCurImageIndex]->GetHeight(); }

protected:
	std::shared_ptr<ImageData*[]> m_sptrAnimation;    // 연속된 이미지 데이터를 가리키는 더블 포인터
	size_t						  m_szAnimationSize;  // 애니메이션 이미지 개수
	size_t						  m_szCurImageIndex;  // 현재 이미지 위치
	ULONGLONG					  m_ulUpdateTick;     // 애니메이션 업데이트 기준
	ULONGLONG					  m_ulPrvUpdateTick;  // 이전 애니메이션 업데이트 tick
};

inline void AnimationComponent::ResetAnimation()
{
	m_szCurImageIndex = 0;
	m_ulPrvUpdateTick = 0;
}