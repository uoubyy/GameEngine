#ifndef EAE6320_RUNTIME_CCAMERA_H
#define EAE6320_RUNTIME_CCAMERA_H

// Includes
//=========

#include "iGameobject.h"

#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Physics/sRigidBodyState.h>
#include <Engine/Math/cMatrix_transformation.h>

#include <Engine/Runtime/Component/cMovementComponent.h>

namespace eae6320
{
	namespace Runtime
	{
		class cCamera final : public iGameobject
		{
		public:
			// Initialization / Clean Up
			//--------------------------

			static cResult Load( cCamera*& o_camera ) ;

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS( cCamera );

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// World space to view space
			Math::cMatrix_transformation GetViewMatrix( const float i_elapsedSecondCount_sinceLastSimulationUpdate );

			// View space to projection
			Math::cMatrix_transformation GetProjectionMatrix();

			// Data
			// The vertical field of view is the vertical angle of the view frustum (i.e. how wide the camera can see vertically)
			float m_verticalFieldOfView_inRadians;
			// aspectRatio = width / height
			float m_aspectRatio;

			float m_z_nearPlane;
			float m_z_farPlane;

			Runtime::cMovementComponent m_movementComponent;

			void SetPosition( const Math::sVector& i_position );

			void SetOrientation( const Math::cQuaternion& i_orientation );

			virtual void UpdateSimulationBasedOnInput() override;

			virtual void UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) override;

		private:
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Data

			// Initialization / Clean Up
			//--------------------------

			cResult Initialize();
			cResult CleanUp();

			cCamera();
			~cCamera();
		};
	}
}

#endif // EAE6320_RUNTIME_CCAMERA_H