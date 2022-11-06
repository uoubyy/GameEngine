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
	struct App2VeretxData 
	{
		float3 position : POSITION;
		float3 normal : NORMAL;
		float3 tangent : TANGENT;
		float2 texcoord : TEXCOORD0;
		float4 color : COLOR;
	};

	struct Vertex2FragData
	{
		float4 position : SV_POSITION;
		float2 uv : TEXCOORD0;
		float4 color : COLOR;
	};

	#define uniform

	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )

	#define vertex_main(void) void main( in const App2VeretxData i_vertexData, out Vertex2FragData o_vertex2frag )
	#define frag_main(void) void main( in const Vertex2FragData i_vertex2frag, out float4 o_color : SV_TARGET )

	#define DeclareInVariable( i_name, i_type, i_location )
	#define DeclareOutVariable( i_name, i_type, i_location )

	#define DeclareSampler2D( i_name, i_id ) Texture2D i_name : register( t##i_id ); SamplerState sampler_##i_name : register( s##i_id );

	#define tex2D( i_texture, i_uv ) ( i_texture.Sample( sampler_##i_texture, i_uv) )

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

	struct App2VeretxData
	{
		float3 position;
		float3 normal;
		float3 tangent;
		float2 texcoord;
		float4 color;
	};

	struct Vertex2FragData
	{
		float4 position;
		float2 uv;
		float4 color;
	};

	float4 tex2D( sampler2D x, float2 v ) { return texture( x, v ); }

	#define DeclareSampler2D( i_name, i_id ) uniform sampler2D i_name;

#endif
