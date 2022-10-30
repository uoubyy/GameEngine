#ifndef EAE6320_GRAPHICS_CEFFECT_H
#define EAE6320_GRAPHICS_CEFFECT_H

// Includes
//=========

#include "cRenderState.h"

#include <Engine/Assets/ReferenceCountedAssets.h>

#include <cstdint>
#include <Engine/Results/Results.h>
#include <string>

#ifdef EAE6320_PLATFORM_GL
#include "OpenGL/Includes.h"
#endif

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Graphics
	{
		class cShader;
		class cRenderState;
	}
}

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cEffect
		{
			// Interface
			//==========

		public:

			// Initialization / Clean Up
			//--------------------------

			static cResult Load( const std::string& i_vertexShaderPath, const std::string& i_fragmentShaderPath, cEffect*& o_effect );

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS( cEffect );

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// Data
			//=====

		public:
		
		protected:

			cShader* m_vertexShader = nullptr;
			cShader* m_fragmentShader = nullptr;

#if defined( EAE6320_PLATFORM_GL )
			GLuint m_programId = 0;
#endif

			cRenderState m_renderState;

		private:

			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Initialization / Clean Up
			//--------------------------

			cResult Initialize( const std::string& i_vertexShaderPath, const std::string& i_fragmentShaderPath );

#if defined( EAE6320_PLATFORM_GL )
			cResult InitializeShadingProgram();
#endif

			cResult CleanUp();

#if defined( EAE6320_PLATFORM_GL )
			cResult CleanUpShadingProgram();
#endif

			cEffect();
			~cEffect();

		public:
			void Bind();

		};
	}
}

#endif // EAE6320_GRAPHICS_CEFFECT_H