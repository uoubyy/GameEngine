/*
	This vertex shader is used to create a Direct3D vertex input layout object
*/

#include <Shaders/shaders.inc>

// Input
//======

DeclareInVariable( i_vertexData, App2VeretxData, 0 )

// Output
//=======

DeclareOutVariable( o_vertex2frag, Vertex2FragData, 1 )

// Entry Point
//============

vertex_main(void)
{
	// The shader program is only used to generate a vertex input layout object;
	// the actual shading code is never used
	o_vertex2frag.position = float4( i_vertexData.position, 1.0 );
	o_vertex2frag.uv = i_vertexData.texcoord;
	o_vertex2frag.color = i_vertexData.color;

#if defined( EAE6320_PLATFORM_GL )
	gl_Position = o_vertex2frag.position;
#endif
}
