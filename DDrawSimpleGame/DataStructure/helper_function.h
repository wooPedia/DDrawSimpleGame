#pragma once

#include <functional>

class CGameObject;

// 함수 객체를 생성해주는 도우미 함수
template <typename Class>
std::function<void(const CGameObject*)> make_memberfunc(
	void (Class::*f)(const CGameObject*),
	Class* pObj)
{
	return std::bind(f, pObj, std::placeholders::_1);
}