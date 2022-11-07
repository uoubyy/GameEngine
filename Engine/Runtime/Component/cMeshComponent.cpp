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
}

eae6320::cResult eae6320::Runtime::cMeshComponent::ChangeMesh( class Graphics::cMesh* i_mesh )
{
	if( i_mesh == m_mesh || i_mesh == nullptr ) return Results::Failure;

	if( m_mesh )
		m_mesh->DecrementReferenceCount();

	m_mesh = i_mesh;
	m_mesh->IncrementReferenceCount();
	return Results::Success;
}

eae6320::cResult eae6320::Runtime::cMeshComponent::ChangeMesh( const std::string& i_mesh_file_path )
{
	auto result = Results::Success;

	// TODO: Safety check
	if ( m_mesh )
		m_mesh->DecrementReferenceCount();

	if ( !( result = eae6320::Graphics::cMesh::Load( i_mesh_file_path.c_str(), m_mesh ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize mesh." );
	}
	return result;
}

eae6320::cResult eae6320::Runtime::cMeshComponent::GenerateRenderData( eae6320::Graphics::sRenderCommand& i_renderCommand )
{
	if( !m_visible ) return Results::Failure;

	return i_renderCommand.SetRenderCommand( m_mesh );
}

eae6320::cResult eae6320::Runtime::cMeshComponent::CleanUp()
{
	if ( m_mesh )
	{
		m_mesh->DecrementReferenceCount();
		m_mesh = nullptr;
	}

	return Results::Success;
}
