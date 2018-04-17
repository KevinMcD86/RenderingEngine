//***************************************************************************************
// TreeSprite.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightingUtil.hlsl"

Texture2DArray gTreeMapArray : register(t0);


SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
	float4x4 gTexTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;

	float4 gFogColor;
	float gFogStart;
	float gFogRange;
	float2 cbPerObjectPad2;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
};

cbuffer cbMaterial : register(b2)
{
	float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
	float4x4 gMatTransform;
};
 
struct VertexIn
{
	float3 PosL  : POSITION;
	float3 NormalL : NORMAL;
};

struct VertexOut
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
};

struct GeoOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW	   : POSITION;
    uint   PrimID  : SV_PrimitiveID;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Just pass data over to geometry shader.
	vout.PosL = vin.PosL;
	vout.NormalL   = vin.NormalL;

	return vout;
}
 
 // We expand each point into a line (2 vertices), so the maximum number of vertices
 // we output per geometry shader invocation is 2.
[maxvertexcount(2)]
void GS(point VertexOut gin[1], 
        uint primID : SV_PrimitiveID, 
        inout LineStream<GeoOut> lineStream)
{	
	//
	// Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.
	//

	//float3 up = float3(0.0f, 1.0f, 0.0f);
	//float3 look = gEyePosW - gin[0].CenterW;
	//look.y = 0.0f; // y-axis aligned, so project to xz-plane
	//look = normalize(look);
	//float3 right = cross(up, look);

	////
	//// Compute triangle strip vertices (quad) in world space.
	////
	//float halfWidth  = 0.5f*gin[0].SizeW.x;
	//float halfHeight = 0.5f*gin[0].SizeW.y;
	float normalLineLength = 2.0f;
	
	float4 v[2];
	v[0] = float4(gin[0].PosL, 1.0f);
	v[1] = float4(gin[0].PosL + normalLineLength*gin[0].NormalL, 1.0f);

	//
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	//
	
	GeoOut gout;
	[unroll]
	for(int i = 0; i < 2; ++i)
	{
		gout.PosH     = mul(mul(v[i], gWorld), gViewProj);
		gout.PosW	  = mul(v[i], gWorld);
		gout.PrimID   = primID;
		
		lineStream.Append(gout);
	}
}

float4 PS(GeoOut pin) : SV_Target
{
    float4 litColor = float4(1.0f, 0.0f, 0.0f, 1.0f);

#ifdef FOG
	float3 toEyeW = gEyePosW - pin.PosW;
	float distToEye = length(toEyeW);

	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif

    //litColor.a = 1.0f;

    return litColor;
}


