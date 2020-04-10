//------------------------------------------------------------------------------
//  inputserver.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "render/stdneb.h"
#include "input/inputserver.h"

namespace Input
{
#if __VULKAN__
__ImplementClass(Input::InputServer, 'INPS', GLFW::GLFWInputServer);
__ImplementInterfaceSingleton(Input::InputServer);
#elif __WIN32__
__ImplementClass(Input::InputServer, 'INPS', Win32::Win32InputServer);
__ImplementInterfaceSingleton(Input::InputServer);
#else
#error "InputServer class not implemented on this platform!"
#endif

//------------------------------------------------------------------------------
/**
*/
InputServer::InputServer()
{
    __ConstructInterfaceSingleton;
}

//------------------------------------------------------------------------------
/**
*/
InputServer::~InputServer()
{
    __DestructInterfaceSingleton;
}

} // namespace Input
