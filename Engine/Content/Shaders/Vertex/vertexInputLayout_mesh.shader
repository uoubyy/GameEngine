/*
	This vertex shader is used to create a Direct3D vertex input layout object
*/

#include <Shaders/shaders.inc>

// Input
//======

DeclareInVariable( i_vertexPosition_local, float3, 0 )
DeclareInVariable( i_vertexColor, float4, 1 )

// Output
//=======

DeclareOutVariable( o_vertexColor, float4, 0 )

// Entry Point
//============

vertex_main(void)
{
	// The shader program is only used to generate a vertex input layout object;
	// the actual shading code is never used
	o_vertexPosition_projected = float4( i_vertexPosition_local, 1.0 );
	o_vertexColor = i_vertexColor;
}
