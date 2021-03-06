#pragma once
//------------------------------------------------------------------------------
/**
	A shader read-write buffer can be both read and written from within a shader.

	(C)2017-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "ids/id.h"
#include "ids/idpool.h"
#include "util/stringatom.h"
#include "coregraphics/config.h"

namespace CoreGraphics
{
ID_24_8_TYPE(ShaderRWBufferId);

struct ShaderRWBufferCreateInfo
{
	Util::StringAtom name;
	SizeT size;
	BufferUpdateMode mode;
	bool screenRelative : 1;	// when set, size is bytes per pixel
};

/// create shader RW buffer
const ShaderRWBufferId CreateShaderRWBuffer(const ShaderRWBufferCreateInfo& info);
/// destroy shader RW buffer
void DestroyShaderRWBuffer(const ShaderRWBufferId id);

/// update data in buffer, don't forget to run SubmitChanges before you're done!
void ShaderRWBufferUpdate(const ShaderRWBufferId id, void* data, SizeT bytes);
/// submit changes done to the buffer
void ShaderRWBufferFlush(const ShaderRWBufferId id);

} // CoreGraphics
