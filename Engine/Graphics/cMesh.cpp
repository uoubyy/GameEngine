// Includes
//=========

#include "cMesh.h"
#include "cMaterial.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cMutex.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include "VertexFormats.h"

// Helper Class Declaration
//=========================

namespace
{
	eae6320::cResult LoadMesh( const char* const i_path, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData, void*& o_indices, uint32_t& o_triangleCount, uint32_t& o_vertexCount, uint16_t& o_materialsCount, eae6320::Graphics::cMaterial**& o_materials );
}

// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cMesh::Load( const std::string& i_meshPath, cMesh*& o_mesh )
{
	auto result = Results::Success;

	cMesh* newMesh = nullptr;

	VertexFormats::sVertex_mesh* vertexData = nullptr;
	void* indices = nullptr;

	cScopeGuard scopeGuard( [&o_mesh, &result, &newMesh, &vertexData, &indices]
		{
			if (result)
			{
				EAE6320_ASSERT( newMesh != nullptr );
				o_mesh = newMesh;
			}
			else
			{
				if ( newMesh )
				{
					newMesh->DecrementReferenceCount();
					newMesh = nullptr;
				}
				o_mesh = newMesh;
			}

			if ( vertexData )
			{
				delete[] vertexData;
				vertexData = nullptr;
			}

			if ( indices )
			{
				delete[] indices;
				indices = nullptr;
			}
		} );

	// Allocate a new mesh
	{
		newMesh = new (std::nothrow) cMesh();
		if ( !newMesh )
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh" );
			Logging::OutputError( "Failed to allocate memory for the mesh" );
			return result;
		}
	}

	// Load Mesh Data

	uint32_t triangleCount = 0;
	uint32_t vertexCount = 0;

	uint16_t materialsCount = 0;
	eae6320::Graphics::cMaterial** materials = nullptr;

	LoadMesh( i_meshPath.c_str(), vertexData, indices, triangleCount, vertexCount, materialsCount, materials );

	// Initialize the platform-specific graphics API mesh object
	if ( !( result = newMesh->Initialize( vertexData, indices, triangleCount, vertexCount, materialsCount, materials ) ) )
	{
		EAE6320_ASSERTF( false, "Initialization of new mesh failed" );
		return result;
	}

	return result;
}

// Implementation
//===============

// Initialize / Clean Up
//----------------------

eae6320::Graphics::cMesh::cMesh()
{
	m_triangleCount = 0;
}

eae6320::Graphics::cMesh::~cMesh()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}

// Helper Definitions
//===================

namespace
{
	eae6320::cResult LoadMesh( const char* const i_path, eae6320::Graphics::VertexFormats::sVertex_mesh*& o_vertexData, void*& o_indices, uint32_t& o_triangleCount, uint32_t& o_vertexCount, uint16_t& o_materialsCount, eae6320::Graphics::cMaterial**& o_materials )
	{
		auto result = eae6320::Results::Success;

		eae6320::Platform::sDataFromFile dataFromFile;
		std::string errorMessage;

		if ( !( result = eae6320::Platform::LoadBinaryFile( i_path, dataFromFile, &errorMessage ) ) )
		{
			EAE6320_ASSERTF( false, errorMessage.c_str() );
			eae6320::Logging::OutputError( "Failed to load mesh from file %s: %s", i_path, errorMessage.c_str() );
			return result;
		}

		auto currentOffset = reinterpret_cast<uintptr_t>( dataFromFile.data );
		memcpy( &o_vertexCount, reinterpret_cast<void*>( currentOffset ), sizeof( o_vertexCount ) );

		currentOffset += sizeof( o_vertexCount );
		o_vertexData = new (std::nothrow) eae6320::Graphics::VertexFormats::sVertex_mesh[o_vertexCount];

		if ( !o_vertexData )
		{
			result = eae6320::Results::OutOfMemory;
			eae6320::Logging::OutputError( "Couldn't allocate memory for the vertices data." );
			return result;
		}

		memcpy( o_vertexData, reinterpret_cast<void*>( currentOffset ), sizeof( eae6320::Graphics::VertexFormats::sVertex_mesh ) * o_vertexCount );

		currentOffset += sizeof( eae6320::Graphics::VertexFormats::sVertex_mesh ) * o_vertexCount;
		uint32_t indexCount = 0;
		memcpy( &indexCount, reinterpret_cast<void*>( currentOffset ), sizeof( uint32_t) );
		o_triangleCount = indexCount / 3;

		currentOffset += sizeof( uint32_t );
		bool is32 = indexCount > std::numeric_limits<uint16_t>::max();
		size_t dataSize = is32 ? sizeof( uint32_t ) : sizeof( uint16_t );

		if( is32 )
			o_indices = (uint32_t*) malloc( dataSize * indexCount );
		else
			o_indices = (uint16_t*) malloc( dataSize * indexCount );

		if ( !o_indices )
		{
			result = eae6320::Results::OutOfMemory;
			eae6320::Logging::OutputError( "Couldn't allocate memory for the indices data." );
			return result;
		}

		memcpy( o_indices, reinterpret_cast<void*>( currentOffset ), dataSize * indexCount );

		currentOffset += dataSize * indexCount;
		auto dataEndPoint = reinterpret_cast<uintptr_t>( dataFromFile.data );
		dataEndPoint += dataFromFile.size;

		// Extra materials
		if ( currentOffset != dataEndPoint )
		{
			memcpy( &o_materialsCount, reinterpret_cast<void*>( currentOffset ), sizeof( uint16_t ) );
			currentOffset += sizeof( uint16_t );

			if( o_materialsCount > 0 )
			{
				o_materials = new (std::nothrow) eae6320::Graphics::cMaterial*[o_materialsCount];
				if ( !o_materials )
				{
					result = eae6320::Results::OutOfMemory;
					eae6320::Logging::OutputError( "Couldn't allocate memory for the materials data." );
					return result;
				}

				for ( auto i = 0; i < o_materialsCount; ++i )
				{
					eae6320::Graphics::cMaterial* newMaterial;
					uint32_t materialDataSize = 0;
					if ( !( result = eae6320::Graphics::cMaterial::Load( reinterpret_cast<void*>( currentOffset ), materialDataSize, newMaterial ) ) )
					{
						return result;
					}

					currentOffset += materialDataSize;
					o_materials[i] = newMaterial;
				}
			}
		}

		return result;
	}
}