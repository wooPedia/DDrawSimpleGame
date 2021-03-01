// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


//#define USE_RADIAL_DEVICE
#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00
//#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//#include "targetver.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#define SAFE_DELETE(p) { if(p != nullptr) delete p; p = nullptr; } 
#define IN 
#define OUT  

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <memory>
#include <ctype.h>
#include <combaseapi.h>
#include <Ole2.h>
#include <initguid.h>
#include <math.h>
#include <float.h>
#include <cassert>
#include <dbghelp.h>
#include "DDraw.h"
#include "../Util/Header/QueryPerfCounter.h"
#include "../DataStructure/typedef.h"

#include <XInput.h>

//#include "RenderComponent.h"
//#include "AnimationComponent.h"
//#include "PhysicsComponent.h"
//#include "MoveComponent.h"
//#include "PowerComponent.h"
//#include "HealthComponent.h"
//#include "LifeStateComponent.h"
//
//#include "GameObject.h"