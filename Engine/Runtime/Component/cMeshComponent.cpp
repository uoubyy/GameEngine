// Includes
//=========

#include "cMeshComponent.h"
#include "../../Graphics/cMesh.h"
#include "../../Graphics/VertexFormats.h"
#include "../../Graphics/cEffect.h"

eae6320::Runtime::cMeshComponent::cMeshComponent()
{

}

eae6320::Runtime::cMeshComponent::~cMeshComponent()
{
	if ( m_mesh )
	{
		m_mesh->DecrementReferenceCount();
		m_mesh = nullptr;
	}

	if ( m_effect )
	{
		m_effect->DecrementReferenceCount();
		m_effect = nullptr;
	}
}

eae6320::cResult eae6320::Runtime::cMeshComponent::InitializeGeometry( const Graphics::VertexFormats::sVertex_mesh* i_vertexData, const uint16_t* i_indices, const unsigned int i_triangleCount, const unsigned int i_vertexCount )
{
	if( m_mesh )
	{ 
		m_mesh->DecrementReferenceCount();
		m_mesh = nullptr;
	}

	auto result = eae6320::Results::Success;

	//if ( !( result = eae6320::Graphics::cMesh::Load( i_vertexData, i_indices, i_triangleCount, i_vertexCount, m_mesh ) ) )
	//{
	//	EAE6320_ASSERTF( false, "Can't initialize mesh." );
	//}

	return result;
}

eae6320::cResult eae6320::Runtime::cMeshComponent::InitializeShadingData( const std::string& i_vertexShaderPath, const std::string& i_fragmentShaderPath )
{
	if( m_effect )
	{
		m_effect->DecrementReferenceCount();
		m_effect = nullptr;
	}

	auto result = eae6320::Results::Success;

	if ( !( result = eae6320::Graphics::cEffect::Load( i_vertexShaderPath, i_fragmentShaderPath, m_effect ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize shading data." );
	}

	return result;
}

eae6320::cResult eae6320::Runtime::cMeshComponent::ChangeMesh(class Graphics::cMesh* i_mesh)
{
	if( i_mesh == m_mesh || i_mesh == nullptr ) return Results::Failure;

	if( m_mesh )
		m_mesh->DecrementReferenceCount();

	m_mesh = i_mesh;
	m_mesh->IncrementReferenceCount();
	return Results::Success;
}

eae6320::cResult eae6320::Runtime::cMeshComponent::ChangeEffect(class Graphics::cEffect* i_effect)
{
	if( i_effect == m_effect || i_effect == nullptr ) return Results::Failure;

	if ( m_effect )
		m_effect->DecrementReferenceCount();

	m_effect = i_effect;
	m_effect->IncrementReferenceCount();
	return Results::Success;
}

eae6320::cResult eae6320::Runtime::cMeshComponent::GenerateRenderData( eae6320::Graphics::sRenderCommand& i_renderCommand )
{
	if( !m_visible ) return Results::Failure;

	return i_renderCommand.SetRenderCommand( m_mesh, m_effect );
}

eae6320::cResult eae6320::Runtime::cMeshComponent::CleanUp()
{
	if ( m_mesh )
	{
		m_mesh->DecrementReferenceCount();
		m_mesh = nullptr;
	}

	if ( m_effect )
	{
		m_effect->DecrementReferenceCount();
		m_effect = nullptr;
	}

	return Results::Success;
}
