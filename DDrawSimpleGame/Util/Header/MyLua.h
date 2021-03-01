#pragma once

////////////////////////////////////////////////////////
// MyLua.h
// Lua ������ �а� �ٷ�� ���� �����ִ� ���� Ŭ����
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
// Desc: ���� ���̺��� key�� �ش��ϴ� value�� ��ȯ�մϴ�. 
////////////////////////////////////////////////////////////
template <typename T>
void CMyLua::Lua_GetValueFromTable(const char* key, OUT T* outValue)
{
	static_assert(
		true == std::is_integral_v<T>
		|| true == std::is_floating_point_v<T>,
		"Check The type of the outValue!"
		);

	lua_pushstring(m_pLS, key); // key�� ������ top�� �ø���
	lua_gettable(m_pLS, -2);    // [-2]�� table�� key(top)�� ��ȸ, top�� key�� ������ pop��

	double dValue = lua_tonumber(m_pLS, -1);
	*outValue = static_cast<T>(dValue);

	lua_pop(m_pLS, 1); // ������ top���� 1����ŭ pop
}


////////////////////////////////////////////////////////////
// Name: Lua_GetValue()
// Desc: �ش� (����) ������ ���� ��ȯ�մϴ�. 
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