#include "cLightSource.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Math/Functions.h>

eae6320::cResult eae6320::Runtime::cLightSource::Load( cLightSource*& o_lightSource, eLightSourceType i_type )
{
	auto result = Results::Success;
	cLightSource* newLightSource = nullptr;

	cScopeGuard scopeGuard( [&result, &o_lightSource, &newLightSource]
		{
			if ( result )
			{
				EAE6320_ASSERT( newLightSource != nullptr );
				o_lightSource = newLightSource;
			}
			else
			{
				if ( newLightSource )
				{
					newLightSource->DecrementReferenceCount();
					newLightSource = nullptr;
				}
				o_lightSource = nullptr;
			}
		} );

	newLightSource = new ( std::nothrow ) cLightSource();
	if ( !newLightSource )
	{
		result = Results::OutOfMemory;
		EAE6320_ASSERTF( false, "Couldn't allocate memory for the Light Source Object" );
		return result;
	}

	return result;
}

void eae6320::Runtime::cLightSource::SetOrientation( const Math::cQuaternion& i_orientation )
{
	m_movementComponent.SetRotation( i_orientation );
}

eae6320::Math::cMatrix_transformation eae6320::Runtime::cLightSource::GetViewMatrix( const float i_elapsedSecondCount_sinceLastSimulationUpdate )
{
	return eae6320::Math::cMatrix_transformation::CreateWorldToCameraTransform( m_movementComponent.GetPredictOrientation( i_elapsedSecondCount_sinceLastSimulationUpdate ), m_movementComponent.GetPredictPosition( i_elapsedSecondCount_sinceLastSimulationUpdate ) );
}

eae6320::Math::cMatrix_transformation eae6320::Runtime::cLightSource::GetProjectionMatrix()
{
	if( m_lightSourceType == DirectionalLight )
		return eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective( eae6320::Math::ConvertDegreesToRadians( 45.0f ), 1.0f, m_z_nearPlane, m_z_farPlane );
	else
		return eae6320::Math::cMatrix_transformation::CreateCameraToProjectedTransform_ortho( m_orthographicFrustum.left, m_orthographicFrustum.right, m_orthographicFrustum.bottom, m_orthographicFrustum.top, m_z_nearPlane, m_z_farPlane );
}

eae6320::cResult eae6320::Runtime::cLightSource::Initialize()
{
	return Results::Success;
}

eae6320::cResult eae6320::Runtime::cLightSource::CleanUp()
{
	return Results::Success;
}

eae6320::Runtime::cLightSource::cLightSource() : m_lightSourceType( eLightSourceType::DirectionalLight )
{

}

eae6320::Runtime::cLightSource::cLightSource( eLightSourceType i_type ) : m_lightSourceType( i_type )
{

}

eae6320::Runtime::cLightSource::~cLightSource()
{

}

