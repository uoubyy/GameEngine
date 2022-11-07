// Includes
//=========

#include "cMeshBuilder.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Platform/Platform.h>

#include <External/Lua/Includes.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

#include <fstream>

// Helper Class Declaration
//=========================

namespace
{
	struct sVertex_mesh
	{
		// Position
		float x, y, z;
		// Normal
		float nx, ny, nz;
		// Tangent
		float tx, ty, tz;
		// Bitangent
		float btx, bty, btz;
		// Texture coordinates
		float u, v;
		// Color
		uint8_t r, g, b, a;

		uint8_t mat;
	};

	struct sMaterialInfo
	{
		float baseColor[3] = { 0 };
		std::string baseColorTexName;

		float specularColor[3] = { 0 };
		std::string	specularColorTexName;

		float ambient[3] = { 0 };
		std::string	ambientTexName;

		float transparency[3] = { 0 };
		std::string	transparencyTexName;

		std::string	normalTexName;

		struct
		{
			size_t first = std::numeric_limits<size_t>::max(), last = 0;
		} vertexRange;
		struct
		{
			size_t first = std::numeric_limits<size_t>::max(), last = 0;
		} indexRange;
	};

	eae6320::cResult LoadMesh( const char* const i_sourcePath, const char* const i_targetPath, sVertex_mesh*& o_vertexData, void*& o_indices, uint32_t& o_triangleCount, uint32_t& o_vertexCount, sMaterialInfo*& o_materials, uint16_t& o_materialsCount );

	eae6320::cResult LoadElements( lua_State& io_luaState, void*& o_indices, uint32_t& o_triangleCount );
	eae6320::cResult LoadElementsValues( lua_State& io_luaState, void*& o_indices, uint32_t& o_triangleCount );
	eae6320::cResult LoadElement( lua_State& io_luaState, void* o_indices, uint32_t i_index, bool i_32 );

	eae6320::cResult LoadVertices( lua_State& io_luaState, sVertex_mesh*& o_vertexData, uint32_t& o_vertexCount );
	eae6320::cResult LoadVerticesValues( lua_State& io_luaState, sVertex_mesh*& o_vertexData, uint32_t& o_vertexCount );
	eae6320::cResult LoadVertex( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );
	eae6320::cResult LoadVertexPosition( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );
	eae6320::cResult LoadVertexColor( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );

	eae6320::cResult LoadVertexNormal( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );
	eae6320::cResult LoadVertexTangent( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );
	eae6320::cResult LoadVertexBiTangent( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );
	eae6320::cResult LoadVertexUV( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );

	eae6320::cResult LoadMaterials( lua_State& io_luaState, const std::string& i_sourcePath, const std::string& i_meshName, const std::string& i_targetPath, sMaterialInfo*& o_materials, uint16_t& o_materialsCount );
	eae6320::cResult LoadMaterial( lua_State& io_luaState, const std::string& i_sourcePath, const std::string& i_meshName, const std::string& i_targetPath, sMaterialInfo* o_materials, uint16_t i_index );

	void GetFilePathandFileName( const std::string& i_path, std::string& o_path, std::string& o_filename );
}

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMeshBuilder::Build( const std::vector<std::string>& i_arguments )
{
	auto result = eae6320::Results::Success;

	std::string errorMessage;
	if ( !( result = eae6320::Platform::CreateDirectoryIfItDoesntExist( m_path_target, &errorMessage ) ) )
	{
		eae6320::Assets::OutputErrorMessageWithFileInfo( m_path_target, errorMessage.c_str() );
		return result;
	}

	constexpr auto noErrorIfTargetAlreadyExists = false;
	constexpr auto updateTheTargetFileTime = true;

	sVertex_mesh* vertexData = nullptr;
	void* indices = nullptr;
	sMaterialInfo* materials = nullptr;

	uint32_t indiceCount = 0;
	uint32_t triangleCount = 0;
	uint32_t vertexCount = 0;
	uint16_t materialsCount = 0;

	eae6320::cScopeGuard scopeGuard_onExit( [ &vertexData, &indices, &materials ]
		{
			if ( vertexData )
			{
				delete[] vertexData;
			}
			if ( indices )
			{
				delete[] indices;
			}
			if ( materials )
			{
				delete[] materials;
			}
		});

	if ( !( result = LoadMesh( m_path_source, m_path_target, vertexData, indices, triangleCount, vertexCount, materials, materialsCount ) ) )
	{
		eae6320::Assets::OutputErrorMessageWithFileInfo( m_path_target, "Failed to extra Lua mesh file." );
			return result;
	}

	indiceCount = triangleCount * 3;

	std::ofstream binaryMeshFile( m_path_target, std::ofstream::binary );
	if ( binaryMeshFile.is_open() )
	{
		size_t size1 = sizeof( vertexCount );
		size_t size2 = sizeof( sVertex_mesh ) * vertexCount;
		size_t size3 = sizeof( indiceCount );

		size_t dataSize = indiceCount > std::numeric_limits<uint16_t>::max() ? sizeof( uint32_t ) : sizeof( uint16_t );
		size_t size4 = dataSize * indiceCount;
		auto bufferSize = size1 + size2 + size3 + size4;

		// calculate materials info size
		{
			bufferSize += sizeof( materialsCount );

			for ( size_t index = 0; index < materialsCount; ++index )
			{
				auto material = materials[index];
				// every texture name's length is under 255 char
				bufferSize += 5 * sizeof( uint8_t ); // 5 texture

				bufferSize += material.baseColorTexName.length();
				bufferSize += material.specularColorTexName.length();
				bufferSize += material.ambientTexName.length();
				bufferSize += material.normalTexName.length();
				bufferSize += material.transparencyTexName.length();

				bufferSize += 4 * sizeof( uint32_t ); // vertex and index range

				bufferSize += 4 * 3 * sizeof(float); // 4 color
			}
		}

		char* buffer = new (std::nothrow) char[bufferSize];

		if ( !buffer )
		{
			result = eae6320::Results::OutOfMemory;
			eae6320::Assets::OutputErrorMessage( "Couldn't allocate memory for the binary mesh data." );
			return result;
		}

		if( bufferSize >= size1 )
			memcpy( buffer, &vertexCount, size1 );
		if ( bufferSize >= size1 + size2 )
			memcpy( buffer + size1, vertexData, size2 );
		if ( bufferSize >= size1 + size2 + size3 )
			memcpy( buffer + size1 + size2, &indiceCount, size3 );
		if ( bufferSize >= size1 + size2 + size3 + size4 )
			memcpy( buffer + size1 + size2 + size3, indices, size4 );

		// write materials info
		{
			auto currentOffset = reinterpret_cast<uintptr_t>( buffer + size1 + size2 + size3 + size4 );
			memcpy( reinterpret_cast<void*>( currentOffset ), &materialsCount, sizeof( materialsCount ) );
			currentOffset += sizeof( materialsCount );

			for ( size_t index = 0; index < materialsCount; ++index )
			{
				auto material = materials[index];
				{
					uint8_t len = 0;
					// every texture name's length is under 255 char
					// Base color
					memcpy( reinterpret_cast<void*>( currentOffset ), reinterpret_cast<void*>( material.baseColor ), 3 * sizeof( float ) );
					currentOffset += 3 * sizeof( float );
					len = static_cast<uint8_t>( material.baseColorTexName.length() );
					memcpy( reinterpret_cast<void*>( currentOffset ), &len, sizeof( uint8_t ) );
					currentOffset += sizeof( uint8_t );
					memcpy( reinterpret_cast<void*>( currentOffset ), material.baseColorTexName.c_str(), len );
					currentOffset += len;

					// Specular Color
					memcpy( reinterpret_cast<void*>( currentOffset ), reinterpret_cast<void*>( material.specularColor ), 3 * sizeof( float ) );
					currentOffset += 3 * sizeof( float );
					len = static_cast<uint8_t>( material.specularColorTexName.length() );
					memcpy( reinterpret_cast<void*>( currentOffset ), &len, sizeof( uint8_t ) );
					currentOffset += sizeof( uint8_t );
					memcpy( reinterpret_cast<void*>( currentOffset ), material.specularColorTexName.c_str(), len );
					currentOffset += len;

					// Ambient
					memcpy( reinterpret_cast<void*>( currentOffset ), reinterpret_cast<void*>( material.ambient ), 3 * sizeof( float ) );
					currentOffset += 3 * sizeof( float );
					len = static_cast<uint8_t>( material.ambientTexName.length() );
					memcpy( reinterpret_cast<void*>( currentOffset ), &len, sizeof( uint8_t ) );
					currentOffset += sizeof( uint8_t );
					memcpy( reinterpret_cast<void*>( currentOffset ), material.ambientTexName.c_str(), len );
					currentOffset += len;

					// Transparency
					memcpy( reinterpret_cast<void*>( currentOffset ), reinterpret_cast<void*>( material.transparency ), 3 * sizeof( float ) );
					currentOffset += 3 * sizeof( float );
					len = static_cast<uint8_t>( material.transparencyTexName.length() );
					memcpy( reinterpret_cast<void*>( currentOffset ), &len, sizeof( uint8_t ) );
					currentOffset += sizeof( uint8_t );
					memcpy( reinterpret_cast<void*>( currentOffset ), material.transparencyTexName.c_str(), len );
					currentOffset += len;

					// Normal
					len = static_cast<uint8_t>( material.normalTexName.length() );
					memcpy( reinterpret_cast<void*>( currentOffset ), &len, sizeof( uint8_t ) );
					currentOffset += sizeof( uint8_t );
					memcpy( reinterpret_cast<void*>( currentOffset ), material.normalTexName.c_str(), len );
					currentOffset += len;

					// Vertex range
					memcpy( reinterpret_cast<void*>( currentOffset ), &(material.vertexRange.first), sizeof( uint32_t ) );
					currentOffset += sizeof( uint32_t );
					memcpy( reinterpret_cast<void*>( currentOffset ), &( material.vertexRange.last ), sizeof( uint32_t ) );
					currentOffset += sizeof( uint32_t );

					// Index range
					memcpy( reinterpret_cast<void*>( currentOffset ), &( material.indexRange.first ), sizeof( uint32_t ) );
					currentOffset += sizeof( uint32_t );
					memcpy( reinterpret_cast<void*>( currentOffset ), &( material.indexRange.last ), sizeof( uint32_t ) );
					currentOffset += sizeof( uint32_t );
				}
			}
		}

		binaryMeshFile.write( buffer, bufferSize );

		delete[] buffer;

		binaryMeshFile.close();
	}

	return result;
}

// Helper Definitions
//===================

namespace
{
	eae6320::cResult LoadMesh( const char* const i_sourcePath, const char* const i_targetPath, sVertex_mesh*& o_vertexData, void*& o_indices, uint32_t& o_triangleCount, uint32_t& o_vertexCount, sMaterialInfo*& o_materials, uint16_t& o_materialsCount )
	{
		auto result = eae6320::Results::Success;

		// Create a new Lua state
		lua_State* luaState = nullptr;
		eae6320::cScopeGuard scopeGuard_onExit( [&luaState]
			{
				if ( luaState )
				{
					lua_close( luaState );
					luaState = nullptr;
				}
			});
		{
			luaState = luaL_newstate();
			if ( !luaState )
			{
				result = eae6320::Results::OutOfMemory;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_sourcePath, "Failed to create a new Lua state" );
				return result;
			}
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		const auto stackTopBeforeLoad = lua_gettop( luaState );
		{
			const auto luaResult = luaL_loadfile( luaState, i_sourcePath );
			if ( luaResult != LUA_OK )
			{
				result = eae6320::Results::Failure;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_sourcePath, lua_tostring(luaState, -1) );
				// Pop the error message
				lua_pop( luaState, 1 );
				return result;
			}
		}
		// Execute the "chunk", which should load the asset
		// into a table at the top of the stack
		{
			constexpr int argumentCount = 0;
			constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
			constexpr int noMessageHandler = 0;
			const auto luaResult = lua_pcall( luaState, argumentCount, returnValueCount, noMessageHandler );
			if ( luaResult == LUA_OK )
			{
				// A well-behaved asset file will only return a single value
				const auto returnedValueCount = lua_gettop( luaState ) - stackTopBeforeLoad;
				if ( returnedValueCount == 1 )
				{
					// A correct asset file _must_ return a table
					if ( !lua_istable( luaState, -1 ) )
					{
						result = eae6320::Results::InvalidFile;
						eae6320::Assets::OutputErrorMessageWithFileInfo( i_sourcePath, "Asset files must return a table (instead of a %s )", luaL_typename( luaState, -1 ) );
						// Pop the returned non-table value
						lua_pop( luaState, 1 );
						return result;
					}
				}
				else
				{
					result = eae6320::Results::InvalidFile;
					eae6320::Assets::OutputErrorMessageWithFileInfo( i_sourcePath, "Asset files must return a single table (instead of %s values)", luaL_typename( luaState, -1 ) );
					// Pop every value that was returned
					lua_pop( luaState, returnedValueCount );
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_sourcePath, luaL_typename( luaState, -1 ) );
				// Pop the error message
				lua_pop( luaState, 1 );
				return result;
			}
		}

		// If this code is reached the asset file was loaded successfully,
		// and its table is now at index -1
		eae6320::cScopeGuard scopeGuard_popAssetTable( [luaState]
			{
				lua_pop( luaState, 1 );
			});

		if ( !( result = LoadVertices( *luaState, o_vertexData, o_vertexCount ) ) )
		{
			return result;
		}
		if ( !( result = LoadElements( *luaState, o_indices, o_triangleCount ) ) )
		{
			return result;
		}

		{
			std::string sourceFolderPath, sourceMeshFileName;
			GetFilePathandFileName( i_sourcePath, sourceFolderPath, sourceMeshFileName );

			std::string targetFolderPath, targetMeshFileName;
			GetFilePathandFileName( i_targetPath, targetFolderPath, targetMeshFileName );

			LoadMaterials( *luaState, sourceFolderPath, sourceMeshFileName, targetFolderPath, o_materials, o_materialsCount );
		}

		return result;
	}


	eae6320::cResult LoadElements( lua_State& io_luaState, void*& o_indices, uint32_t& o_triangleCount )
	{
		auto result = eae6320::Results::Success;

		constexpr auto* const key = "elements";
		lua_pushstring( &io_luaState, key );
		lua_gettable( &io_luaState, -2 );

		eae6320::cScopeGuard scopeGuard_elements( [&io_luaState]
			{
				lua_pop ( &io_luaState, 1 );
			});

		if ( lua_istable( &io_luaState, -1 ) )
		{
			if ( !( result = LoadElementsValues( io_luaState, o_indices, o_triangleCount ) ) )
			{
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
			return result;
		}

		return result;
	}

	eae6320::cResult LoadElementsValues( lua_State& io_luaState, void*& o_indices, uint32_t& o_triangleCount )
	{
		auto result = eae6320::Results::Success;

		o_triangleCount = static_cast<uint32_t>( luaL_len( &io_luaState, -1 ) );
		constexpr uint32_t vertexCountPerTriangle = 3;
		const auto indiceCount = o_triangleCount * vertexCountPerTriangle;

		bool is32 = false;
		if( indiceCount > std::numeric_limits<uint16_t>::max())
		{ 
			o_indices = new (std::nothrow) uint32_t[indiceCount];
			is32 = true;
		}
		else
		{ 
			o_indices = new (std::nothrow) uint16_t[indiceCount];
		}

		if ( !o_indices )
		{
			result = eae6320::Results::OutOfMemory;
			eae6320::Assets::OutputErrorMessage( "Couldn't allocate memory for the indices data." );
			return result;
		}
		for ( uint32_t i = 1; i <= o_triangleCount; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			eae6320::cScopeGuard scopeGuard_element( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				});
			if ( !( result = LoadElement( io_luaState, o_indices, ( i - 1 ) * 3, is32 ) ) )
			{
				return result;
			}
		}

		return result;
	}

	eae6320::cResult LoadElement( lua_State& io_luaState, void* o_indices, uint32_t i_index, bool i_32)
	{
		auto result = eae6320::Results::Success;

		// eae6320::Logging::OutputMessage( "Iterating through element index:" );

		constexpr uint32_t vertexCountPerTriangle = 3;

		for ( uint32_t i = 1; i <= vertexCountPerTriangle; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			eae6320::cScopeGuard scopeGuard_popTexturePath( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				});

			if( i_32 )
			{ 
				auto currentOffset = reinterpret_cast<uintptr_t>( o_indices );
				currentOffset += i_index * sizeof( uint32_t );
				uint32_t* index = reinterpret_cast<uint32_t*>( currentOffset );
				*index = static_cast<uint32_t>(lua_tointeger(&io_luaState, -1));
			}
			else
			{
				auto currentOffset = reinterpret_cast<uintptr_t>( o_indices );
				currentOffset += i_index * sizeof( uint16_t );
				uint16_t* index = reinterpret_cast<uint16_t*>( currentOffset );
				*index = static_cast<uint16_t>( lua_tointeger( &io_luaState, -1 ) );
			}

			i_index++;
			// eae6320::Logging::OutputMessage( "\t%f", lua_tonumber( &io_luaState, -1 ) );
		}

		return result;
	}

	eae6320::cResult LoadVertices( lua_State& io_luaState, sVertex_mesh*& o_vertexData, uint32_t& o_vertexCount )
	{
		auto result = eae6320::Results::Success;

		constexpr auto* const key = "vertices";
		lua_pushstring( &io_luaState, key );
		lua_gettable( &io_luaState, -2 );

		eae6320::cScopeGuard scopeGuard_vertices( [&io_luaState]
			{
				lua_pop( &io_luaState, 1 );
			});

		if ( lua_istable( &io_luaState, -1 ) )
		{
			if ( !( result = LoadVerticesValues( io_luaState, o_vertexData, o_vertexCount ) ) )
			{
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
			return result;
		}

		return result;
	}

	eae6320::cResult LoadVerticesValues( lua_State& io_luaState, sVertex_mesh*& o_vertexData, uint32_t& o_vertexCount )
	{
		auto result = eae6320::Results::Success;

		// eae6320::Logging::OutputMessage( "Iterating through every vertex:" );
		o_vertexCount = static_cast<uint32_t>( luaL_len( &io_luaState, -1 ) );

		o_vertexData = new (std::nothrow) sVertex_mesh[o_vertexCount];

		if ( !o_vertexData )
		{
			result = eae6320::Results::OutOfMemory;
			eae6320::Assets::OutputErrorMessage( "Couldn't allocate memory for the indices data." );
			return result;
		}
		for ( uint32_t i = 1; i <= o_vertexCount; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			eae6320::cScopeGuard scopeGuard_vertex( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				});
			if ( !( result = LoadVertex( io_luaState, o_vertexData, i - 1 ) ) )
			{
				return result;
			}
		}

		return result;
	}

	eae6320::cResult LoadVertex( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index )
	{
		auto result = eae6320::Results::Success;
		{
			constexpr auto* const key = "position";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard_position( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				});

			if ( lua_istable( &io_luaState, -1 ) )
			{
				if ( !( result = LoadVertexPosition( io_luaState, o_vertexData, i_index ) ) )
				{
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "color";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard_color( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				});

			if ( lua_istable( &io_luaState, -1 ) )
			{
				if ( !( result = LoadVertexColor( io_luaState, o_vertexData, i_index ) ) )
				{
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "normal";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard_normal( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				if ( !( result = LoadVertexNormal( io_luaState, o_vertexData, i_index ) ) )
				{
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "tangent";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard_tangent( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				if ( !( result = LoadVertexTangent( io_luaState, o_vertexData, i_index ) ) )
				{
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "bitangent";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard_bitangent( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				if ( !( result = LoadVertexBiTangent( io_luaState, o_vertexData, i_index ) ) )
				{
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "uv";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard_uv( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				if ( !( result = LoadVertexUV( io_luaState, o_vertexData, i_index ) ) )
				{
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "mat";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard_mat( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_isinteger( &io_luaState, -1 ) )
			{
				o_vertexData[i_index].mat = static_cast<uint8_t>( lua_tointeger( &io_luaState, -1  ) );
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a integer (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		return result;
	}

	eae6320::cResult LoadVertexPosition( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index )
	{
		auto result = eae6320::Results::Success;

		// eae6320::Logging::OutputMessage( "Iterating through vertex position:" );

		for ( uint32_t i = 1; i <= 3; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			eae6320::cScopeGuard scopeGuard_position( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				});

			if ( i == 1 )
				o_vertexData[i_index].x = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
			else if (i == 2)
				o_vertexData[i_index].y = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
			else
				o_vertexData[i_index].z = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );

			// eae6320::Logging::OutputMessage( "\t%f", lua_tonumber( &io_luaState, -1 ) );
		}

		return result;
	}

	eae6320::cResult LoadVertexColor( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index )
	{
		auto result = eae6320::Results::Success;

		// eae6320::Logging::OutputMessage( "Iterating through vertex color:" );

		for ( uint32_t i = 1; i <= 4; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			eae6320::cScopeGuard scopeGuard_position( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				});

			if ( i == 1 )
				o_vertexData[i_index].r = static_cast<uint8_t>( lua_tointeger( &io_luaState, -1 ) ) * 255;
			else if ( i == 2 )
				o_vertexData[i_index].g = static_cast<uint8_t>( lua_tointeger( &io_luaState, -1 ) ) * 255;
			else if ( i == 3 )
				o_vertexData[i_index].b = static_cast<uint8_t>( lua_tointeger( &io_luaState, -1 ) ) * 255;
			else
				o_vertexData[i_index].a = static_cast<uint8_t>( lua_tointeger( &io_luaState, -1 ) ) * 255;

			// eae6320::Logging::OutputMessage( "\t%d", lua_tointeger( &io_luaState, -1 ) );
		}

		return result;
	}

	eae6320::cResult LoadVertexNormal( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index )
	{
		auto result = eae6320::Results::Success;

		for ( uint32_t i = 1; i <= 3; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			eae6320::cScopeGuard scopeGuard_normal( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( i == 1 )
				o_vertexData[i_index].nx = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
			else if ( i == 2 )
				o_vertexData[i_index].ny = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
			else
				o_vertexData[i_index].nz = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
		}

		return result;
	}

	eae6320::cResult LoadVertexTangent( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index )
	{
		auto result = eae6320::Results::Success;

		for ( uint32_t i = 1; i <= 3; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			eae6320::cScopeGuard scopeGuard_normal( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( i == 1 )
				o_vertexData[i_index].tx = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
			else if ( i == 2 )
				o_vertexData[i_index].ty = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
			else
				o_vertexData[i_index].tz = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
		}

		return result;
	}

	eae6320::cResult LoadVertexBiTangent( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index )
	{
		auto result = eae6320::Results::Success;

		for ( uint32_t i = 1; i <= 3; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			eae6320::cScopeGuard scopeGuard_normal( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( i == 1 )
				o_vertexData[i_index].btx = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
			else if ( i == 2 )
				o_vertexData[i_index].bty = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
			else
				o_vertexData[i_index].btz = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
		}

		return result;
	}

	eae6320::cResult LoadVertexUV( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index )
	{
		auto result = eae6320::Results::Success;

		for ( uint32_t i = 1; i <= 2; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			eae6320::cScopeGuard scopeGuard_normal( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( i == 1 )
				o_vertexData[i_index].u = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
			else
				o_vertexData[i_index].v = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
		}

		return result;
	}

	eae6320::cResult LoadMaterials( lua_State& io_luaState, const std::string& i_sourcePath, const std::string& i_meshName, const std::string& i_targetPath, sMaterialInfo*& o_materials, uint16_t& o_materialsCount )
	{
		auto result = eae6320::Results::Success;

		constexpr auto* const key = "materials";
		lua_pushstring( &io_luaState, key );
		lua_gettable( &io_luaState, -2 );

		eae6320::cScopeGuard scopeGuard_materials( [&io_luaState]
			{
				lua_pop( &io_luaState, 1 );
			} );

		if ( lua_istable( &io_luaState, -1 ) )
		{
			o_materialsCount = static_cast<uint32_t>( luaL_len( &io_luaState, -1 ) );

			o_materials = new (std::nothrow) sMaterialInfo[o_materialsCount];

			if ( !o_materials )
			{
				result = eae6320::Results::OutOfMemory;
				eae6320::Assets::OutputErrorMessage( "Couldn't allocate memory for the materials data." );
				return result;
			}

			for ( uint32_t i = 1; i <= o_materialsCount; ++i )
			{
				lua_pushinteger( &io_luaState, i );
				lua_gettable( &io_luaState, -2 );
				eae6320::cScopeGuard scopeGuard_material( [&io_luaState]
					{
						lua_pop( &io_luaState, 1 );
					} );
				if ( !( result = LoadMaterial( io_luaState, i_sourcePath, i_meshName, i_targetPath, o_materials, i - 1 ) ) )
				{
					return result;
				}
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			eae6320::Assets::OutputWarningMessage( "Mesh file %s does not contains materials.", i_meshName.c_str() );
			return result;
		}

		return result;
	}

	eae6320::cResult LoadMaterial( lua_State& io_luaState, const std::string& i_path, const std::string& i_meshName, const std::string& i_targetPath, sMaterialInfo* o_materials, uint16_t i_index )
	{
		auto result = eae6320::Results::Success;

		auto splitIndex = i_meshName.find_first_of( "." );
		std::string meshFileName = i_meshName.substr( 0, splitIndex );
		std::string meshFileExtension = i_meshName.substr( splitIndex + 1 );

		{
			constexpr auto* const key = "nodeName";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_isstring( &io_luaState, -1 ) )
			{
				std::string nodeName = lua_tostring( &io_luaState, -1 );
				// eae6320::Assets::OutputWarningMessage( "Load material with node name %s.", nodeName.c_str() );
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a string (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "baseColor";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				for ( uint32_t i = 1; i <= 3; ++i )
				{
					lua_pushinteger( &io_luaState, i );
					lua_gettable( &io_luaState, -2 );
					eae6320::cScopeGuard scopeGuard_color( [&io_luaState]
						{
							lua_pop( &io_luaState, 1 );
						} );

					o_materials[i_index].baseColor[i - 1] = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "baseColorTexName";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_isstring( &io_luaState, -1 ) )
			{
				std::string baseColorTexName = lua_tostring( &io_luaState, -1 );
				if( !baseColorTexName.empty() )
				{ 
					std::string textureName, texturePath;
					GetFilePathandFileName( baseColorTexName, texturePath, textureName );

					std::string targetFilePath = i_targetPath + "/" + meshFileName + "/" + textureName;

					std::string errorMessage;

					eae6320::Platform::CreateDirectoryIfItDoesntExist( targetFilePath.c_str(), &errorMessage );

					if ( !( result = eae6320::Platform::CopyFile( baseColorTexName.c_str(), targetFilePath.c_str(), false, true, &errorMessage ) ) )
					{
						eae6320::Assets::OutputErrorMessageWithFileInfo( baseColorTexName.c_str(), errorMessage.c_str() );
						return result;
					}

					o_materials[i_index].baseColorTexName = meshFileName + "/" + textureName;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a string (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "specularColor";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				for ( uint32_t i = 1; i <= 3; ++i )
				{
					lua_pushinteger( &io_luaState, i );
					lua_gettable( &io_luaState, -2 );
					eae6320::cScopeGuard scopeGuard_color( [&io_luaState]
						{
							lua_pop( &io_luaState, 1 );
						} );

					o_materials[i_index].specularColor[i - 1] = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "specularColorTexName";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_isstring( &io_luaState, -1 ) )
			{
				std::string specularTexName = lua_tostring( &io_luaState, -1 );
				if ( !specularTexName.empty() )
				{
					std::string textureName, texturePath;
					GetFilePathandFileName( specularTexName, texturePath, textureName );

					std::string targetFilePath = i_targetPath + "/" + meshFileName + "/" + textureName;

					std::string errorMessage;

					eae6320::Platform::CreateDirectoryIfItDoesntExist( targetFilePath.c_str(), &errorMessage );

					if ( !( result = eae6320::Platform::CopyFile( specularTexName.c_str(), targetFilePath.c_str(), false, true, &errorMessage ) ) )
					{
						eae6320::Assets::OutputErrorMessageWithFileInfo( specularTexName.c_str(), errorMessage.c_str() );
						return result;
					}

					o_materials[i_index].specularColorTexName = meshFileName + "/" + textureName;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a string (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "ambient";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				for ( uint32_t i = 1; i <= 3; ++i )
				{
					lua_pushinteger( &io_luaState, i );
					lua_gettable( &io_luaState, -2 );
					eae6320::cScopeGuard scopeGuard_color( [&io_luaState]
						{
							lua_pop( &io_luaState, 1 );
						} );

					o_materials[i_index].ambient[i - 1] = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "ambientTexName";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_isstring( &io_luaState, -1 ) )
			{
				o_materials[i_index].ambientTexName = lua_tostring( &io_luaState, -1 );
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a string (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "normalTexName";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_isstring( &io_luaState, -1 ) )
			{
				std::string normalTexName = lua_tostring( &io_luaState, -1 );
				if ( !normalTexName.empty() )
				{
					std::string textureName, texturePath;
					GetFilePathandFileName( normalTexName, texturePath, textureName );

					std::string targetFilePath = i_targetPath + "/" + meshFileName + "/" + textureName;

					std::string errorMessage;

					eae6320::Platform::CreateDirectoryIfItDoesntExist( targetFilePath.c_str(), &errorMessage );

					if ( !( result = eae6320::Platform::CopyFile( normalTexName.c_str(), targetFilePath.c_str(), false, true, &errorMessage ) ) )
					{
						eae6320::Assets::OutputErrorMessageWithFileInfo( normalTexName.c_str(), errorMessage.c_str() );
						return result;
					}

					o_materials[i_index].normalTexName = meshFileName + "/" + textureName;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a string (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "transparency";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				for ( uint32_t i = 1; i <= 3; ++i )
				{
					lua_pushinteger( &io_luaState, i );
					lua_gettable( &io_luaState, -2 );
					eae6320::cScopeGuard scopeGuard_color( [&io_luaState]
						{
							lua_pop( &io_luaState, 1 );
						} );

					o_materials[i_index].transparency[i - 1] = static_cast<float>( lua_tonumber( &io_luaState, -1 ) );
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "transparencyTexName";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_isstring( &io_luaState, -1 ) )
			{
				std::string transparencyTexName = lua_tostring( &io_luaState, -1 );
				if ( !transparencyTexName.empty() )
				{
					std::string textureName, texturePath;
					GetFilePathandFileName( transparencyTexName, texturePath, textureName );

					std::string targetFilePath = i_targetPath + "/" + meshFileName + "/" + textureName;

					std::string errorMessage;

					eae6320::Platform::CreateDirectoryIfItDoesntExist( targetFilePath.c_str(), &errorMessage );

					if ( !( result = eae6320::Platform::CopyFile( transparencyTexName.c_str(), targetFilePath.c_str(), false, true, &errorMessage ) ) )
					{
						eae6320::Assets::OutputErrorMessageWithFileInfo( transparencyTexName.c_str(), errorMessage.c_str() );
						return result;
					}

					o_materials[i_index].transparencyTexName = meshFileName + "/" + textureName;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a string (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "vertexRange";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				for( size_t i = 1; i <= 2; ++i )
				{
					lua_pushinteger( &io_luaState, i );
					lua_gettable( &io_luaState, -2 );
					eae6320::cScopeGuard scopeGuard_vertexRange( [&io_luaState]
						{
							lua_pop( &io_luaState, 1 );
						} );
					
					if ( lua_isinteger( &io_luaState, -1 ) )
					{
						if ( i == 1 )
							o_materials[i_index].vertexRange.first = static_cast<uint32_t>( lua_tointeger( &io_luaState, -1 ) );
						else
							o_materials[i_index].vertexRange.last = static_cast<uint32_t>( lua_tointeger( &io_luaState, -1 ) );
					}
					else
					{
						result = eae6320::Results::InvalidFile;
						return result;
					}
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		{
			constexpr auto* const key = "indexRange";
			lua_pushstring( &io_luaState, key );
			lua_gettable( &io_luaState, -2 );

			eae6320::cScopeGuard scopeGuard( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );

			if ( lua_istable( &io_luaState, -1 ) )
			{
				for ( size_t i = 1; i <= 2; ++i )
				{
					lua_pushinteger( &io_luaState, i );
					lua_gettable( &io_luaState, -2 );
					eae6320::cScopeGuard scopeGuard_indexRange( [&io_luaState]
						{
							lua_pop( &io_luaState, 1 );
						} );

					if ( lua_isinteger( &io_luaState, -1 ) )
					{
						if ( i == 1 )
							o_materials[i_index].indexRange.first = static_cast<uint32_t>( lua_tointeger( &io_luaState, -1 ) );
						else
							o_materials[i_index].indexRange.last = static_cast<uint32_t>( lua_tointeger( &io_luaState, -1 ) );
					}
					else
					{
						result = eae6320::Results::InvalidFile;
						return result;
					}
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessage( "The value at \" %s \" must be a table (instead of a %s )", key, luaL_typename( &io_luaState, -1 ) );
				return result;
			}
		}

		return result;
	}

	void GetFilePathandFileName( const std::string& i_path, std::string& o_path, std::string& o_filename )
	{
		std::string::size_type found = i_path.find_last_of( "/\\" );

		if ( found != std::string::npos )
		{
			o_path = i_path.substr( 0, found );
			o_filename = i_path.substr( found + 1 );
		}
		else
		{
			o_path.clear();
			o_filename = i_path;
		}
	}
}
