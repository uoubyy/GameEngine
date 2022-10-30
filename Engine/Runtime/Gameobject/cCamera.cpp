// Includes
//=========

#include "cCamera.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Math/Functions.h>

#include <Engine/UserInput/UserInput.h>

#include <new>

eae6320::cResult eae6320::Runtime::cCamera::Load(cCamera*& o_camera)
{
	auto result = Results::Success;
	cCamera* newCamera = nullptr;

	cScopeGuard scopeGuard( [&result, &o_camera, &newCamera]
		{
			if ( result )
			{
				EAE6320_ASSERT( newCamera != nullptr );
				o_camera = newCamera;
			}
			else
			{
				if ( newCamera )
				{
					newCamera->DecrementReferenceCount();
					newCamera = nullptr;
				}
				o_camera = nullptr;
			}
		});

	newCamera = new (std::nothrow) cCamera();
	if ( !newCamera )
	{
		result = Results::OutOfMemory;
		EAE6320_ASSERTF( false, "Couldn't allocate memory for the Camera Object" );
		return result;
	}

	return result;
}

eae6320::Math::cMatrix_transformation eae6320::Runtime::cCamera::GetViewMatrix( const float i_elapsedSecondCount_sinceLastSimulationUpdate )
{
	return eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform( m_movementComponent.GetPredictOrientation( i_elapsedSecondCount_sinceLastSimulationUpdate), m_movementComponent.GetPredictPosition( i_elapsedSecondCount_sinceLastSimulationUpdate ) );
}

eae6320::Math::cMatrix_transformation eae6320::Runtime::cCamera::GetProjectionMatrix()
{
	return eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective( m_verticalFieldOfView_inRadians, m_aspectRatio, m_z_nearPlane, m_z_farPlane );
}

void eae6320::Runtime::cCamera::SetPosition( const Math::sVector& i_position )
{
	m_movementComponent.SetPosition( i_position );
}

void eae6320::Runtime::cCamera::SetOrientation( const Math::cQuaternion& i_orientation )
{
	m_movementComponent.SetRotation( i_orientation );
}

void eae6320::Runtime::cCamera::UpdateSimulationBasedOnInput()
{
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Up ) )
	{
		m_movementComponent.SetVeolocity( Math::sVector( 0.0f, -1.0f, 0.0f ) );
	}
	else if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Down ) )
	{
		m_movementComponent.SetVeolocity( Math::sVector( 0.0f, 1.0f, 0.0f ) );
	}
	else if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Left ) )
	{
		m_movementComponent.SetVeolocity( Math::sVector( 1.0f, 0.0f, 0.0f ) );
	}
	else if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Right ) )
	{
		m_movementComponent.SetVeolocity( Math::sVector( -1.0f, 0.0f, 0.0f ) );
	}
}

void eae6320::Runtime::cCamera::UpdateSimulationBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate )
{
	m_movementComponent.UpdateSimulationBasedOnTime( i_elapsedSecondCount_sinceLastUpdate );
}

eae6320::cResult eae6320::Runtime::cCamera::Initialize()
{
	return Results::Success;
}

eae6320::cResult eae6320::Runtime::cCamera::CleanUp()
{
	return Results::Success;
}

eae6320::Runtime::cCamera::cCamera()
{
	m_verticalFieldOfView_inRadians = eae6320::Math::ConvertDegreesToRadians( 60.0f );
	m_aspectRatio = 1.0f;
	m_z_nearPlane = 0.3f;
	m_z_farPlane = 1000.0f;
}

eae6320::Runtime::cCamera::~cCamera()
{

}

