#include "cCharacter.h"
#include <Engine/UserInput/UserInput.h>

eae6320::cCharacter::cCharacter()
	: m_scale(1.0f, 1.0f, 1.0f)
{

}

eae6320::cCharacter::~cCharacter()
{

}

void eae6320::cCharacter::UpdateBasedOnInput()
{

}

void eae6320::cCharacter::UpdateBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate )
{

}

void eae6320::cCharacter::UpdateSimulationBasedOnInput()
{
	if( !m_inputEnabled ) return;

	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::W ) && !UserInput::IsKeyPressed( UserInput::KeyCodes::Control ) )
	{
		m_movementComponent.SetVeolocity( Math::sVector( 0.0f, 1.0f, 0.0f ) );
	}
	else if ( UserInput::IsKeyPressed( UserInput::KeyCodes::S ) && !UserInput::IsKeyPressed( UserInput::KeyCodes::Control ) )
	{
		m_movementComponent.SetVeolocity( Math::sVector( 0.0f, -1.0f, 0.0f ) );
	}
	else if ( UserInput::IsKeyPressed( UserInput::KeyCodes::A ) )
	{
		m_movementComponent.SetVeolocity( Math::sVector( -1.0f, 0.0f, 0.0f ) );
	}
	else if ( UserInput::IsKeyPressed( UserInput::KeyCodes::D ) )
	{
		m_movementComponent.SetVeolocity( Math::sVector( 1.0f, 0.0f, 0.0f ) );
	}
	else if ( UserInput::IsKeyPressed( UserInput::KeyCodes::R ) )
	{
		m_movementComponent.SetAngularSpeed( 1.0f );
	}
	else
	{
		m_movementComponent.SetVeolocity( Math::sVector( 0.0f, 0.0f, 0.0f ) );
		m_movementComponent.SetAngularSpeed( 0.0f );
	}
}

void eae6320::cCharacter::UpdateSimulationBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate )
{
	m_movementComponent.UpdateSimulationBasedOnTime( i_elapsedSecondCount_sinceLastUpdate );
}

eae6320::cResult eae6320::cCharacter::GenerateRenderData( eae6320::Graphics::sRenderCommand& i_renderCommand, const float i_elapsedSecondCount_sinceLastUpdate )
{
	auto result = m_meshComponent.GenerateRenderData( i_renderCommand );

	if( result )
	{ 
		eae6320::Math::cMatrix_transformation scaleMatrix( m_scale );
		i_renderCommand.m_transformation = scaleMatrix * m_movementComponent.GetPredictTransform( i_elapsedSecondCount_sinceLastUpdate );
	}

	return result;
}

eae6320::cResult eae6320::cCharacter::CleanUp()
{
	auto result = m_meshComponent.CleanUp();

	return result;
}
