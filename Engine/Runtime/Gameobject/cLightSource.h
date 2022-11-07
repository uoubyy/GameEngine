#ifndef EAE6320_RUNTIME_CLIGHT_SOURCE_H
#define EAE6320_RUNTIME_CLIGHT_SOURCE_H

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
		enum eLightSourceType : uint8_t
		{
			PointLight = 1 << 0,

			DirectionalLight = 1 << 1,
		};

		class cLightSource final : public iGameobject
		{
		public:
			// Initialization / Clean Up
			//--------------------------

			static cResult Load( cLightSource*& o_lightSource, eLightSourceType i_type );

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS( cLightSource );

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			void SetPosition( const Math::sVector& i_position ) { m_movementComponent .SetPosition( i_position ); }

			Math::sVector GetPosition() const { return m_movementComponent.GetPosition(); }

			void SetOrientation( const Math::cQuaternion& i_orientation );

			// World space to light source space
			Math::cMatrix_transformation GetViewMatrix( const float i_elapsedSecondCount_sinceLastSimulationUpdate );

			// Light source space to projection
			Math::cMatrix_transformation GetProjectionMatrix();

			// Data
			bool m_enableShadowMap = false;

			float m_z_nearPlane = 0.1f;
			float m_z_farPlane = 100.0f;

			struct
			{
				float left = 10.0f, right = 10.0f, bottom = 10.0f, top = 10.0f;
			} m_orthographicFrustum;

		private:
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Data
			Runtime::cMovementComponent m_movementComponent;

			eLightSourceType m_lightSourceType;

			// Initialization / Clean Up
			//--------------------------

			cResult Initialize();
			cResult CleanUp();

			cLightSource();
			cLightSource( eLightSourceType i_type );
			~cLightSource();
		};
	}
}

#endif // EAE6320_RUNTIME_CLIGHT_SOURCE_H