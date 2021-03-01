#include "../Header/MyLua.h"

CMyLua::CMyLua()
	: m_pLS(luaL_newstate())
{
	luaL_openlibs(m_pLS);
}

CMyLua::~CMyLua()
{
	lua_close(m_pLS);
}

////////////////////////////////////////////////////////////
// Name: Lua_LoadFile()
// Desc: Lua ������ �ε��մϴ�.
////////////////////////////////////////////////////////////
bool CMyLua::Lua_LoadFile(const char* fileName)
{
	bool bResult = false;
	if (luaL_dofile(m_pLS, fileName) == 0)
	{
		bResult = true;
	}

	return bResult;
}

////////////////////////////////////////////////////////////
// Name: Lua_BeginTable()
// Desc: �ش� ���̺��� ���ÿ� �ø��ϴ�. 
// �ش� ���̺� ���� ��Ҹ� �����ϱ� ���� ����մϴ�.
////////////////////////////////////////////////////////////
bool CMyLua::Lua_BeginTable(const char* tableName)
{
	lua_getglobal(m_pLS, tableName);

	return lua_istable(m_pLS, -1);
}

////////////////////////////////////////////////////////////
// Name: Lua_EndTable()
// Desc: �ش� ���̺��� ���ÿ��� �����մϴ�.
// �ش� ���̺��� ��� �Ϸ� �� ȣ���� �ʿ��մϴ�.
////////////////////////////////////////////////////////////
void CMyLua::Lua_EndTable()
{
	lua_pop(m_pLS, 1);
}

////////////////////////////////////////////////////////////
// Name: Lua_GetStringFromTable()
// Desc: ���� ���̺��� key�� �ش��ϴ� value(string)�� ��ȯ�մϴ�. 
////////////////////////////////////////////////////////////
std::string CMyLua::Lua_GetStringFromTable(const char* key)
{
	lua_pushstring(m_pLS, key);
	lua_gettable(m_pLS, -2);

	std::string str = lua_tostring(m_pLS, -1);
	lua_pop(m_pLS, 1);

	return str;
}

////////////////////////////////////////////////////////////
// Name: Lua_GetString()
// Desc: �ش� (����) ������ ��Ʈ���� ��ȯ�մϴ�. 
////////////////////////////////////////////////////////////
std::string CMyLua::Lua_GetString(const char* valueName)
{
	lua_getglobal(m_pLS, valueName);

	std::string str = lua_tostring(m_pLS, -1);
	lua_pop(m_pLS, 1);

	return str;
}