#ifndef EAE6320_GRAPHICS_STEXTURE_H
#define EAE6320_GRAPHICS_STEXTURE_H

// Includes
//=========

#include <cstdint>
#include <Engine/Results/Results.h>

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;
#endif

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
#if defined( EAE6320_PLATFORM_D3D )
			ID3D11Texture2D* m_texture = nullptr;
			ID3D11ShaderResourceView* m_shaderResourceView = nullptr;
			ID3D11SamplerState* m_samplerState = nullptr;
#elif defined( EAE6320_PLATFORM_GL )
			unsigned int m_textureID = 0;
#endif
			eTextureType m_textureType;

			cResult CleanUp();
		};
	}
}

#endif // EAE6320_GRAPHICS_STEXTURE_H