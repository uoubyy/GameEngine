/*
	A mesh is a representation of geometry information.
*/
#ifndef EAE6320_GRAPHICS_CMESH_H
#define EAE6320_GRAPHICS_CMESH_H

// Includes
//=========

#include <Engine/Assets/ReferenceCountedAssets.h>

#include <cstdint>
#include <Engine/Results/Results.h>
#include <string>

#ifdef EAE6320_PLATFORM_GL
#include "OpenGL/Includes.h"
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cVertexFormat;

		namespace VertexFormats
		{
			struct sVertex_mesh;
		}
	}
}

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11Buffer;
#endif

namespace eae6320
{
	namespace Graphics
	{
		class cMesh
		{
			// Interface
			//==========

		public:

			// Initialization / Clean Up
			//--------------------------

			static cResult Load( const std::string& i_meshPath, cMesh*& o_mesh );

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS( cMesh );

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// Data
			//=====

		protected:

#if defined( EAE6320_PLATFORM_D3D )
			cVertexFormat* m_vertexFormat = nullptr;

			ID3D11Buffer* m_vertexBuffer = nullptr;

			ID3D11Buffer* m_indexBuffer = nullptr;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint m_vertexArrayId = 0;

			GLuint m_vertexBufferId = 0;

			GLuint m_elementBufferId = 0;
#endif
			unsigned int m_triangleCount;
			
		private:
			
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Initialization / Clean Up
			//--------------------------

			cResult Initialize( const VertexFormats::sVertex_mesh* i_vertexData, const void* i_indices, const uint32_t i_triangleCount, const uint32_t i_vertexCount );

			cResult CleanUp();

			cMesh();
			~cMesh();

		public:

			// Render
			//-------
			void Draw();
		};
	}
}

#endif