/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

#if defined( EAE6320_PLATFORM_D3D )

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )

	#define vertex_main(void) void main( in const float3 i_vertexPosition_local : POSITION, out float4 o_vertexPosition_projected : SV_POSITION, in const float4 i_vertexColor : COLOR, out float4 o_vertexColor : COLOR )
	#define frag_main(void) void main( in const float4 i_fragmentPosition : SV_POSITION, in const float4 o_vertexColor : COLOR, out float4 o_color : SV_TARGET )

	#define DeclareInVariable( i_name, i_type, i_location )
	#define DeclareOutVariable( i_name, i_type, i_location )

#elif defined( EAE6320_PLATFORM_GL )

	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name
	#define float4x4 mat4
	#define float2 vec2
	#define float3 vec3
	#define float4 vec4

	#define vertex_main(void) void main()
	#define frag_main(void) void main()

	#define mul( i_matrix, i_vec ) i_matrix * i_vec

	#define DeclareInVariable( i_name, i_type, i_location ) layout( location = i_location ) in i_type i_name;
	#define DeclareOutVariable( i_name, i_type, i_location ) layout( location = i_location ) out i_type i_name;
	
	#define o_vertexPosition_projected gl_Position

#endif
