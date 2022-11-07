// Includes
//=========

#include "../cMesh.h"
#include "../cMaterial.h"

#include "Includes.h"
#include "../sContext.h"
#include "../cVertexFormat.h"
#include "../VertexFormats.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

// Implementation
//===============

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cMesh::Initialize( const VertexFormats::sVertex_mesh* i_vertexData, const void* i_indices, const uint32_t i_triangleCount, const uint32_t i_vertexCount, const uint16_t i_materialsCount, cMaterial** i_materials )
{
	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT( direct3dDevice );

	auto result = eae6320::Results::Success;

	// Vertex Format
	{
		if ( !( result = eae6320::Graphics::cVertexFormat::Load( eae6320::Graphics::eVertexType::Mesh, m_vertexFormat,
			"data/Shaders/Vertex/vertexInputLayout_mesh.shader") ) )
		{
			EAE6320_ASSERTF( false, "Can't initialize geometry without vertex format" );
			return result;
		}
	}
	// Vertex Buffer
	{
		unsigned int bufferSize = sizeof( i_vertexData[0] ) * i_vertexCount;
		EAE6320_ASSERT( bufferSize <= std::numeric_limits<decltype( D3D11_BUFFER_DESC::ByteWidth )>::max() );

		const auto bufferDescription = [bufferSize]
		{
			D3D11_BUFFER_DESC bufferDescription{};

			bufferDescription.ByteWidth = bufferSize;
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used

			return bufferDescription;
		}();

		const auto initialData = [i_vertexData]
		{
			D3D11_SUBRESOURCE_DATA initialData{};

			initialData.pSysMem = i_vertexData;
			// (The other data members are ignored for non-texture buffers)

			return initialData;
		}();

		const auto result_create = direct3dDevice->CreateBuffer( &bufferDescription, &initialData, &m_vertexBuffer );
		if ( FAILED( result_create ) )
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create );
			eae6320::Logging::OutputError( "Direct3D failed to create a 3D object vertex buffer (HRESULT %#010x)", result_create );
			return result;
		}
	}
	// Index Buffer
	{
		constexpr unsigned int vertexCountPerTriangle = 3;
		const auto indexCount = i_triangleCount * vertexCountPerTriangle;
		bool is32 = indexCount > std::numeric_limits<uint16_t>::max();
		size_t itemSize = is32 ? sizeof(uint32_t) : sizeof(uint16_t);

		auto bufferSize = itemSize * i_triangleCount * vertexCountPerTriangle;
		EAE6320_ASSERT( bufferSize <= std::numeric_limits<decltype( D3D11_BUFFER_DESC::ByteWidth )>::max() );

		const auto bufferDescription = [bufferSize]
		{
			D3D11_BUFFER_DESC bufferDescription{};

			bufferDescription.ByteWidth = static_cast<unsigned int>( bufferSize );
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used

			return bufferDescription;
		}();

		const auto initialData = [i_indices]
		{
			D3D11_SUBRESOURCE_DATA initialData{};

			initialData.pSysMem = i_indices;
			// (The other data members are ignored for non-texture buffers)

			return initialData;
		}();

		const auto result_create = direct3dDevice->CreateBuffer( &bufferDescription, &initialData, &m_indexBuffer );
		if ( FAILED( result_create ) )
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF( false, "3D object index buffer creation failed (HRESULT %#010x)", result_create );
			eae6320::Logging::OutputError( "Direct3D failed to create a 3D object index buffer (HRESULT %#010x)", result_create );
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

	if ( m_vertexBuffer )
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if ( m_indexBuffer )
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	if ( m_vertexFormat )
	{
		m_vertexFormat->DecrementReferenceCount();
		m_vertexFormat = nullptr;
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
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT(direct3dImmediateContext);

	// Draw the geometry
	{
		// Bind a specific vertex buffer to the device as a data source
		{
			EAE6320_ASSERT( m_vertexBuffer != nullptr );
			constexpr unsigned int startingSlot = 0;
			constexpr unsigned int vertexBufferCount = 1;
			// The "stride" defines how large a single vertex is in the stream of data
			constexpr unsigned int bufferStride = sizeof( VertexFormats::sVertex_mesh );
			// It's possible to start streaming data in the middle of a vertex buffer
			constexpr unsigned int bufferOffset = 0;
			direct3dImmediateContext->IASetVertexBuffers( startingSlot, vertexBufferCount, &m_vertexBuffer, &bufferStride, &bufferOffset );
		}
		// Specify what kind of data the vertex buffer holds
		{
			// Bind the vertex format (which defines how to interpret a single vertex)
			{
				EAE6320_ASSERT( m_vertexFormat != nullptr );
				m_vertexFormat->Bind();
			}
			// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
			// the vertex buffer was defined as a triangle list
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			direct3dImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		}

		constexpr unsigned int vertexCountPerTriangle = 3;
		unsigned int indexCountToRender = m_triangleCount * vertexCountPerTriangle;
		bool is32 = indexCountToRender > std::numeric_limits<uint16_t>::max() ? true : false;

		// Bind the index buffer
		{
			EAE6320_ASSERT( m_indexBuffer );
			constexpr unsigned int offset = 0;
			direct3dImmediateContext->IASetIndexBuffer( m_indexBuffer, is32 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, offset );
		}
		// Render triangles from the currently-bound index buffer
		{
			// It's possible to start streaming data in the middle of a vertex buffer
			constexpr unsigned int indexOfFirstIndexToUse = 0;
			constexpr unsigned int offsetToAddToEachIndex = 0;

			if ( m_materialsCount > 0 )
			{
				for ( auto i = 0; i < m_materialsCount; ++i )
				{
					m_materials[i]->Bind();
					direct3dImmediateContext->DrawIndexed( m_materials[i]->m_indexRange.last - m_materials[i]->m_indexRange.first + 1, m_materials[i]->m_indexRange.first, offsetToAddToEachIndex );
				}
			}
			else
			{
				direct3dImmediateContext->DrawIndexed( indexCountToRender, indexOfFirstIndexToUse, offsetToAddToEachIndex );
			}
		}
	}
}