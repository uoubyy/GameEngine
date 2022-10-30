#ifndef EAE6320_GRAPHICS_RENDER_COMMAND_H
#define EAE6320_GRAPHICS_RENDER_COMMAND_H

#include <Engine/Results/Results.h>
#include "../Math/cMatrix_transformation.h"

namespace eae6320
{
	namespace Graphics
	{
		struct sRenderCommand
		{
			class cMesh* m_mesh = nullptr;

			class cEffect* m_effect = nullptr;

			eae6320::Math::cMatrix_transformation m_transformation;

			cResult CleanUp();

			cResult SetRenderCommand( class cMesh* i_mesh, class cEffect* i_effect = nullptr );
		};
	}
}

#endif // EAE6320_GRAPHICS_RENDER_COMMAND_H