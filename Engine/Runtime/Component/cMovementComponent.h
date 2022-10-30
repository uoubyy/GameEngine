#ifndef EAE6320_RUNTIME_CMOVEMENT_COMPONENT_H
#define EAE6320_RUNTIME_CMOVEMENT_COMPONENT_H

// Includes
//=========

#include "iComponent.h"

#include <Engine/Physics/sRigidBodyState.h>

namespace eae6320
{
	namespace Runtime
	{
		class cMovementComponent final: public iComponent
		{
		public:
				
			// Initialization / Clean Up
			//--------------------------

			cMovementComponent();

			~cMovementComponent();

			void UpdateSimulationBasedOnInput();

			void UpdateSimulationBasedOnTime( const float i_elapsedSecondCount_sinceLastUpdate );

			void SetVeolocity( const Math::sVector& i_velocity );

			void SetPosition( const Math::sVector& i_position );

			void SetRotation( const Math::cQuaternion& i_orientation );

			Math::sVector GetPredictPosition( const float i_secondCountToExtrapolate ) const;

			Math::cQuaternion GetPredictOrientation( const float i_secondCountToExtrapolate ) const;

			Math::cMatrix_transformation GetPredictTransform( const float i_secondCountToExtrapolate ) const;

			cResult CleanUp();

		private:
			// Data
			Physics::sRigidBodyState m_rigidBodystate;
		};
	}
}

#endif // EAE6320_RUNTIME_CMOVEMENT_COMPONENT_H