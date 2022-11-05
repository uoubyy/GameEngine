// Includes
//=========

#include "../cMesh.h"
#include "../cMaterial.h"

#include "../VertexFormats.h"

#include <cstdlib>
#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <new>

// Implementation
//===============

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cMesh::Initialize( const VertexFormats::sVertex_mesh* i_vertexData, const void* i_indices, const uint32_t i_triangleCount, const uint32_t i_vertexCount, const uint16_t i_materialsCount, cMaterial** i_materials )
{
	auto result = Results::Success;

	// Create a vertex array object and make it active
	{
		constexpr GLsizei arrayCount = 1;
		glGenVertexArrays( arrayCount, &m_vertexArrayId );
		const auto errorCode = glGetError();
		if ( errorCode == GL_NO_ERROR )
		{
			glBindVertexArray( m_vertexArrayId );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to bind a new vertex array: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to get an unused vertex array ID: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			return result;
		}
	}
	// Create a vertex buffer object and make it active
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers( bufferCount, &m_vertexBufferId );
		const auto errorCode = glGetError();
		if ( errorCode == GL_NO_ERROR )
		{
			glBindBuffer( GL_ARRAY_BUFFER, m_vertexBufferId );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to bind a new vertex buffer: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			return result;
		}
	}
	// Create an element buffer object and make it active
	{
		constexpr GLsizei bufferCount = 1;
		glGenBuffers( bufferCount, &m_elementBufferId );
		const auto errorCode = glGetError();
		if ( errorCode == GL_NO_ERROR )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_elementBufferId );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to bind a new index buffer: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				return result;
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to get an unused vertex buffer ID: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			return result;
		}
	}
	// Assign the data to the buffer
	{
		const int bufferSize = sizeof( i_vertexData[0] ) * i_vertexCount;
		EAE6320_ASSERT( bufferSize <= std::numeric_limits<GLsizeiptr>::max() );

		glBufferData( GL_ARRAY_BUFFER, static_cast<GLsizeiptr>( bufferSize ), reinterpret_cast<GLvoid*>( const_cast<VertexFormats::sVertex_mesh*>( i_vertexData ) ),
			// In our class we won't ever read from the buffer
			GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to allocate the vertex buffer: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			return result;
		}
	}
	// Initialize vertex format
	{
		// The "stride" defines how large a single vertex is in the stream of data
		// (or, said another way, how far apart each position element is)
		constexpr auto stride = static_cast<GLsizei>( sizeof(eae6320::Graphics::VertexFormats::sVertex_mesh ) );

		// Position (0)
		// 3 floats == 12 bytes
		// Offset = 0
		{
			constexpr GLuint vertexElementLocation = 0;
			constexpr GLint elementCount = 3;
			constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer( vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride,
				reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormats::sVertex_mesh, x ) ) );
			const auto errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glEnableVertexAttribArray( vertexElementLocation );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError("OpenGL failed to enable the POSITION vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to set the POSITION vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				return result;
			}
		}

		// Normal (1)
		{
			constexpr GLuint vertexElementLocation = 1;
			constexpr GLint elementCount = 3;

			glVertexAttribPointer( vertexElementLocation, elementCount, GL_FLOAT, GL_FALSE, stride,
				reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormats::sVertex_mesh, nx ) ) );
			const auto errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glEnableVertexAttribArray( vertexElementLocation );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to enable the NORMAL vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to set the NORMAL vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				return result;
			}
		}

		// Tangent (2)
		{
			constexpr GLuint vertexElementLocation = 2;
			constexpr GLint elementCount = 3;

			glVertexAttribPointer( vertexElementLocation, elementCount, GL_FLOAT, GL_FALSE, stride,
				reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormats::sVertex_mesh, tx ) ) );
			const auto errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glEnableVertexAttribArray( vertexElementLocation );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to enable the TANGENT vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to set the TANGENT vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				return result;
			}
		}

		// BiTangent (3)
		{
			constexpr GLuint vertexElementLocation = 3;
			constexpr GLint elementCount = 3;

			glVertexAttribPointer( vertexElementLocation, elementCount, GL_FLOAT, GL_FALSE, stride,
				reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormats::sVertex_mesh, btx ) ) );
			const auto errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glEnableVertexAttribArray( vertexElementLocation );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to enable the BITANGENT vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to set the BITANGENT vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				return result;
			}
		}

		// UV (4)
		{
			constexpr GLuint vertexElementLocation = 4;
			constexpr GLint elementCount = 2;

			glVertexAttribPointer( vertexElementLocation, elementCount, GL_FLOAT, GL_FALSE, stride,
				reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormats::sVertex_mesh, u ) ) );
			const auto errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glEnableVertexAttribArray( vertexElementLocation );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to enable the UV vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to set the UV vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				return result;
			}
		}

		// Color (5)
		{
			constexpr GLuint vertexElementLocation = 5;
			constexpr GLint elementCount = 4;
			//constexpr GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
			glVertexAttribPointer( vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, GL_TRUE, stride,
				reinterpret_cast<GLvoid*>( offsetof( eae6320::Graphics::VertexFormats::sVertex_mesh, r ) ) );
			const auto errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glEnableVertexAttribArray( vertexElementLocation );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					eae6320::Logging::OutputError( "OpenGL failed to enable the COLOR vertex attribute at location %u: %s",
						vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
					return result;
				}
			}
			else
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				eae6320::Logging::OutputError( "OpenGL failed to set the COLOR vertex attribute at location %u: %s",
					vertexElementLocation, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				return result;
			}
		}
	}
	// Assign the data to the element buffer
	{
		// const uint16_t* i_indices
		// re-arrange indices order
		constexpr unsigned int vertexCountPerTriangle = 3;
		const auto indiceCnt = i_triangleCount * vertexCountPerTriangle;
		void* indices;

		bool is32 = false;
		
		if ( indiceCnt > std::numeric_limits<uint16_t>::max())
		{
			indices = new (std::nothrow) uint32_t[indiceCnt];
			is32 = true;
		}
		else
		{
			indices = new (std::nothrow) uint16_t[indiceCnt];
		}

		auto dataSize = is32 ? sizeof(uint32_t) : sizeof(uint16_t);

		if( indices )
		{ 
			auto dst = reinterpret_cast<uintptr_t>( indices );
			auto src = reinterpret_cast<uintptr_t>( i_indices );

			for ( size_t i = 0; i < indiceCnt; i += vertexCountPerTriangle)
			{
				memcpy(reinterpret_cast<void*>(dst + i * dataSize), reinterpret_cast<void*>(src + i * dataSize), dataSize);
				memcpy(reinterpret_cast<void*>(dst + ( i + 2 ) * dataSize), reinterpret_cast<void*>(src + ( i + 1 ) * dataSize), dataSize);
				memcpy(reinterpret_cast<void*>(dst + ( i + 1 ) * dataSize), reinterpret_cast<void*>(src + ( i + 2 ) * dataSize), dataSize);	
			}
		}

		// Safely clear dynamic memory
		cScopeGuard scopeGuard( [indices]
		{
			delete[] indices;
		});

		const int bufferSize = ( is32 ? sizeof( uint32_t ) : sizeof( uint16_t ) ) * i_triangleCount * vertexCountPerTriangle;
		EAE6320_ASSERT( bufferSize <= std::numeric_limits<GLsizeiptr>::max() );

		glBufferData( GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>( bufferSize ), reinterpret_cast<GLvoid*>( indices ),
			GL_STATIC_DRAW);
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			eae6320::Logging::OutputError( "OpenGL failed to allocate the element buffer: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			return result;
		}
	}

	m_triangleCount = i_triangleCount;

	m_materialsCount = i_materialsCount;
	m_materials = i_materials;

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::CleanUp()
{
	auto result = Results::Success;

	if ( m_vertexArrayId != 0 )
	{
		// Make sure that the vertex array isn't bound
		{
			// Unbind the vertex array
			glBindVertexArray( 0 );
			const auto errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				if ( result )
				{
					result = Results::Failure;
				}
				EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
				Logging::OutputError( "OpenGL failed to unbind all vertex arrays before cleaning up geometry: %s",
					reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			}
		}
		constexpr GLsizei arrayCount = 1;
		glDeleteVertexArrays( arrayCount, &m_vertexArrayId );
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			if ( result )
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the vertex array: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		m_vertexArrayId = 0;
	}

	if ( m_vertexBufferId != 0 )
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers( bufferCount, &m_vertexBufferId );
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			if ( result )
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the vertex buffer: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		m_vertexBufferId = 0;
	}

	if ( m_elementBufferId != 0 )
	{
		constexpr GLsizei bufferCount = 1;
		glDeleteBuffers( bufferCount, &m_elementBufferId );
		const auto errorCode = glGetError();
		if ( errorCode != GL_NO_ERROR )
		{
			if (result)
			{
				result = Results::Failure;
			}
			EAE6320_ASSERTF( false, reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
			Logging::OutputError( "OpenGL failed to delete the index buffer: %s",
				reinterpret_cast<const char*>( gluErrorString( errorCode ) ) );
		}
		m_elementBufferId = 0;
	}

	{
		for ( auto i = 0; i < m_materialsCount; ++i )
		{
			if ( m_materials[i] )
			{
				m_materials[i]->DecrementReferenceCount();
				m_materials[i] = nullptr;
			}
		}

		delete[] m_materials;
	}

	return result;
}

void eae6320::Graphics::cMesh::Draw()
{
	// Draw the geometry
	{
		// Bind a specific vertex buffer to the device as a data source
		{
			EAE6320_ASSERT( m_vertexArrayId != 0 );
			glBindVertexArray( m_vertexArrayId );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
		// Render triangles from the currently-bound vertex buffer
		{
			// The mode defines how to interpret multiple vertices as a single "primitive";
			// a triangle list is defined
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			constexpr GLenum mode = GL_TRIANGLES;

			// It's possible to start rendering primitives in the middle of the stream
			const GLvoid* const offset = 0;
			constexpr unsigned int vertexCountPerTriangle = 3;

			const auto indiceCnt = m_triangleCount * vertexCountPerTriangle;
			bool is32 = indiceCnt > std::numeric_limits<uint16_t>::max() ? true : false;

			if ( m_materialsCount > 0 )
			{
				for ( auto i = 0; i < m_materialsCount; ++i )
				{
					m_materials[i]->Bind();
					uint32_t elementsCount = m_materials[i]->m_indexRange.last - m_materials[i]->m_indexRange.first + 1;
					size_t elementSize = is32 ? sizeof(uint32_t) : sizeof(uint16_t);
					glDrawElements( mode, elementsCount, is32 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>( m_materials[i]->m_indexRange.first * elementSize ) );
				}
			}
			else
			{ 
				if ( indiceCnt > std::numeric_limits<uint16_t>::max() )
					glDrawElements( mode, m_triangleCount * vertexCountPerTriangle, GL_UNSIGNED_INT, offset );
				else
					glDrawElements( mode, m_triangleCount * vertexCountPerTriangle, GL_UNSIGNED_SHORT, offset );
			}

			const auto errorCode = glGetError();
			EAE6320_ASSERT( errorCode == GL_NO_ERROR );
		}
	}
}