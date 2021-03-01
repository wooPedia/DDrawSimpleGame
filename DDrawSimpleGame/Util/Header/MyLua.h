#pragma once

////////////////////////////////////////////////////////
// MyLua.h
// Lua 파일을 읽고 다루는 것을 도와주는 래퍼 클래스
////////////////////////////////////////////////////////

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <string>
#include "../../pch/stdafx.h"

class CMyLua final
{
public:
	CMyLua();
	~CMyLua();

	bool Lua_LoadFile(const char* fileName);
	bool Lua_BeginTable(const char* tableName);
	void Lua_EndTable();

	template <typename T>
	void Lua_GetValueFromTable(const char* key, OUT T* outValue);
	std::string Lua_GetStringFromTable(const char* key);

	template <typename T>
	void Lua_GetValue(const char* valueName, OUT T* outValue);
	std::string Lua_GetString(const char* valueName);

private:
	lua_State* m_pLS;
};

////////////////////////////////////////////////////////////
// Name: Lua_GetValueFromTable()
// Desc: 현재 테이블에서 key에 해당하는 value를 반환합니다. 
////////////////////////////////////////////////////////////
template <typename T>
void CMyLua::Lua_GetValueFromTable(const char* key, OUT T* outValue)
{
	static_assert(
		true == std::is_integral_v<T>
		|| true == std::is_floating_point_v<T>,
		"Check The type of the outValue!"
		);

	lua_pushstring(m_pLS, key); // key를 스택의 top에 올리고
	lua_gettable(m_pLS, -2);    // [-2]의 table을 key(top)로 조회, top의 key는 저절로 pop됨

	double dValue = lua_tonumber(m_pLS, -1);
	*outValue = static_cast<T>(dValue);

	lua_pop(m_pLS, 1); // 스택의 top부터 1개만큼 pop
}


////////////////////////////////////////////////////////////
// Name: Lua_GetValue()
// Desc: 해당 (전역) 변수의 값을 반환합니다. 
////////////////////////////////////////////////////////////
template <typename T>
void CMyLua::Lua_GetValue(const char* valueName, OUT T* outValue)
{
	static_assert(
		true == std::is_integral_v<T>
		|| true == std::is_floating_point_v<T>,
		"Check The type of the outValue!"
		);

	lua_getglobal(m_pLS, valueName);

	double dValue = lua_tonumber(m_pLS, -1);
	*outValue = static_cast<T>(dValue);

	lua_pop(m_pLS, 1);
}