/*
	This is the standard fragment shader

	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>

// Constant Buffers
//=================

DeclareConstantBuffer( g_constantBuffer_frame, 0 )
{
	float4x4 g_transform_worldToCamera;
	float4x4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For float4 alignment
	float2 g_padding;
};

// Input
//======

DeclareInVariable( i_vertex2frag, Vertex2FragData, 1 )

// Output
//=======
DeclareOutVariable( o_color, float4, 0 )

DeclareSampler2D( baseColorTexture, 0 )
uniform float transparency;
uniform float3 baseColor;

// Entry Point
//============

frag_main(void)
{
	float3 albedo = tex2D( baseColorTexture, i_vertex2frag.uv ).rgb;
	o_color = float4( albedo.r, albedo.g, albedo.b, 1.0 );
}
