#include "../Header/RenderComponent.h"
#include "../../../MyDDraw/Header/MyDDraw.h"

#include <memory>

extern MyDDraw* g_pDDraw;

RenderComponent::RenderComponent(CGameObject* owner)
	: BaseComponent(owner)
	, m_sptrImageData(nullptr)
{
}

RenderComponent::RenderComponent(std::shared_ptr<ImageData> sptrImageData /* = nullptr */)
	: m_sptrImageData(sptrImageData)
{
}

RenderComponent::~RenderComponent()
{
}

RenderComponent::RenderComponent(const RenderComponent& other)
	: BaseComponent(nullptr)
	, m_sptrImageData(other.m_sptrImageData) // read만 하므로 공유
{
}

RenderComponent& RenderComponent::operator=(const RenderComponent& rhs)
{
	if (this != &rhs)
	{
		m_sptrImageData = rhs.m_sptrImageData; // 읽기만 하기에 공유
	}

	return *this;
}

void RenderComponent::Initialize(void* pArg)
{
}

void RenderComponent::Draw(OUT int* x, OUT int* y)
{
	g_pDDraw->DrawImageData(x, y, *m_sptrImageData);
}

void RenderComponent::Draw(OUT INT_VECTOR2* ivOutPos)
{
	g_pDDraw->DrawImageData(&ivOutPos->x, &ivOutPos->y, *m_sptrImageData);
}