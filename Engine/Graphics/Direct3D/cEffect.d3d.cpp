// Includes
//=========

#include "../cEffect.h"

#include "Includes.h"

#include "../sContext.h"
#include "../cShader.h"

#include <Engine/Asserts/Asserts.h>

// Implementation
//===============

void eae6320::Graphics::cEffect::Bind()
{
	auto* const direct3dImmediateContext = sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT( direct3dImmediateContext );

	{
		constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
		constexpr unsigned int interfaceCount = 0;
		// Vertex shader
		{
			EAE6320_ASSERT( ( m_vertexShader != nullptr ) && ( m_vertexShader->m_shaderObject.vertex != nullptr ) );
			direct3dImmediateContext->VSSetShader( m_vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount );
		}
		// Fragment shader
		{
			EAE6320_ASSERT( ( m_fragmentShader != nullptr ) && ( m_fragmentShader->m_shaderObject.vertex != nullptr ) );
			direct3dImmediateContext->PSSetShader( m_fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount );
		}
	}
	// Render state
	{
		m_renderState.Bind();
	}
}