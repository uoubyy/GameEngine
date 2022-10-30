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

DeclareInVariable( o_vertexColor, float4, 1 )

// Output
//=======
DeclareOutVariable( o_color, float4, 0 )

// Entry Point
//============

frag_main(void)
{
	// Output solid white
	// float r = sin( g_elapsedSecondCount_simulationTime ) * 0.5 + 0.5;
	// float g = cos( g_elapsedSecondCount_simulationTime ) * 0.5 + 0.5;
	// o_color = float4(
	// 	// RGB (color)
	// 	r, g, 1.0,
	// 	// Alpha (opacity)
	// 	1.0 );
	o_color = o_vertexColor;
}
