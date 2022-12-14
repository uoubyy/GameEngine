// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Logging/Logging.h>

#include <Engine/Graphics/Graphics.h>
#include <Engine/Graphics/sRenderCommand.h>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Graphics/VertexFormats.h>

#include <Engine/Math/Functions.h>
#include <Engine/Runtime/Gameobject/cCamera.h>

#include "Gameobject/cCharacter.h"

#include <utility>

// Inherited Implementation
//=========================

// Run
//----

namespace
{
	constexpr auto MAX_COMMAND_BUFFER_SIZE = std::numeric_limits<uint16_t>::max();

	eae6320::Graphics::sRenderCommand s_renderCommand[MAX_COMMAND_BUFFER_SIZE];

	uint16_t s_renderCommandNums = 0;

	eae6320::Runtime::cCamera* s_camera1 = nullptr;
	eae6320::Runtime::cCamera* s_camera2 = nullptr;

	eae6320::Runtime::cCamera* s_targetCamera = nullptr;

	eae6320::cCharacter s_backpack;
}

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cMyGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{

}

void eae6320::cMyGame::UpdateSimulationBasedOnInput()
{
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::P ) )
	{
		SetSimulationRate( 0.0f );
	}
	else
	{
		SetSimulationRate( 1.0f );
	}

	// Render Command Buffer full
	if( s_renderCommandNums >= MAX_COMMAND_BUFFER_SIZE - 2 )
	{
		eae6320::Logging::OutputMessage( "Render Command Buffer Full!." );
		return;
	}

	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Num_5 ) )
	{
		s_targetCamera = s_camera1;
	}
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Num_6 ) )
	{
		s_targetCamera = s_camera2;
	}

	s_targetCamera->UpdateSimulationBasedOnInput();

	s_backpack.UpdateSimulationBasedOnInput();
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate )
{
	s_targetCamera->UpdateSimulationBasedOnTime( i_elapsedSecondCount_sinceLastUpdate );

	s_backpack.UpdateSimulationBasedOnTime( i_elapsedSecondCount_sinceLastUpdate );
}

void eae6320::cMyGame::SubmitDataToBeRendered( const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate )
{
	{
		const double elapsedSecondCount = GetElapsedSecondCount_simulation();

		//float r = static_cast<float>( std::cos( elapsedSecondCount ) ) * 0.5f + 0.5f;
		//float g = static_cast<float>( std::sin( elapsedSecondCount ) ) * 0.5f + 0.5f;

		const float clearColor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };

		eae6320::Graphics::SubmitClearColor( clearColor );
	}

	if ( s_backpack.GenerateRenderData( s_renderCommand[s_renderCommandNums], i_elapsedSecondCount_sinceLastSimulationUpdate ) )
		s_renderCommandNums++;

	int commandNums = s_renderCommandNums;

	eae6320::Graphics::SubmitRenderCommands( s_renderCommand, commandNums );
	
	eae6320::Graphics::SubmitCamera( s_targetCamera->GetViewMatrix( i_elapsedSecondCount_sinceLastSimulationUpdate ), s_targetCamera->GetProjectionMatrix(), s_targetCamera->m_movementComponent.GetPredictPosition( i_elapsedSecondCount_sinceLastSimulationUpdate ) );

	s_renderCommandNums = 0;
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	auto result = eae6320::Results::Success;

	const char* windowName = GetMainWindowName();
	eae6320::Logging::OutputMessage( "My Game: \"%s\" start initialize.", windowName );

	{
		s_backpack.m_meshComponent.ChangeMesh( "data/Meshes/backpack.dat" );
		s_backpack.SetUpInput( true );
		s_backpack.SetUniformScale( 0.8f );
		eae6320::Math::sVector position( 0.0f, 2.0f, 0.0f );
		s_backpack.m_movementComponent.SetPosition( position );
		// s_backpack.m_movementComponent.SetAngularSpeed( 1.0f );
	}

	if ( !( result = eae6320::Runtime::cCamera::Load( s_camera1 ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize camera 1" );
		return result;
	}
	else
	{
		eae6320::Math::sVector position( 0.0f, 2.0f, 5.0f );
		s_camera1->SetPosition( position );

		s_camera1->m_z_nearPlane = 0.1f;
		s_camera1->m_z_farPlane = 100.0f;
		s_camera1->m_verticalFieldOfView_inRadians = eae6320::Math::ConvertDegreesToRadians( 45.0f );

		s_targetCamera = s_camera1;
	}

	if ( !( result = eae6320::Runtime::cCamera::Load( s_camera2 ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize camera 2" );
		return result;
	}
	else
	{
		eae6320::Math::sVector position( 0.0f, 0.0f, 2.0f );
		s_camera2->SetPosition( position );

		s_camera2->m_z_nearPlane = 0.1f;
		s_camera2->m_z_farPlane = 100.0f;
		s_camera2->m_verticalFieldOfView_inRadians = eae6320::Math::ConvertDegreesToRadians( 45.0f );
	}

	return result;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	// If early exit, we need to clean render the commands buffer
	// that hasn't been submitted to graphics
	for ( size_t i = 0; i < MAX_COMMAND_BUFFER_SIZE; ++i )
	{
		s_renderCommand[i].CleanUp();
		// For the buffer has been submitted and in rendering 
		// Graphics will clean them
	}

	s_backpack.CleanUp();

	if ( s_camera1 )
	{
		s_camera1->DecrementReferenceCount();
		s_camera1 = nullptr;
	}

	if ( s_camera2 )
	{
		s_camera2->DecrementReferenceCount();
		s_camera2 = nullptr;
	}

	s_targetCamera = nullptr;

	const char* windowName = GetMainWindowName();
	eae6320::Logging::OutputMessage( "My Game: \"%s\" start cleanup.", windowName );
	return Results::Success;
}
