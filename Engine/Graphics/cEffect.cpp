// Includes
//=========

#include "cEffect.h"

#include "cShader.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cMutex.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <new>
#include <vector>

// Helper Class Declaration
//=========================

// Interface
//==========

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::cEffect::Load( const std::string& i_vertexShaderPath, const std::string& i_fragmentShaderPath, cEffect*& o_effect )
{
	auto result = Results::Success;

	cEffect* newEffect = nullptr;
	cScopeGuard scopeGuard( [&result, &o_effect, &newEffect] 
		{
			if ( result )
			{
				EAE6320_ASSERT( newEffect != nullptr );
				o_effect = newEffect;
			}
			else
			{
				if ( newEffect )
				{
					newEffect->DecrementReferenceCount();
					newEffect = nullptr;
				}
				o_effect = nullptr;
			}
		} );

	// Allocate a new effect
	{
		newEffect = new (std::nothrow) cEffect();
		if ( !newEffect )
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF( false, "Couldn't allocate memory for the effect" );
			Logging::OutputError( "Failed to allocate memory for the effect" );
			return result;
		}
	}

	// Initialize the graphics API effect object
	if ( !( result = newEffect->Initialize( i_vertexShaderPath, i_fragmentShaderPath ) ) )
	{
		EAE6320_ASSERTF( false, "Initialization of new effect failed" );
		return result;
	}

	return result;
}

// Implementation
//===============

// Initialize / Clean Up
//----------------------

eae6320::Graphics::cEffect::cEffect()
{

}

eae6320::Graphics::cEffect::~cEffect()
{
	const auto result = CleanUp();
	EAE6320_ASSERT( result );
}

eae6320::cResult eae6320::Graphics::cEffect::Initialize( const std::string& i_vertexShaderPath, const std::string& i_fragmentShaderPath )
{
	auto result = eae6320::Results::Success;

	if ( !( result = eae6320::Graphics::cShader::Load( i_vertexShaderPath,
		m_vertexShader, eae6320::Graphics::eShaderType::Vertex ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize shading data without vertex shader" );
		return result;
	}
	if ( !( result = eae6320::Graphics::cShader::Load( i_fragmentShaderPath,
		m_fragmentShader, eae6320::Graphics::eShaderType::Fragment ) ) )
	{
		EAE6320_ASSERTF( false, "Can't initialize shading data without fragment shader" );
		return result;
	}
	{
		constexpr auto renderStateBits = []
		{
			uint8_t renderStateBits = 0;

			eae6320::Graphics::RenderStates::DisableAlphaTransparency( renderStateBits );
			eae6320::Graphics::RenderStates::EnableDepthTesting( renderStateBits );
			eae6320::Graphics::RenderStates::EnableDepthWriting( renderStateBits );
			eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides( renderStateBits );

			return renderStateBits;
		}();
		if ( !( result = m_renderState.Initialize( renderStateBits ) ) )
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return result;
		}
	}

#if defined( EAE6320_PLATFORM_GL )
	result = InitializeShadingProgram();
#endif

	return result;
}

eae6320::cResult eae6320::Graphics::cEffect::CleanUp()
{
	auto result = eae6320::Results::Success;

#if defined( EAE6320_PLATFORM_GL )
	result = CleanUpShadingProgram();
#endif

	if ( m_vertexShader )
	{
		m_vertexShader->DecrementReferenceCount();
		m_vertexShader = nullptr;
	}

	if ( m_fragmentShader )
	{
		m_fragmentShader->DecrementReferenceCount();
		m_fragmentShader = nullptr;
	}

	return result;
}