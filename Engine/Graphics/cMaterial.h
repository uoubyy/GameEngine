
#ifndef EAE6320_GRAPHICS_CMATERIAL_H
#define EAE6320_GRAPHICS_CMATERIAL_H

// Includes
//=========

#include <Engine/Assets/ReferenceCountedAssets.h>

#include <Engine/Results/Results.h>

#include "sTexture.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cMaterial
		{
			// Interface
			//==========

		public:

			// Initialization / Clean Up
			//--------------------------

			static cResult Load( const void* i_dataBuffer, uint32_t& o_dataOffset, cMaterial*& o_material );

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS( cMaterial );

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// Data
			//=====
			struct
			{
				uint32_t first = std::numeric_limits<uint32_t>::max(), last = 0;
			} m_vertexRange;

			struct
			{
				uint32_t first = std::numeric_limits<uint32_t>::max(), last = 0;
			} m_indexRange;

		private:
			
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Initialization / Clean Up
			//--------------------------

			cResult Initialize( const void* i_dataBuffer, uint32_t& o_dataOffset );

			cResult CleanUp();

			cMaterial();
			~cMaterial();

			// Data
			//=====

			class cEffect* m_effect = nullptr;

			float m_baseColor[3] = { 0 };
			sTexture m_baseColorTexture;

			float m_specularColor[3] = { 0 };
			sTexture m_specularColorTexture;

			float m_ambient[3] = { 0 };
			sTexture m_ambientColorTexture;

			float m_transparency[3] = { 0 };
			sTexture m_transparencyTexture;

			sTexture m_normalTexture;
		public:
			
			void Bind();
		};
	}
}

#endif // EAE6320_GRAPHICS_CMATERIAL_H