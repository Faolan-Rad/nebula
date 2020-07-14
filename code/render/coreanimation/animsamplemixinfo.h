#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreAnimation::AnimSampleMixInfo
    
    A data structure for providing sample/mixing attributes to 
    asynchronous jobs in the CoreAnimation subsystem.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2020 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "math/vec4.h"
#include "coreanimation/sampletype.h"

//------------------------------------------------------------------------------
namespace CoreAnimation
{
struct NEBULA_ALIGN16 AnimSampleMixInfo
{
    SampleType::Code sampleType;
    float sampleWeight;
    float mixWeight;
    Math::vec4 velocityScale;
};

} // namespace CoreAnimation   
//------------------------------------------------------------------------------
