#include "cMaterial.h"
#include "cEffect.h"

#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

// Helper Class Declaration
//=========================

namespace
{
	eae6320::cResult Loadmaterial( const void* i_dataBuffer, uint32_t& o_dataOffset, eae6320::Graphics::sMaterialInfo& o_materialInfo );
}

// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cMaterial::Load( const void* i_dataBuffer, uint32_t& o_dataOffset, cMaterial*& o_material )
{
	auto result = Results::Success;

	cMaterial* material = nullptr;

	cScopeGuard scopeGuard( [&result, &o_material, &material] 
		{
			if ( result )
			{
				EAE6320_ASSERT( material != nullptr );
				o_material = material;
			}
			else
			{
				if ( material )
				{
					material->DecrementReferenceCount();
					material = nullptr;
				}
				o_material = material;
			}
		} );

	// Allocate a new material
	{
		material = new (std::nothrow) cMaterial();
		if ( !material )
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF( false, "Couldn't allocate memory for a new material" );
			Logging::OutputError( "Failed to allocate memory for a new material" );
			return result;
		}
	}
	
	uint32_t currentOffset = 0;
	sMaterialInfo materialInfo;

	if ( !( result = Loadmaterial( i_dataBuffer, currentOffset, materialInfo ) ) )
	{
		EAE6320_ASSERTF( false, "Load material info failed!" );
		return result;
	}

	o_dataOffset = currentOffset;

	// Initialize the platform-specific material object
	if ( !( result = material->Initialize( materialInfo ) ) )
	{
		EAE6320_ASSERTF( false, "Initialization of new material failed!" );
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cMaterial::CleanUp()
{
	if ( m_effect )
	{
		m_effect->DecrementReferenceCount();
		m_effect = nullptr;
	}

	m_baseColorTexture.CleanUp();
	m_specularColorTexture.CleanUp();
	m_ambientColorTexture.CleanUp();
	m_transparencyTexture.CleanUp();
	m_normalTexture.CleanUp();

	return Results::Success;
}

eae6320::Graphics::cMaterial::cMaterial()
{
	m_baseColorTexture.m_textureType = eTextureType::BaseColorTexture;

	m_specularColorTexture.m_textureType = eTextureType::SpecularTexture;

	m_ambientColorTexture.m_textureType = eTextureType::AmbientTexture;

	m_normalTexture.m_textureType = eTextureType::NormalTexture;

	m_transparencyTexture.m_textureType = eTextureType::TransparencyTexture;
}

eae6320::Graphics::cMaterial::~cMaterial()
{
	const auto result = CleanUp();
	EAE6320_ASSERT( result );
}

namespace
{
	eae6320::cResult Loadmaterial( const void* i_dataBuffer, uint32_t& o_dataOffset, eae6320::Graphics::sMaterialInfo& o_materialInfo )
	{
		auto result = eae6320::Results::Success;

		auto currentOffset = reinterpret_cast<uintptr_t>( i_dataBuffer );

		char* filePath = new ( std::nothrow ) char[255];
		if ( !filePath )
		{
			return eae6320::Results::Failure;
		}
		// Load Base Color Texture
		{
			memcpy( reinterpret_cast<void*>( o_materialInfo.m_baseColor ), reinterpret_cast<void*>( currentOffset ), 3 * sizeof( float ) );
			currentOffset += 3 * sizeof( float );
			o_dataOffset += 3 * sizeof( float );

			uint8_t filePathLen = 0;
			memcpy( &filePathLen, reinterpret_cast<void*>( currentOffset ), sizeof( uint8_t ) );
			currentOffset += sizeof( uint8_t );
			o_dataOffset += sizeof( uint8_t );

			if ( filePathLen > 0 )
			{
				memset( filePath, 0, 255 );
				memcpy( reinterpret_cast<void*>( filePath ), reinterpret_cast<void*>( currentOffset ), filePathLen );

				o_materialInfo.m_baseColorTexturePath = filePath;
			}

			currentOffset += filePathLen;
			o_dataOffset += filePathLen;
		}

		// Load Specular Color Texture
		{
			memcpy( reinterpret_cast<void*>( o_materialInfo.m_specularColor ), reinterpret_cast<void*>( currentOffset ), 3 * sizeof( float ) );
			currentOffset += 3 * sizeof( float );
			o_dataOffset += 3 * sizeof( float );

			uint8_t filePathLen = 0;
			memcpy( &filePathLen, reinterpret_cast<void*>( currentOffset ), sizeof( uint8_t ) );
			currentOffset += sizeof( uint8_t );
			o_dataOffset += sizeof( uint8_t );

			if ( filePathLen > 0 )
			{
				memset( filePath, 0, 255 );
				memcpy( reinterpret_cast<void*>( filePath ), reinterpret_cast<void*>( currentOffset ), filePathLen );

				o_materialInfo.m_specularColorTexturePath = filePath;
			}

			currentOffset += filePathLen;
			o_dataOffset += filePathLen;
		}

		// Load Ambient Texture 
		{
			memcpy( reinterpret_cast<void*>( o_materialInfo.m_ambient ), reinterpret_cast<void*>( currentOffset ), 3 * sizeof( float ) );
			currentOffset += 3 * sizeof( float );
			o_dataOffset += 3 * sizeof( float );

			uint8_t filePathLen = 0;
			memcpy( &filePathLen, reinterpret_cast<void*>( currentOffset ), sizeof( uint8_t ) );
			currentOffset += sizeof( uint8_t );
			o_dataOffset += sizeof( uint8_t );

			if ( filePathLen > 0 )
			{
				memset( filePath, 0, 255 );
				memcpy( reinterpret_cast<void*>( filePath ), reinterpret_cast<void*>( currentOffset ), filePathLen );

				o_materialInfo.m_ambientColorTexturePath = filePath;
			}

			currentOffset += filePathLen;
			o_dataOffset += filePathLen;
		}

		// Load Transparency Texture 
		{
			memcpy( reinterpret_cast<void*>( o_materialInfo.m_transparency ), reinterpret_cast<void*>( currentOffset ), 3 * sizeof( float ) );
			currentOffset += 3 * sizeof( float );
			o_dataOffset += 3 * sizeof( float );

			uint8_t filePathLen = 0;
			memcpy( &filePathLen, reinterpret_cast<void*>( currentOffset ), sizeof( uint8_t ) );
			currentOffset += sizeof( uint8_t );
			o_dataOffset += sizeof( uint8_t );

			if ( filePathLen > 0 )
			{
				memset( filePath, 0, 255 );
				memcpy( reinterpret_cast<void*>( filePath ), reinterpret_cast<void*>( currentOffset ), filePathLen );

				o_materialInfo.m_transparencyTexturePath = filePath;
			}

			currentOffset += filePathLen;
			o_dataOffset += filePathLen;
		}

		// Load Normal Texture
		{
			uint8_t filePathLen = 0;
			memcpy( &filePathLen, reinterpret_cast<void*>( currentOffset ), sizeof( uint8_t ) );
			currentOffset += sizeof( uint8_t );
			o_dataOffset += sizeof( uint8_t );

			if ( filePathLen > 0 )
			{
				memset( filePath, 0, 255 );
				memcpy( reinterpret_cast<void*>( filePath ), reinterpret_cast<void*>( currentOffset ), filePathLen );

				o_materialInfo.m_normalTexturePath = filePath;
			}

			currentOffset += filePathLen;
			o_dataOffset += filePathLen;
		}

		// Load vertex range
		{
			memcpy( &o_materialInfo.m_vertexRange.first, reinterpret_cast<void*>( currentOffset ), sizeof( uint32_t ) );
			currentOffset += sizeof( uint32_t );
			o_dataOffset += sizeof( uint32_t );

			memcpy( &o_materialInfo.m_vertexRange.last, reinterpret_cast<void*>( currentOffset ), sizeof( uint32_t ) );
			currentOffset += sizeof( uint32_t );
			o_dataOffset += sizeof( uint32_t );
		}

		// Load index range
		{
			memcpy( &o_materialInfo.m_indexRange.first, reinterpret_cast<void*>( currentOffset ), sizeof( uint32_t ) );
			currentOffset += sizeof( uint32_t );
			o_dataOffset += sizeof( uint32_t );

			memcpy( &o_materialInfo.m_indexRange.last, reinterpret_cast<void*>( currentOffset ), sizeof( uint32_t ) );
			currentOffset += sizeof( uint32_t );
			o_dataOffset += sizeof( uint32_t );
		}

		return result;
	}
}