// Includes
//=========

#include "../cMaterial.h"
#include "../cEffect.h"
#include "../sContext.h"

#include "Includes.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

// Helper Class Declaration
//=========================

namespace
{
	eae6320::cResult LoadTexture( const std::string& i_path, eae6320::Graphics::sTexture& o_texture );
}

// Implementation
//===============

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cMaterial::Initialize( const eae6320::Graphics::sMaterialInfo& i_materialInfo )
{
	auto result = eae6320::Results::Success;

	if ( !( result = eae6320::Graphics::cEffect::Load( "data/Shaders/Vertex/lambert.shader", "data/Shaders/Fragment/lambert.shader", m_effect ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize shading data." );
	}

	for ( auto i = 0; i < 3; ++i )
	{
		m_baseColor[i] = i_materialInfo.m_baseColor[i];
		m_specularColor[i] = i_materialInfo.m_specularColor[i];
		m_ambient[i] = i_materialInfo.m_ambient[i];
		m_transparency[i] = i_materialInfo.m_transparency[i];
	}

	m_vertexRange.first = i_materialInfo.m_vertexRange.first;
	m_vertexRange.last = i_materialInfo.m_vertexRange.last;

	m_indexRange.first = i_materialInfo.m_indexRange.first;
	m_indexRange.last = i_materialInfo.m_indexRange.last;

	const std::string meshPathDictionary = "data/Meshes/";

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT( direct3dDevice );

	// Load Base Color Texture
	{
		if ( i_materialInfo.m_baseColorTexturePath != "" )
		{
			result = LoadTexture( meshPathDictionary + i_materialInfo.m_baseColorTexturePath, m_baseColorTexture );
		}
	}

	// Load Specular Color Texture
	{
		if ( i_materialInfo.m_specularColorTexturePath != "" )
		{
			result = LoadTexture( meshPathDictionary + i_materialInfo.m_specularColorTexturePath, m_specularColorTexture );
		}
	}

	// Load ambient
	{
		if ( i_materialInfo.m_ambientColorTexturePath != "" )
		{
			result = LoadTexture( meshPathDictionary + i_materialInfo.m_ambientColorTexturePath, m_ambientColorTexture );
		}
	}

	// Load normal texture 
	{
		if ( i_materialInfo.m_normalTexturePath != "" )
		{
			result = LoadTexture( meshPathDictionary + i_materialInfo.m_normalTexturePath, m_normalTexture );
		}
	}

	// Load transparency texture
	{
		if ( i_materialInfo.m_transparencyTexturePath != "" )
		{
			result = LoadTexture( meshPathDictionary + i_materialInfo.m_transparencyTexturePath, m_transparencyTexture );
		}
	}

	return result;
}

void eae6320::Graphics::cMaterial::Bind()
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT( direct3dImmediateContext );

	m_effect->Bind();

	if( m_baseColorTexture.m_shaderResourceView && m_baseColorTexture.m_samplerState )
	{
		direct3dImmediateContext->PSSetShaderResources( 0, 1, &m_baseColorTexture.m_shaderResourceView );
		direct3dImmediateContext->PSSetSamplers( 0, 1, &m_baseColorTexture.m_samplerState );
	}
}

namespace
{
	eae6320::cResult LoadTexture( const std::string& i_path, eae6320::Graphics::sTexture& o_texture )
	{
		auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
		EAE6320_ASSERT( direct3dDevice );

		std::string errorMessage;

		unsigned char* initialData = nullptr;
		int width = 0, height = 0, components = 0;

		eae6320::cScopeGuard scopeGuard( [&initialData]
			{
				if ( initialData )
				{
					free(initialData);
				}
			} );

		auto result = eae6320::Platform::LoadTextureFile( i_path.c_str(), initialData, width, height, components, 0, &errorMessage );

		if ( !result )
		{
			eae6320::Logging::OutputError( "Failed to load texture from file %s: %s", i_path.c_str(), errorMessage.c_str() );
		}

		const auto textureDescription = [width, height, components]
		{
			D3D11_TEXTURE2D_DESC textureDescription{};
			
			textureDescription.Width = width;
			textureDescription.Height = height;
			textureDescription.MipLevels = 1;
			textureDescription.ArraySize = 1;
			textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			textureDescription.SampleDesc.Count = 1;
			textureDescription.SampleDesc.Quality = 0;
			textureDescription.Usage = D3D11_USAGE_IMMUTABLE;
			textureDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDescription.CPUAccessFlags = 0;
			textureDescription.MiscFlags = 0;

			return textureDescription;
		}();

		const auto textureInitialData = [width, height, initialData]
		{
			D3D11_SUBRESOURCE_DATA textureInitialData{};

			textureInitialData.pSysMem = initialData;
			textureInitialData.SysMemPitch = width * sizeof(uint32_t);
			textureInitialData.SysMemSlicePitch = width * height * sizeof(uint32_t);

			return textureInitialData;
		}();

		{
			const auto result_create = direct3dDevice->CreateTexture2D( &textureDescription, &textureInitialData, &o_texture.m_texture );
			if ( FAILED( result_create ) )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, "Texture2D object creation failed (HRESULT %#010x)", result_create );
				eae6320::Logging::OutputError( "Direct3D failed to create a Texture2D object (HRESULT %#010x)", result_create );
				return result;
			}
		}

		const auto shaderResourceViewDescription = []
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescription;

			shaderResourceViewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			shaderResourceViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDescription.Texture2D.MipLevels = 1;
			shaderResourceViewDescription.Texture2D.MostDetailedMip = 0;

			return shaderResourceViewDescription;
		}();

		{
			const auto result_create = direct3dDevice->CreateShaderResourceView( o_texture.m_texture, &shaderResourceViewDescription, &o_texture.m_shaderResourceView );
			if ( FAILED( result_create ) )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, "Shader Resource View object creation failed (HRESULT %#010x)", result_create );
				eae6320::Logging::OutputError( "Direct3D failed to create a Shader Resource View object (HRESULT %#010x)", result_create );
				return result;
			}
		}

		const auto samplerStateDescription = []
		{
			D3D11_SAMPLER_DESC samplerStateDescription{};

			samplerStateDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerStateDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerStateDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerStateDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerStateDescription.MipLODBias = 0.0f;
			samplerStateDescription.MaxAnisotropy = 1;
			samplerStateDescription.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerStateDescription.BorderColor[0] = 0;
			samplerStateDescription.BorderColor[1] = 0;
			samplerStateDescription.BorderColor[2] = 0;
			samplerStateDescription.BorderColor[3] = 0;
			samplerStateDescription.MinLOD = 0;
			samplerStateDescription.MaxLOD = D3D11_FLOAT32_MAX;

			return samplerStateDescription;
		}();

		{
			const auto result_create = direct3dDevice->CreateSamplerState( &samplerStateDescription, &o_texture.m_samplerState );
			if ( FAILED( result_create ) )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, "Sampler State creation failed (HRESULT %#010x)", result_create );
				eae6320::Logging::OutputError( "Direct3D failed to create a Sampler State (HRESULT %#010x)", result_create );
				return result;
			}
		}

		return result;
	}
}