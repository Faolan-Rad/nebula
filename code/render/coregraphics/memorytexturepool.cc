//------------------------------------------------------------------------------
//  memorytextureloader.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "render/stdneb.h"
#include "coregraphics/memorytexturepool.h"

#if __VULKAN__
namespace CoreGraphics
{
__ImplementClass(CoreGraphics::MemoryTexturePool, 'MTBL', Vulkan::VkMemoryTexturePool);
}
#else
#error "MemoryIndexBufferLoader class not implemented on this platform!"
#endif
