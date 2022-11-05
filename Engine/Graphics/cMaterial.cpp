#include "cMaterial.h"
#include "cEffect.h"

#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

#ifdef EAE6320_PLATFORM_GL
#include "OpenGL/Includes.h"
#endif

// Helper Class Declaration
//=========================

namespace
{
#ifdef EAE6320_PLATFORM_GL
	eae6320::cResult LoadTexture( const unsigned int i_textureID, const std::string& i_path, unsigned int i_wrap_s = GL_REPEAT, unsigned int i_wrap_t = GL_REPEAT );
#endif
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

	// Initialize the platform-specific material object
	uint32_t currentOffset = 0;
	if ( !( result = material->Initialize( i_dataBuffer, currentOffset ) ) )
	{
		EAE6320_ASSERTF( false, "Initialization of new material failed" );
		return result;
	}

	o_dataOffset = currentOffset;

	return result;
}

eae6320::cResult eae6320::Graphics::cMaterial::Initialize( const void* i_dataBuffer, uint32_t& o_dataOffset )
{
	auto result = Results::Success;
	// TODO
	// Load effect according to the shader model type
	if ( !( result = eae6320::Graphics::cEffect::Load( "data/Shaders/Vertex/lambert.shader", "data/Shaders/Fragment/lambert.shader", m_effect ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize shading data." );
	}

	auto currentOffset = reinterpret_cast<uintptr_t>( i_dataBuffer );

#if defined( EAE6320_PLATFORM_GL )
	char* filePath = new ( std::nothrow ) char[255];
	if ( !filePath )
	{
		return Results::Failure;
	}
	// Load Base Color Texture
	{
		memcpy( reinterpret_cast<void*>( m_baseColor ), reinterpret_cast<void*>( currentOffset ), 3 * sizeof( float ) );
		currentOffset += 3 * sizeof( float );
		o_dataOffset += 3 * sizeof( float );

		uint8_t filePathLen = 0;
		memcpy( &filePathLen, reinterpret_cast<void*>( currentOffset ), sizeof( uint8_t ) );
		currentOffset += sizeof( uint8_t );
		o_dataOffset += sizeof( uint8_t );
		
		if( filePathLen > 0 )
		{ 
			memset( filePath, 0, 255 );
			memcpy( reinterpret_cast<void*>( filePath ), reinterpret_cast<void*>( currentOffset ), filePathLen );

			glGenTextures( 1, &m_baseColorTexture.m_textureID );

			std::string texturePath = filePath;
			if ( !( result = LoadTexture( m_baseColorTexture.m_textureID, "data/Meshes/" + texturePath ) ) )
			{
				return Results::Failure;
			}
		}

		currentOffset += filePathLen;
		o_dataOffset += filePathLen;
	}

	// Load Specular Color Texture
	{
		memcpy( reinterpret_cast<void*>( m_specularColor ), reinterpret_cast<void*>( currentOffset ), 3 * sizeof( float ) );
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

			glGenTextures( 1, &m_specularColorTexture.m_textureID );

			std::string texturePath = filePath;
			if ( !( result = LoadTexture( m_specularColorTexture.m_textureID, "data/Meshes/" + texturePath ) ) )
			{
				return Results::Failure;
			}
		}

		currentOffset += filePathLen;
		o_dataOffset += filePathLen;
	}

	// Load Ambient Texture 
	{
		memcpy( reinterpret_cast<void*>( m_ambient ), reinterpret_cast<void*>( currentOffset ), 3 * sizeof( float ) );
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

			glGenTextures( 1, &m_ambientColorTexture.m_textureID );

			std::string texturePath = filePath;
			if ( !( result = LoadTexture( m_ambientColorTexture.m_textureID, "data/Meshes/" + texturePath ) ) )
			{
				return Results::Failure;
			}
		}

		currentOffset += filePathLen;
		o_dataOffset += filePathLen;
	}

	// Load Transparency Texture 
	{
		memcpy( reinterpret_cast<void*>( m_transparency ), reinterpret_cast<void*>( currentOffset ), 3 * sizeof( float ) );
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

			glGenTextures( 1, &m_transparencyTexture.m_textureID );

			std::string texturePath = filePath;
			if ( !( result = LoadTexture( m_transparencyTexture.m_textureID, "data/Meshes/" + texturePath ) ) )
			{
				return Results::Failure;
			}
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

			glGenTextures( 1, &m_normalTexture.m_textureID );

			std::string texturePath = filePath;
			if ( !( result = LoadTexture( m_normalTexture.m_textureID, "data/Meshes/" + texturePath ) ) )
			{
				return Results::Failure;
			}
		}

		currentOffset += filePathLen;
		o_dataOffset += filePathLen;
	}

	// Load vertex range
	{
		memcpy( &m_vertexRange.first, reinterpret_cast<void*>( currentOffset ), sizeof( uint32_t ) );
		currentOffset += sizeof( uint32_t );
		o_dataOffset += sizeof( uint32_t );

		memcpy( &m_vertexRange.last, reinterpret_cast<void*>( currentOffset ), sizeof( uint32_t ) );
		currentOffset += sizeof( uint32_t );
		o_dataOffset += sizeof( uint32_t );
	}

	// Load index range
	{
		memcpy( &m_indexRange.first, reinterpret_cast<void*>( currentOffset ), sizeof( uint32_t ) );
		currentOffset += sizeof( uint32_t );
		o_dataOffset += sizeof( uint32_t );

		memcpy( &m_indexRange.last, reinterpret_cast<void*>( currentOffset ), sizeof( uint32_t ) );
		currentOffset += sizeof( uint32_t );
		o_dataOffset += sizeof( uint32_t );
	}
#endif

	return result;
}

eae6320::cResult eae6320::Graphics::cMaterial::CleanUp()
{
	if( m_effect )
	{
		m_effect->DecrementReferenceCount();
		m_effect = nullptr;
	}
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

void eae6320::Graphics::cMaterial::Bind()
{
#if defined( EAE6320_PLATFORM_GL )
	m_effect->Bind();

	if( m_baseColorTexture.m_textureID != 0 )
	{
		glActiveTexture( GL_TEXTURE0 );
		glUniform1i( glGetUniformLocation( m_effect->GetShaderId(), "baseColorTexture" ), 0 );
		glBindTexture( GL_TEXTURE_2D, m_baseColorTexture.m_textureID );
	}

	{
		glUniform3f( glGetUniformLocation( m_effect->GetShaderId(), "baseColor" ), m_baseColor[0], m_baseColor[1], m_baseColor[2] );
	}

	{
		glUniform1f( glGetUniformLocation( m_effect->GetShaderId(), "transparency" ), 1.0f - m_transparency[0] );
	}
	//if ( m_normalTexture.m_textureID != 0 )
	//{
	//	glActiveTexture( GL_TEXTURE1 );
	//	auto errorCode = glGetError();
	//	auto shaderId = m_effect->GetShaderId();
	//	auto location = glGetUniformLocation( shaderId, "normalTexture" );
	//	glUniform1i( glGetUniformLocation( shaderId, "normalTexture" ), 1 );
	//	errorCode = glGetError();
	//	glBindTexture( GL_TEXTURE_2D, m_normalTexture.m_textureID );
	//}
	//if ( m_specularColorTexture.m_textureID != 0 )
	//{
	//	glActiveTexture( GL_TEXTURE2 );
	//	glUniform1i( glGetUniformLocation( m_effect->GetShaderId(), "specularColorTexture" ), 2 );
	//	glBindTexture( GL_TEXTURE_2D, m_specularColorTexture.m_textureID );
	//}
	//if ( m_ambientColorTexture.m_textureID != 0 )
	//{
	//	glActiveTexture( GL_TEXTURE3 );
	//	glUniform1i( glGetUniformLocation( m_effect->GetShaderId(), "ambientColorTexture" ), 3 );
	//	glBindTexture( GL_TEXTURE_2D, m_ambientColorTexture.m_textureID );
	//}
	//if ( m_normalTexture.m_textureID != 0 )
	//{
	//	glActiveTexture( GL_TEXTURE4 );
	//	glUniform1i( glGetUniformLocation( m_effect->GetShaderId(), "normalTexture" ), 4 );
	//	glBindTexture( GL_TEXTURE_2D, m_normalTexture.m_textureID );
	//}
#endif
}

namespace
{
	eae6320::cResult LoadTexture( const unsigned int i_textureID, const std::string& i_path, unsigned int i_wrap_s, unsigned int i_wrap_t )
	{
		std::string errorMessage;

		unsigned char* data = nullptr;
		int width = 0, height = 0, components = 0;

		auto result = eae6320::Platform::LoadTextureFile( i_path.c_str(), data, width, height, components, 0, &errorMessage );

		if ( !result )
		{
			eae6320::Logging::OutputError( "Failed to load texture from file %s: %s", i_path.c_str(), errorMessage.c_str() );
		}
		else
		{
			GLenum format = GL_RED;
			if ( components == 3 )
				format = GL_RGB;
			else if ( components == 4 )
				format = GL_RGBA;

			glBindTexture( GL_TEXTURE_2D, i_textureID );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
			glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data );
			glGenerateMipmap( GL_TEXTURE_2D );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, i_wrap_s );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, i_wrap_t );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			free( data );
		}

		return result;
	}
}