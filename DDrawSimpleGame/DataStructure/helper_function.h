#pragma once

#include <functional>

class CGameObject;

// �Լ� ��ü�� �������ִ� ����� �Լ�
template <typename Class>
std::function<void(const CGameObject*)> make_memberfunc(
	void (Class::*f)(const CGameObject*),
	Class* pObj)
{
	return std::bind(f, pObj, std::placeholders::_1);
}