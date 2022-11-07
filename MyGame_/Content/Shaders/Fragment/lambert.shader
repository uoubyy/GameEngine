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

DeclareInVariable( i_vertex2frag, Vertex2FragData, 1 )

// Output
//=======
DeclareOutVariable( o_color, float4, 0 )

DeclareSampler2D( baseColorTexture, 0 )
DeclareSampler2D( normalTexture, 1 )
DeclareSampler2D( specularColorTexture, 2 )

uniform float transparency;

// Entry Point
//============

frag_main(void)
{
	float3 albedo = tex2D( baseColorTexture, i_vertex2frag.uv ).rgb;

	// ambient
    float ambientStrength = 0.1;
    float3 ambient = ambientStrength * albedo;

	 // diffuse 
    float3 norm = tex2D( normalTexture, i_vertex2frag.uv ).rgb;
    float3 lightDir = normalize( g_light_position - i_vertex2frag.position.xyz );
    float diff = max( dot( norm, lightDir ), 0.0 );
    float3 diffuse = diff * albedo;

	// specular
    float3 specularStrength = tex2D( specularColorTexture, i_vertex2frag.uv ).rgb;
    float3 viewDir = normalize( g_view_position - i_vertex2frag.position.xyz );
    float3 reflectDir = reflect( -lightDir, norm );  
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), 32 );
    float3 specular = specularStrength * spec * g_light_color;  

	o_color = float4( ambient + diffuse + specular, 1.0 );
}
