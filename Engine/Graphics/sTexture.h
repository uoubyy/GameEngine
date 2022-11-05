#ifndef EAE6320_GRAPHICS_STEXTURE_H
#define EAE6320_GRAPHICS_STEXTURE_H

// Includes
//=========

#include <cstdint>

namespace eae6320
{
	namespace Graphics
	{
		enum eTextureType : uint8_t 
		{
			BaseColorTexture = 1 << 0,

			SpecularTexture = 1 << 1,

			AmbientTexture = 1 << 2,

			NormalTexture = 1 << 3,

			TransparencyTexture = 1 << 4,
		};

		struct sTexture 
		{
			eTextureType m_textureType;

			unsigned int m_textureID = 0;

			char* m_textureFilePath = nullptr;
		};
	}
}

#endif // EAE6320_GRAPHICS_STEXTURE_H