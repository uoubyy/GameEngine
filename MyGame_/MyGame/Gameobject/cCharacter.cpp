#include "cCharacter.h"
#include <Engine/UserInput/UserInput.h>

eae6320::cCharacter::cCharacter()
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

	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::W ) )
	{
		m_movementComponent.SetVeolocity( Math::sVector( 0.0f, 1.0f, 0.0f ) );
	}
	else if ( UserInput::IsKeyPressed( UserInput::KeyCodes::S ) )
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
}

void eae6320::cCharacter::UpdateSimulationBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate )
{
	m_movementComponent.UpdateSimulationBasedOnTime( i_elapsedSecondCount_sinceLastUpdate );
}

eae6320::cResult eae6320::cCharacter::GenerateRenderData( eae6320::Graphics::sRenderCommand& i_renderCommand, const float i_elapsedSecondCount_sinceLastUpdate )
{
	auto result = m_meshComponent.GenerateRenderData( i_renderCommand );

	if( result )
		i_renderCommand.m_transformation = m_movementComponent.GetPredictTransform( i_elapsedSecondCount_sinceLastUpdate );

	return result;
}

eae6320::cResult eae6320::cCharacter::CleanUp()
{
	auto result = m_meshComponent.CleanUp();

	return result;
}
