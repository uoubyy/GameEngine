#ifndef EAE6320_RUNTIME_CMESH_COMPONENT_H
#define EAE6320_RUNTIME_CMESH_COMPONENT_H

#include <cstdint>
#include <string>

// Includes
//=========

#include "iComponent.h"
#include <Engine/Graphics/sRenderCommand.h>

namespace eae6320
{
	namespace Graphics
	{
		class cMesh;
		class cEffect;

		namespace VertexFormats
		{
			struct sVertex_mesh;
		}
	}
}

namespace eae6320
{
	namespace Runtime
	{
		class cMeshComponent final : public iComponent
		{
		public:

			// Initialization / Clean Up
			//--------------------------

			cMeshComponent();

			~cMeshComponent();

			eae6320::cResult InitializeGeometry( const Graphics::VertexFormats::sVertex_mesh* i_vertexData, const uint16_t* i_indices, const unsigned int i_triangleCount, const unsigned int i_vertexCount );

			eae6320::cResult InitializeShadingData( const std::string& i_vertexShaderPath, const std::string& i_fragmentShaderPath );

			eae6320::cResult ChangeMesh( class Graphics::cMesh* i_mesh );

			eae6320::cResult ChangeEffect( class Graphics::cEffect* i_effect );

			cResult GenerateRenderData( eae6320::Graphics::sRenderCommand& i_renderCommand );

			cResult CleanUp();

			inline bool IsVisible() { return m_visible; }

			inline void SetVisible( bool i_visible ) { m_visible = i_visible; }

		private:
			// Data
			class Graphics::cMesh* m_mesh = nullptr;

			class Graphics::cEffect* m_effect = nullptr;

			bool m_visible = true;
		};
	}
}

#endif // EAE6320_RUNTIME_CMESH_COMPONENT_H