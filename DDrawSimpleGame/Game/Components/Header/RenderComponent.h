/*
	RenderComponent.h

	화면에 그려져야 하는 오브젝트를 위한 컴포넌트
	단일 이미지가 아닌 애니메이션이 필요하다면 
	해당 컴포넌트가 아닌 AnimationComponent를 사용
*/

#pragma once

#include "BaseComponent.h"
#include "../../../Util/Header/ImageData.h"

class CGameObject;

class RenderComponent : public BaseComponent
{
public:
	RenderComponent(CGameObject* owner);
	RenderComponent(std::shared_ptr<ImageData> sptrImageData = nullptr);
	virtual ~RenderComponent();

	RenderComponent(const RenderComponent& other);
	RenderComponent& operator=(const RenderComponent& rhs);

	virtual void Initialize(void* pArg) override;

	virtual void Draw(OUT int* x, OUT int* y);
	virtual void Draw(OUT INT_VECTOR2* ivOutPos);

	inline void SetImageDate(std::shared_ptr<ImageData> sptrImageData) { m_sptrImageData = sptrImageData; }

	inline DWORD GetWidth() const { return m_sptrImageData->GetWidth(); }
	inline DWORD GetHeight() const { return m_sptrImageData->GetHeight(); }

protected:
	std::shared_ptr<ImageData> m_sptrImageData;
};