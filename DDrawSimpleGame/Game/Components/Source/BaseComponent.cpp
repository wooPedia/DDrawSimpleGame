#include "../Header/BaseComponent.h"

BaseComponent::BaseComponent(CGameObject* owner)
	: m_pOwner(owner)
{
}

BaseComponent::~BaseComponent()
{
	m_pOwner = nullptr;
}
