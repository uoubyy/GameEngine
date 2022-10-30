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
		// POSITION
		// 3 floats == 12 bytes
		// Offset = 0
		float x, y, z;

		uint8_t r, g, b, a;
	};

	eae6320::cResult LoadMesh(const char* const i_path, sVertex_mesh*& o_vertexData, void*& o_indices, uint32_t& o_triangleCount, uint32_t& o_vertexCount);

	eae6320::cResult LoadElements( lua_State& io_luaState, void*& o_indices, uint32_t& o_triangleCount );
	eae6320::cResult LoadElementsValues( lua_State& io_luaState, void*& o_indices, uint32_t& o_triangleCount );
	eae6320::cResult LoadElement( lua_State& io_luaState, void* o_indices, uint32_t i_index, bool i_32 );

	eae6320::cResult LoadVertices( lua_State& io_luaState, sVertex_mesh*& o_vertexData, uint32_t& o_vertexCount );
	eae6320::cResult LoadVerticesValues( lua_State& io_luaState, sVertex_mesh*& o_vertexData, uint32_t& o_vertexCount );
	eae6320::cResult LoadVertex( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );
	eae6320::cResult LoadVertexPosition( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );
	eae6320::cResult LoadVertexColor( lua_State& io_luaState, sVertex_mesh* o_vertexData, uint32_t i_index );
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

	uint32_t indiceCount = 0;
	uint32_t triangleCount = 0;
	uint32_t vertexCount = 0;

	eae6320::cScopeGuard scopeGuard_onExit( [ &vertexData, &indices ]
		{
			if ( vertexData )
			{
				delete[] vertexData;
			}
			if ( indices)
			{
				delete[] indices;
			}
		});

	if ( !( result = LoadMesh(m_path_source, vertexData, indices, triangleCount, vertexCount ) ) )
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

		binaryMeshFile.write( buffer, bufferSize );
		binaryMeshFile.close();
	}

	//if ( !( result = eae6320::Platform::CopyFile( m_path_source, m_path_target, noErrorIfTargetAlreadyExists, updateTheTargetFileTime, &errorMessage) ) )
	//{
	//	eae6320::Assets::OutputErrorMessageWithFileInfo( m_path_target, errorMessage.c_str() );
	//	return result;
	//}

	return result;
}

// Helper Definitions
//===================

namespace
{
	eae6320::cResult LoadMesh(const char* const i_path, sVertex_mesh*& o_vertexData, void*& o_indices, uint32_t& o_triangleCount, uint32_t& o_vertexCount)
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
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "Failed to create a new Lua state" );
				return result;
			}
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		const auto stackTopBeforeLoad = lua_gettop( luaState );
		{
			const auto luaResult = luaL_loadfile( luaState, i_path );
			if ( luaResult != LUA_OK )
			{
				result = eae6320::Results::Failure;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, lua_tostring(luaState, -1) );
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
						eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "Asset files must return a table (instead of a %s )", luaL_typename( luaState, -1 ) );
						// Pop the returned non-table value
						lua_pop( luaState, 1 );
						return result;
					}
				}
				else
				{
					result = eae6320::Results::InvalidFile;
					eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, "Asset files must return a single table (instead of %s values)", luaL_typename( luaState, -1 ) );
					// Pop every value that was returned
					lua_pop( luaState, returnedValueCount );
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				eae6320::Assets::OutputErrorMessageWithFileInfo( i_path, luaL_typename( luaState, -1 ) );
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

			eae6320::cScopeGuard scopeGuard_vertex( [&io_luaState]
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

			eae6320::cScopeGuard scopeGuard_vertex( [&io_luaState]
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
}
