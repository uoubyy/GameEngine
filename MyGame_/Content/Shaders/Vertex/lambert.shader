/*
	This is the standard vertex shader

	A vertex shader is responsible for two things:
		* Telling the GPU where the vertex (one of the three in a triangle) should be drawn on screen in a given window
			* The GPU will use this to decide which fragments (i.e. pixels) need to be shaded for a given triangle
		* Providing any data that a corresponding fragment shader will need
			* This data will be interpolated across the triangle and thus vary for each fragment of a triangle that gets shaded
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
	
	float3 g_view_position;
};

DeclareConstantBuffer( g_constantBuffer_drawCall, 2 )
{
    float4x4 g_transform_localToWorld;
	float3 g_light_position;
	float3 g_light_color;
};

// Input
//======

DeclareInVariable( i_vertexData, App2VeretxData, 0 )

// Output
//======

DeclareOutVariable( o_vertex2frag, Vertex2FragData, 1 )


// Entry Point
//============

vertex_main(void)
{
	// Transform the local vertex into world space
	float4 vertexPosition_world;
	{
		// This will be done in a future assignment.
		// For now, however, local space is treated as if it is the same as world space.
		float4 vertexPosition_local = float4( i_vertexData.position, 1.0 );
		vertexPosition_world = mul( g_transform_localToWorld, vertexPosition_local );
	}
	// Calculate the position of this vertex projected onto the display
	{
		// Transform the vertex from world space into camera space
		float4 vertexPosition_camera = mul( g_transform_worldToCamera, vertexPosition_world );
		// Project the vertex from camera space into projected space
		o_vertex2frag.position = mul( g_transform_cameraToProjected, vertexPosition_camera );
	}

	o_vertex2frag.uv = i_vertexData.texcoord;
	o_vertex2frag.color = i_vertexData.color;

#if defined( EAE6320_PLATFORM_GL )
	gl_Position = o_vertex2frag.position;
#endif
}
