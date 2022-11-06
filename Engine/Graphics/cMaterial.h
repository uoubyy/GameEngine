
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
		struct sMaterialInfo
		{
			float m_baseColor[3];
			std::string m_baseColorTexturePath;

			float m_specularColor[3];
			std::string m_specularColorTexturePath;

			float m_ambient[3];
			std::string m_ambientColorTexturePath;

			float m_transparency[3];
			std::string m_transparencyTexturePath;

			std::string m_normalTexturePath;

			struct
			{
				uint32_t first = std::numeric_limits<uint32_t>::max(), last = 0;
			} m_vertexRange;

			struct
			{
				uint32_t first = std::numeric_limits<uint32_t>::max(), last = 0;
			} m_indexRange;

			sMaterialInfo() : m_baseColor{ 0, 0, 0 }, m_baseColorTexturePath( "" ), m_specularColor{ 0, 0, 0 }, m_specularColorTexturePath( "" ), m_ambient{ 0, 0, 0 },
				m_ambientColorTexturePath( "" ), m_transparency{ 0, 0, 0 }, m_transparencyTexturePath( "" ), m_normalTexturePath( "" ) {}
		};

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

			cResult Initialize( const sMaterialInfo& i_materialInfo );

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