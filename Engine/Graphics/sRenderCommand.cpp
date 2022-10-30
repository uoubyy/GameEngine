// Includes
//=========

#include "sRenderCommand.h"
#include "cMesh.h"
#include "cEffect.h"

#include <Engine/Logging/Logging.h>

// Interface
//==========

// Initialize / Clean Up
//----------------------"

eae6320::cResult eae6320::Graphics::sRenderCommand::SetRenderCommand( cMesh* i_mesh, cEffect* i_effect )
{
	m_mesh = i_mesh;
	m_effect = i_effect;

	if( m_mesh )
	{ 
		m_mesh->IncrementReferenceCount();
	}
	else
	{
		return Results::Failure;
	}

	if( m_effect )
	{ 
		m_effect->IncrementReferenceCount();
	}

	return Results::Success;
}

eae6320::cResult eae6320::Graphics::sRenderCommand::CleanUp()
{
	if( m_mesh )
		m_mesh->DecrementReferenceCount();
	if( m_effect )
		m_effect->DecrementReferenceCount();

	m_mesh = nullptr;
	m_effect = nullptr;

	return Results::Success;
}