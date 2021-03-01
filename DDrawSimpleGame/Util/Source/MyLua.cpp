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
// Desc: Lua 파일을 로드합니다.
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
// Desc: 해당 테이블을 스택에 올립니다. 
// 해당 테이블 내의 요소를 참조하기 전에 사용합니다.
////////////////////////////////////////////////////////////
bool CMyLua::Lua_BeginTable(const char* tableName)
{
	lua_getglobal(m_pLS, tableName);

	return lua_istable(m_pLS, -1);
}

////////////////////////////////////////////////////////////
// Name: Lua_EndTable()
// Desc: 해당 테이블을 스택에서 제거합니다.
// 해당 테이블의 사용 완료 후 호출이 필요합니다.
////////////////////////////////////////////////////////////
void CMyLua::Lua_EndTable()
{
	lua_pop(m_pLS, 1);
}

////////////////////////////////////////////////////////////
// Name: Lua_GetStringFromTable()
// Desc: 현재 테이블에서 key에 해당하는 value(string)를 반환합니다. 
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
// Desc: 해당 (전역) 변수의 스트링을 반환합니다. 
////////////////////////////////////////////////////////////
std::string CMyLua::Lua_GetString(const char* valueName)
{
	lua_getglobal(m_pLS, valueName);

	std::string str = lua_tostring(m_pLS, -1);
	lua_pop(m_pLS, 1);

	return str;
}