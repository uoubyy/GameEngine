#ifndef EAE6320_CCHARACTER_H
#define EAE6320_CCHARACTER_H

#include <Engine/Runtime/Gameobject/iGameobject.h>
#include <Engine/Runtime/Component/cMeshComponent.h>
#include <Engine/Runtime/Component/cMovementComponent.h>
#include <Engine/Graphics/sRenderCommand.h>

namespace eae6320
{
	class cCharacter final : public Runtime::iGameobject
	{
	public:
		
		cCharacter();

		~cCharacter();

		Runtime::cMeshComponent m_meshComponent;

		Runtime::cMovementComponent m_movementComponent;

		void UpdateBasedOnInput() final;

		void UpdateBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate ) final;

		void UpdateSimulationBasedOnInput() final;

		void UpdateSimulationBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate ) final;

		cResult GenerateRenderData( eae6320::Graphics::sRenderCommand& i_renderCommand, const float i_elapsedSecondCount_sinceLastUpdate );

		cResult CleanUp();

		void SetUpInput( bool i_inputEnable ) { m_inputEnabled = i_inputEnable; }

	private:
		bool m_inputEnabled = false;
	};
}

#endif // EAE6320_CCHARACTER_H