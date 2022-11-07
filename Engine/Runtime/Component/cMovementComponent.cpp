// Includes
//=========

#include "cMovementComponent.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include "../../Math/cMatrix_transformation.h"

void eae6320::Runtime::cMovementComponent::UpdateSimulationBasedOnInput()
{

}

void eae6320::Runtime::cMovementComponent::UpdateSimulationBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate )
{
	m_rigidBodystate.Update( i_elapsedSecondCount_sinceLastUpdate );
}

void eae6320::Runtime::cMovementComponent::SetVeolocity( const Math::sVector& i_velocity )
{
	m_rigidBodystate.velocity = i_velocity;
}

void eae6320::Runtime::cMovementComponent::SetAcceleration( Math::sVector i_acceleration )
{
	m_rigidBodystate.acceleration = i_acceleration;
}

void eae6320::Runtime::cMovementComponent::SetAngularSpeed( const float i_angularSpeed )
{
	m_rigidBodystate.angularSpeed = i_angularSpeed;
}

void eae6320::Runtime::cMovementComponent::SetPosition( const Math::sVector& i_position)
{
	m_rigidBodystate.position = i_position;
}

void eae6320::Runtime::cMovementComponent::SetRotation( const Math::cQuaternion& i_orientation )
{
	m_rigidBodystate.orientation = i_orientation;
}

eae6320::Math::sVector eae6320::Runtime::cMovementComponent::GetPredictPosition( const float i_secondCountToExtrapolate ) const
{
	return m_rigidBodystate.PredictFuturePosition( i_secondCountToExtrapolate );
}

eae6320::Math::cQuaternion eae6320::Runtime::cMovementComponent::GetPredictOrientation( const float i_secondCountToExtrapolate ) const
{
	return m_rigidBodystate.PredictFutureOrientation( i_secondCountToExtrapolate );
}

eae6320::Math::cMatrix_transformation eae6320::Runtime::cMovementComponent::GetPredictTransform( const float i_secondCountToExtrapolate ) const
{
	return m_rigidBodystate.PredictFutureTransform( i_secondCountToExtrapolate );
}

eae6320::cResult eae6320::Runtime::cMovementComponent::CleanUp()
{
	return Results::Success;
}

eae6320::Runtime::cMovementComponent::cMovementComponent()
{

}

eae6320::Runtime::cMovementComponent::~cMovementComponent()
{

}

