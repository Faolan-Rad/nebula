//------------------------------------------------------------------------------
//  lights_cluster.fx
//  (C) 2019 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/shared.fxh"
#include "lib/clustering.fxh"
#include "lib/lights_clustered.fxh"

group(BATCH_GROUP) constant LightUniforms [ string Visibility = "CS"; ]
{
	textureHandle SSAOBuffer;
	uint NumPointLights;
	uint NumSpotLights;
	uint NumClusters;
};

write rgba16f image2D Lighting;
write rgba16f image2D DebugOutput;

//------------------------------------------------------------------------------
/**
*/
[localsizex] = 64
shader 
void csCull()
{
	uint index1D = gl_GlobalInvocationID.x;

	if (index1D > NumClusters)
		return;

	ClusterAABB aabb = AABBs[index1D];

	uint flags = 0;

	// update pointlights
	uint numLights = 0;
	for (uint i = 0; i < NumPointLights; i++)
	{
		const PointLight light = PointLights[i];
		if (TestAABBSphere(aabb, light.position.xyz, light.position.w))
		{
			PointLightIndexList[index1D * MAX_LIGHTS_PER_CLUSTER + numLights] = i;
			numLights++;
		}
	}
	PointLightCountList[index1D] = numLights;

	// update feature flags if we have any lights
	if (numLights > 0)
		flags |= CLUSTER_POINTLIGHT_BIT;

	// update spotlights
	numLights = 0;
	for (uint i = 0; i < NumSpotLights; i++)
	{
		const SpotLight light = SpotLights[i];
		// first do fast discard sphere test
		if (TestAABBSphere(aabb, light.position.xyz, light.position.w))
		{
			// then do more refined cone test, if previous test passed
			if (TestAABBCone(aabb, light.position.xyz, light.forward.xyz, light.position.w, light.angleSinCos))
			{
				SpotLightIndexList[index1D * MAX_LIGHTS_PER_CLUSTER + numLights] = i;
				numLights++;
			}
		}		
	}
	SpotLightCountList[index1D] = numLights;

	// update feature flags if we have any lights
	if (numLights > 0)
		flags |= CLUSTER_SPOTLIGHT_BIT;

	atomicOr(AABBs[index1D].featureFlags, flags);
}

//------------------------------------------------------------------------------
/**
*/
[localsizex] = 64
shader
void csDebug()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	float depth = fetch2D(DepthBuffer, PosteffectSampler, coord, 0).r;

	// convert screen coord to view-space position
	vec4 viewPos = PixelToView(coord * InvFramebufferDimensions, depth);

	uint3 index3D = CalculateClusterIndex(coord / BlockSize, viewPos.z, InvZScale, InvZBias);
	uint idx = Pack3DTo1D(index3D, NumCells.x, NumCells.y);

	uint flag = AABBs[idx].featureFlags; // add 0 so we can read the value
	vec4 color = vec4(0, 0, 0, 0);
	if (CHECK_FLAG(flag, CLUSTER_POINTLIGHT_BIT))
	{
		uint count = PointLightCountList[idx];
		color.r = count / float(NumPointLights);
	}
	if (CHECK_FLAG(flag, CLUSTER_SPOTLIGHT_BIT))
	{
		uint count = SpotLightCountList[idx];
		color.g = count / float(NumSpotLights);
	}
	
	imageStore(DebugOutput, int2(coord), color);
}

//------------------------------------------------------------------------------
/**
*/
vec3 
LocalLights(
	uint idx, 
	vec4 viewPos, 
	vec3 viewVec, 
	vec3 normal, 
	float depth, 
	vec4 material, 
	vec4 albedo)
{
	vec3 light = vec3(0, 0, 0);
	uint flag = AABBs[idx].featureFlags;
	if (CHECK_FLAG(flag, CLUSTER_POINTLIGHT_BIT))
	{
		// shade point lights
		uint count = PointLightCountList[idx];
		PointLightShadowExtension ext;
		for (int i = 0; i < count; i++)
		{
			uint lidx = PointLightIndexList[idx * MAX_LIGHTS_PER_CLUSTER + i];
			PointLight li = PointLights[lidx];
			light += CalculatePointLight(
				li,
				ext,
				viewPos.xyz,
				viewVec,
				normal,
				depth,
				material,
				albedo
			);
		}
	}
	if (CHECK_FLAG(flag, CLUSTER_SPOTLIGHT_BIT))
	{
		uint count = SpotLightCountList[idx];
		SpotLightShadowExtension shadowExt;
		SpotLightProjectionExtension projExt;
		for (int i = 0; i < count; i++)
		{
			uint lidx = SpotLightIndexList[idx * MAX_LIGHTS_PER_CLUSTER + i];
			SpotLight li = SpotLights[lidx];

			// if we have extensions, load them from their respective buffers
			if (li.shadowExtension != -1)
				shadowExt = SpotLightShadow[li.shadowExtension];
			if (li.projectionExtension != -1)
				projExt = SpotLightProjection[li.projectionExtension];

			light += CalculateSpotLight(
				li,
				projExt,
				shadowExt,
				viewPos.xyz,
				viewVec,
				normal,
				depth,
				material,
				albedo
			);
		}
	}
	return light;
}

//------------------------------------------------------------------------------
/**
	Calculate pixel light contribution
*/
[localsizex] = 64
shader
void csRender()
{
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	vec4 normal = fetch2D(NormalBuffer, PosteffectSampler, coord, 0).rgba;
	float depth = fetch2D(DepthBuffer, PosteffectSampler, coord, 0).r;
	vec4 material = fetch2D(SpecularBuffer, PosteffectSampler, coord, 0).rgba;
	//material[MAT_ROUGHNESS] = 1 - material[MAT_ROUGHNESS] ;
	// material.a = 1.0f;
    float ssao = 1.0f - fetch2D(SSAOBuffer, PosteffectSampler, coord, 0).r;
	float ssaoSq = ssao * ssao;
	//material = vec4(1, 0, 1.0, 1.0f);
	vec4 albedo = fetch2D(AlbedoBuffer, PosteffectSampler, coord, 0).rgba;

	// convert screen coord to view-space position
	vec4 viewPos = PixelToView(coord * InvFramebufferDimensions, depth);
	vec4 worldPos = ViewToWorld(viewPos);
	vec3 worldViewVec = normalize(EyePos.xyz - worldPos.xyz);
	vec3 viewVec = -normalize(viewPos.xyz);
	vec3 viewNormal = (View * vec4(normal.xyz, 0)).xyz;

	uint3 index3D = CalculateClusterIndex(coord / BlockSize, viewPos.z, InvZScale, InvZBias); 
	uint idx = Pack3DTo1D(index3D, NumCells.x, NumCells.y);

	vec3 light = vec3(0,0,0); 

	// render lights where we have geometry
	if (normal.a != -1.0f)
	{
		// render global light
		light += CalculateGlobalLight(viewPos, worldViewVec, normal.xyz, depth, material, albedo);

		// render local lights
		// TODO: new material model for local lights
		light += LocalLights(idx, viewPos, viewVec, viewNormal, depth, material, albedo);

		// reflections and irradiance
		vec3 F0 = vec3(0.04);
		CalculateF0(albedo.rgb, material[MAT_METALLIC], F0);
		vec3 reflectVec = reflect(-worldViewVec, normal.xyz);
		float cosTheta = dot(normal.xyz, worldViewVec);
		vec3 F = FresnelSchlickGloss(F0, cosTheta, material[MAT_ROUGHNESS]);
		vec3 reflection = sampleCubeLod(EnvironmentMap, CubeSampler, reflectVec, material[MAT_ROUGHNESS] * NumEnvMips).rgb * GlobalLightColor.xyz;
        vec3 irradiance = sampleCubeLod(IrradianceMap, CubeSampler, normal.xyz, 0).rgb * GlobalLightColor.xyz;
		float cavity = material[MAT_CAVITY];
		
		vec3 kD = vec3(1.0f) - F;
		kD *= 1.0f - material[MAT_METALLIC];

		const vec3 ambientTerm = (irradiance * kD * albedo.rgb) * ssao;
		light += (ambientTerm + reflection * F) * cavity;
   	}	
	else // sky pixels
	{
		light += sampleCubeLod(EnvironmentMap, CubeSampler, normalize(worldPos.xyz), 0).rgb;
	}
    
	// write final output
	imageStore(Lighting, coord, light.xyzx);
}

//------------------------------------------------------------------------------
/**
*/
program Cull [ string Mask = "Cull"; ]
{
	ComputeShader = csCull();
};

//------------------------------------------------------------------------------
/**
*/
program Debug [ string Mask = "Debug"; ]
{
	ComputeShader = csDebug();
};

//------------------------------------------------------------------------------
/**
*/
program Render [ string Mask = "Render"; ]
{
	ComputeShader = csRender();
};
