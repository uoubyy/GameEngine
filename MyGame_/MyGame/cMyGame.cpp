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

	eae6320::Graphics::cMesh* s_mesh1 = nullptr;
	eae6320::Graphics::cMesh* s_mesh2 = nullptr;
	eae6320::Graphics::cMesh* s_mesh3 = nullptr;

	//eae6320::cCharacter s_pawn1;
	eae6320::cCharacter s_pawn2;
	// eae6320::cCharacter s_pawn3;
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

	//if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Num_1 ) )
	//{
	//	s_pawn1.m_meshComponent.SetVisible( true );
	//	s_pawn2.m_meshComponent.SetVisible( false );
	//}
	//else if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Num_2 ) )
	//{
	//	s_pawn1.m_meshComponent.SetVisible( false );
	//	s_pawn2.m_meshComponent.SetVisible( true );
	//}
	//else
	//{
	//	s_pawn1.m_meshComponent.SetVisible( true );
	//	s_pawn2.m_meshComponent.SetVisible( true );
	//}

	//if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Num_3 ) )
	//{
	//	s_pawn1.m_meshComponent.ChangeMesh( s_mesh1 );
	//}
	//if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Num_4 ) )
	//{
	//	s_pawn1.m_meshComponent.ChangeMesh( s_mesh2 );
	//}

	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Num_5 ) )
	{
		s_targetCamera = s_camera1;
	}
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Num_6 ) )
	{
		s_targetCamera = s_camera2;
	}

	s_targetCamera->UpdateSimulationBasedOnInput();

	//s_pawn1.UpdateSimulationBasedOnInput();
	s_pawn2.UpdateSimulationBasedOnInput();
	//s_pawn3.UpdateSimulationBasedOnInput();
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate )
{
	s_targetCamera->UpdateSimulationBasedOnTime( i_elapsedSecondCount_sinceLastUpdate );

	//s_pawn1.UpdateSimulationBasedOnTime( i_elapsedSecondCount_sinceLastUpdate );
	s_pawn2.UpdateSimulationBasedOnTime( i_elapsedSecondCount_sinceLastUpdate );
	//s_pawn3.UpdateSimulationBasedOnTime( i_elapsedSecondCount_sinceLastUpdate );
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

	// If not visible, GenerateRenderData will failed
	//if ( s_pawn1.GenerateRenderData( s_renderCommand[s_renderCommandNums], i_elapsedSecondCount_sinceLastSimulationUpdate ) )
	//	s_renderCommandNums++;

	if ( s_pawn2.GenerateRenderData( s_renderCommand[s_renderCommandNums], i_elapsedSecondCount_sinceLastSimulationUpdate ) )
		s_renderCommandNums++;

	//if ( s_pawn3.GenerateRenderData(s_renderCommand[s_renderCommandNums], i_elapsedSecondCount_sinceLastSimulationUpdate ) )
	//	s_renderCommandNums++;

	int commandNums = s_renderCommandNums;

	eae6320::Graphics::SubmitRenderCommands( s_renderCommand, commandNums );
	
	eae6320::Graphics::SubmitCamera( s_targetCamera->GetViewMatrix( i_elapsedSecondCount_sinceLastSimulationUpdate ), s_targetCamera->GetProjectionMatrix() );

	s_renderCommandNums = 0;
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	auto result = eae6320::Results::Success;

	const char* windowName = GetMainWindowName();
	eae6320::Logging::OutputMessage( "My Game: \"%s\" start initialize.", windowName );

	//{
	//	if ( !( result = eae6320::Graphics::cMesh::Load( "data/Meshes/plane.dat", s_mesh1 ) ) )
	//	{
	//		EAE6320_ASSERTF( false, "Can't initialize mesh 1." );
	//		return result;
	//	}
	//}

	{
		if ( !( result = eae6320::Graphics::cMesh::Load( "data/Meshes/Taser.dat", s_mesh2 ) ) )
		{
			EAE6320_ASSERTF( false, "Can't initialize mesh 2.");
			return result;
		}
	}

	//{
	//	if ( !( result = eae6320::Graphics::cMesh::Load( "data/Meshes/helix.dat", s_mesh3 ) ) )
	//	{
	//		EAE6320_ASSERTF(false, "Can't initialize mesh 3.");
	//		return result;
	//	}
	//}

	//if ( !( result = s_pawn1.m_meshComponent.InitializeShadingData( "data/Shaders/Vertex/standard.shader",
	//	"data/Shaders/Fragment/animatedColor.shader" ) ) )
	//{
	//	EAE6320_ASSERTF( false, "Can't initialize shading data for pawn 1" );
	//	return result;
	//}

	if ( !( result = s_pawn2.m_meshComponent.InitializeShadingData( "data/Shaders/Vertex/standard.shader",
		"data/Shaders/Fragment/animatedColor.shader" ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize shading data for pawn 2" );
		return result;
	}

	//if ( !( result = s_pawn3.m_meshComponent.InitializeShadingData( "data/Shaders/Vertex/standard.shader",
	//	"data/Shaders/Fragment/standard.shader" ) ) )
	//{
	//	EAE6320_ASSERTF( false, "Can't initialize shading data for pawn 3" );
	//	return result;
	//}

	//{
	//	s_pawn1.m_meshComponent.ChangeMesh( s_mesh1 );
	//	eae6320::Math::sVector position( 0.0f, 0.0f, 0.0f );
	//	s_pawn1.m_movementComponent.SetPosition( position );
	//}

	{
		s_pawn2.m_meshComponent.ChangeMesh( s_mesh2 );
		s_pawn2.SetUpInput( true );
		s_pawn2.SetUniformScale( 0.1f );
		eae6320::Math::sVector position( 0.0f, 20.0f, -10.0f );
		s_pawn2.m_movementComponent.SetPosition( position );
	}

	//{
	//	s_pawn3.m_meshComponent.ChangeMesh( s_mesh3 );
	//	eae6320::Math::sVector position( -5.0f, 2.0f, -5.0f );
	//	s_pawn3.m_movementComponent.SetPosition( position );
	//}

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

	if ( s_mesh1 )
	{
		s_mesh1->DecrementReferenceCount();
		s_mesh1 = nullptr;
	}

	if ( s_mesh2 )
	{
		s_mesh2->DecrementReferenceCount();
		s_mesh2 = nullptr;
	}

	if ( s_mesh3 )
	{
		s_mesh3->DecrementReferenceCount();
		s_mesh3 = nullptr;
	}

	//s_pawn1.CleanUp();

	s_pawn2.CleanUp();

	//s_pawn3.CleanUp();

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
