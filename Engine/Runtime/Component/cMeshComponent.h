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

			eae6320::cResult ChangeMesh( const std::string& i_mesh_file_path );

			eae6320::cResult ChangeMesh( class Graphics::cMesh* i_mesh );

			cResult GenerateRenderData( eae6320::Graphics::sRenderCommand& i_renderCommand );

			cResult CleanUp();

			inline bool IsVisible() { return m_visible; }

			inline void SetVisible( bool i_visible ) { m_visible = i_visible; }

		private:
			// Data
			class Graphics::cMesh* m_mesh = nullptr;

			//class Graphics::cEffect* m_effect = nullptr;

			bool m_visible = true;
		};
	}
}

#endif // EAE6320_RUNTIME_CMESH_COMPONENT_H