// Includes
//=========

#include "../cMaterial.h"
#include "../cEffect.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

#include "../OpenGL/Includes.h"

// Helper Class Declaration
//=========================

namespace
{
	eae6320::cResult LoadTexture( const unsigned int i_textureID, const std::string& i_path, unsigned int i_wrap_s = GL_REPEAT, unsigned int i_wrap_t = GL_REPEAT );
}

// Implementation
//===============

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cMaterial::Initialize( const eae6320::Graphics::sMaterialInfo& i_materialInfo )
{
	auto result = eae6320::Results::Success;
	// TODO
	// Load effect according to the shader model type
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

	// Load Base Color Texture
	{
		if ( i_materialInfo.m_baseColorTexturePath != "" )
		{
			glGenTextures( 1, &m_baseColorTexture.m_textureID );

			if ( m_baseColorTexture.m_textureID == 0 )
			{
				EAE6320_ASSERTF( false, "Generate texture failed!" );
				return eae6320::Results::Failure;
			}

			if ( !( result = LoadTexture( m_baseColorTexture.m_textureID, meshPathDictionary + i_materialInfo.m_baseColorTexturePath ) ) )
			{
				return Results::Failure;
			}
		}
	}

	// Load Specular Color Texture
	{
		if ( i_materialInfo.m_specularColorTexturePath != "" )
		{
			glGenTextures( 1, &m_specularColorTexture.m_textureID );

			if ( m_specularColorTexture.m_textureID == 0 )
			{
				EAE6320_ASSERTF( false, "Generate texture failed!" );
				return eae6320::Results::Failure;
			}

			if ( !( result = LoadTexture( m_specularColorTexture.m_textureID, meshPathDictionary + i_materialInfo.m_specularColorTexturePath ) ) )
			{
				return Results::Failure;
			}
		}
	}

	// Load ambient
	{
		if ( i_materialInfo.m_ambientColorTexturePath != "" )
		{
			glGenTextures( 1, &m_ambientColorTexture.m_textureID );

			if ( m_ambientColorTexture.m_textureID == 0 )
			{
				EAE6320_ASSERTF( false, "Generate texture failed!" );
				return eae6320::Results::Failure;
			}

			if ( !( result = LoadTexture( m_ambientColorTexture.m_textureID, meshPathDictionary + i_materialInfo.m_ambientColorTexturePath ) ) )
			{
				return Results::Failure;
			}
		}
	}

	// Load normal texture 
	{
		if ( i_materialInfo.m_normalTexturePath != "" )
		{
			glGenTextures( 1, &m_normalTexture.m_textureID );

			if ( m_normalTexture.m_textureID == 0 )
			{
				EAE6320_ASSERTF( false, "Generate texture failed!" );
				return eae6320::Results::Failure;
			}

			if ( !( result = LoadTexture( m_normalTexture.m_textureID, meshPathDictionary + i_materialInfo.m_normalTexturePath ) ) )
			{
				return Results::Failure;
			}
		}
	}

	// Load transparency texture
	{
		if ( i_materialInfo.m_transparencyTexturePath != "" )
		{
			glGenTextures( 1, &m_transparencyTexture.m_textureID );

			if ( m_transparencyTexture.m_textureID == 0 )
			{
				EAE6320_ASSERTF( false, "Generate texture failed!" );
				return eae6320::Results::Failure;
			}

			if ( !( result = LoadTexture( m_transparencyTexture.m_textureID, meshPathDictionary + i_materialInfo.m_transparencyTexturePath ) ) )
			{
				return Results::Failure;
			}
		}
	}

	return result;
}

void eae6320::Graphics::cMaterial::Bind()
{
	m_effect->Bind();

	if ( m_baseColorTexture.m_textureID != 0 )
	{
		glActiveTexture( GL_TEXTURE0 );
		glUniform1i( glGetUniformLocation( m_effect->GetShaderId(), "baseColorTexture" ), 0 );
		glBindTexture( GL_TEXTURE_2D, m_baseColorTexture.m_textureID );
	}

	if ( m_normalTexture.m_textureID != 0 )
	{
		glActiveTexture( GL_TEXTURE1 );
		glUniform1i( glGetUniformLocation( m_effect->GetShaderId(), "normalTexture" ), 1 );
		glBindTexture( GL_TEXTURE_2D, m_normalTexture.m_textureID );
	}

	if ( m_specularColorTexture.m_textureID != 0 )
	{
		glActiveTexture( GL_TEXTURE2 );
		glUniform1i( glGetUniformLocation( m_effect->GetShaderId(), "specularColorTexture" ), 2 );
		glBindTexture( GL_TEXTURE_2D, m_specularColorTexture.m_textureID );
	}

	{
		glUniform3f( glGetUniformLocation( m_effect->GetShaderId(), "baseColor" ), m_baseColor[0], m_baseColor[1], m_baseColor[2] );
	}

	{
		glUniform1f( glGetUniformLocation( m_effect->GetShaderId(), "transparency" ), 1.0f - m_transparency[0] );
	}
}

namespace
{
	eae6320::cResult LoadTexture( const unsigned int i_textureID, const std::string& i_path, unsigned int i_wrap_s, unsigned int i_wrap_t )
	{
		std::string errorMessage;

		unsigned char* initialData = nullptr;
		int width = 0, height = 0, components = 0;

		eae6320::cScopeGuard scopeGuard( [&initialData]
			{
				if ( initialData )
				{
					free( initialData );
				}
			} );

		auto result = eae6320::Platform::LoadTextureFile( i_path.c_str(), initialData, width, height, components, 0, &errorMessage );

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
			glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, initialData );
			glGenerateMipmap( GL_TEXTURE_2D );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, i_wrap_s );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, i_wrap_t );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}

		return result;
	}
}
