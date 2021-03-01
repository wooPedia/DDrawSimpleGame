/*
	AnimationComponent.h

	�ִϸ��̼��� �ʿ��� ��Ҹ� ���� ������Ʈ
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
	std::shared_ptr<ImageData*[]> m_sptrAnimation;    // ���ӵ� �̹��� �����͸� ����Ű�� ���� ������
	size_t						  m_szAnimationSize;  // �ִϸ��̼� �̹��� ����
	size_t						  m_szCurImageIndex;  // ���� �̹��� ��ġ
	ULONGLONG					  m_ulUpdateTick;     // �ִϸ��̼� ������Ʈ ����
	ULONGLONG					  m_ulPrvUpdateTick;  // ���� �ִϸ��̼� ������Ʈ tick
};

inline void AnimationComponent::ResetAnimation()
{
	m_szCurImageIndex = 0;
	m_ulPrvUpdateTick = 0;
}