// Includes
//=========

#include "cRenderTarget.h"
#include "sContext.h"

#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <new>
#include <vector>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#if defined( EAE6320_PLATFORM_GL )
		#include <gl/GL.h>
		#include <gl/GLU.h>	// The "U" is for "utility functions"
	#endif
#endif

#if defined( EAE6320_PLATFORM_GL )
	// Modern OpenGL requires extensions
	#include <External/OpenGlExtensions/OpenGlExtensions.h>
#endif

// Helper Class Declaration
//=========================

// Interface
//==========

// Initialize / Clean Up
//----------------------"

eae6320::cResult eae6320::Graphics::cRenderTarget::Load( cRenderTarget*& o_renderTarget )
{
	auto result = Results::Success;

	cRenderTarget* newRenderTarget = nullptr;
	cScopeGuard scopeGuard( [&o_renderTarget, &result, &newRenderTarget]
		{
			if ( result )
			{
				EAE6320_ASSERT( newRenderTarget != nullptr );
				o_renderTarget = newRenderTarget;
			}
			else
			{
				if ( newRenderTarget )
				{
					newRenderTarget->DecrementReferenceCount();
					newRenderTarget = nullptr;
				}
				o_renderTarget = newRenderTarget;
			}
		});

	// Allocate a render target
	{
		newRenderTarget = new (std::nothrow) cRenderTarget();
		if ( !newRenderTarget )
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF( false, "Couldn't allocate a render target" );
			Logging::OutputError( "Failed to allocate memory for the render target" );
			return result;
		}
	}
	// Initialize the platform-specific graphics API render target object
	if ( !( result = newRenderTarget->Initialize() ) )
	{
		EAE6320_ASSERTF(false, "Initialization of new render target failed");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cRenderTarget::Initialize()
{
	return eae6320::Results::Success;
}

eae6320::cResult eae6320::Graphics::cRenderTarget::CleanUp()
{
#if defined( EAE6320_PLATFORM_D3D )
	if ( m_renderTargetView )
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}
	if ( m_depthStencilView )
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}
#endif
	return eae6320::Results::Success;
}

eae6320::Graphics::cRenderTarget::cRenderTarget()
{

}

eae6320::Graphics::cRenderTarget::~cRenderTarget()
{
	const auto result = CleanUp();
	EAE6320_ASSERT( result );
}

eae6320::cResult eae6320::Graphics::cRenderTarget::InitializeViews( const Graphics::sInitializationParameters& i_initializationParameters )
{
	auto result = eae6320::Results::Success;

#if defined( EAE6320_PLATFORM_D3D )
	const unsigned int i_resolutionWidth = i_initializationParameters.resolutionWidth;
	const unsigned int i_resolutionHeight = i_initializationParameters.resolutionHeight;

	ID3D11Texture2D* backBuffer = nullptr;
	ID3D11Texture2D* depthBuffer = nullptr;
	eae6320::cScopeGuard scopeGuard( [&backBuffer, &depthBuffer]
		{
			// Regardless of success or failure the two texture resources should be released
			// (if the function is successful the views will hold internal references to the resources)
			if ( backBuffer )
			{
				backBuffer->Release();
				backBuffer = nullptr;
			}
			if ( depthBuffer )
			{
				depthBuffer->Release();
				depthBuffer = nullptr;
			}
		});

	auto& g_context = eae6320::Graphics::sContext::g_context;
	auto* const direct3dDevice = g_context.direct3dDevice;
	EAE6320_ASSERT( direct3dDevice );
	auto* const direct3dImmediateContext = g_context.direct3dImmediateContext;
	EAE6320_ASSERT( direct3dImmediateContext );

	// Create a "render target view" of the back buffer
	// (the back buffer was already created by the call to D3D11CreateDeviceAndSwapChain(),
	// but a "view" of it is required to use as a "render target",
	// meaning a texture that the GPU can render to)
	{
		// Get the back buffer from the swap chain
		{
			constexpr unsigned int bufferIndex = 0;	// This must be 0 since the swap chain is discarded
			const auto d3dResult = g_context.swapChain->GetBuffer( bufferIndex, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer ) );
			if ( FAILED( d3dResult ) )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, "Couldn't get the back buffer from the swap chain (HRESULT %#010x)", d3dResult );
				eae6320::Logging::OutputError( "Direct3D failed to get the back buffer from the swap chain (HRESULT %#010x)", d3dResult );
				return result;
			}
		}
		// Create the view
		{
			constexpr D3D11_RENDER_TARGET_VIEW_DESC* const accessAllSubResources = nullptr;
			const auto d3dResult = direct3dDevice->CreateRenderTargetView( backBuffer, accessAllSubResources, &m_renderTargetView );
			if ( FAILED( d3dResult ) )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, "Couldn't create render target view (HRESULT %#010x)", d3dResult );
				eae6320::Logging::OutputError( "Direct3D failed to create the render target view (HRESULT %#010x)", d3dResult );
				return result;
			}
		}
	}
	// Create a depth/stencil buffer and a view of it
	{
		// Unlike the back buffer no depth/stencil buffer exists until and unless it is explicitly created
		{
			const auto textureDescription = [i_resolutionWidth, i_resolutionHeight]
			{
				auto textureDescription = []() constexpr
				{
					D3D11_TEXTURE2D_DESC textureDescription{};

					textureDescription.MipLevels = 1;	// A depth buffer has no MIP maps
					textureDescription.ArraySize = 1;
					textureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 24 bits for depth and 8 bits for stencil
					{
						DXGI_SAMPLE_DESC& sampleDescription = textureDescription.SampleDesc;

						sampleDescription.Count = 1;	// No multisampling
						sampleDescription.Quality = 0;	// Doesn't matter when Count is 1
					}
					textureDescription.Usage = D3D11_USAGE_DEFAULT;	// Allows the GPU to write to it
					textureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
					textureDescription.CPUAccessFlags = 0;	// The CPU doesn't need access
					textureDescription.MiscFlags = 0;

					return textureDescription;
				}();

				textureDescription.Width = i_resolutionWidth;
				textureDescription.Height = i_resolutionHeight;

				return textureDescription;
			}();
			// The GPU renders to the depth/stencil buffer and so there is no initial data
			// (like there would be with a traditional texture loaded from disk)
			constexpr D3D11_SUBRESOURCE_DATA* const noInitialData = nullptr;
			const auto d3dResult = direct3dDevice->CreateTexture2D( &textureDescription, noInitialData, &depthBuffer );
			if ( FAILED( d3dResult ) )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, "Couldn't create depth buffer (HRESULT %#010x)", d3dResult );
				eae6320::Logging::OutputError( "Direct3D failed to create the depth buffer resource (HRESULT %#010x)", d3dResult );
				return result;
			}
		}
		// Create the view
		{
			constexpr D3D11_DEPTH_STENCIL_VIEW_DESC* const noSubResources = nullptr;
			const auto d3dResult = direct3dDevice->CreateDepthStencilView( depthBuffer, noSubResources, &m_depthStencilView );
			if ( FAILED( d3dResult ) )
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF( false, "Couldn't create depth stencil view (HRESULT %#010x)", d3dResult );
				eae6320::Logging::OutputError( "Direct3D failed to create the depth stencil view (HRESULT %#010x)", d3dResult );
				return result;
			}
		}
	}

	// Bind the views
	{
		constexpr unsigned int renderTargetCount = 1;
		direct3dImmediateContext->OMSetRenderTargets( renderTargetCount, &m_renderTargetView, m_depthStencilView );
	}
	// Specify that the entire render target should be visible
	{
		const auto viewPort = [i_resolutionWidth, i_resolutionHeight]
		{
			auto viewPort = []() constexpr
			{
				D3D11_VIEWPORT viewPort{};

				viewPort.TopLeftX = viewPort.TopLeftY = 0.0f;
				viewPort.MinDepth = 0.0f;
				viewPort.MaxDepth = 1.0f;

				return viewPort;
			}();
			viewPort.Width = static_cast<float>( i_resolutionWidth );
			viewPort.Height = static_cast<float>( i_resolutionHeight );

			return viewPort;
		}();
		constexpr unsigned int viewPortCount = 1;
		direct3dImmediateContext->RSSetViewports( viewPortCount, &viewPort );
	}
#endif
	return result;
}

void eae6320::Graphics::cRenderTarget::ClearBackBuffer( const float i_clearColor[4] )
{
	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
#if defined( EAE6320_PLATFORM_D3D )
	auto* const direct3dImmediateContext = eae6320::Graphics::sContext::g_context.direct3dImmediateContext;
	EAE6320_ASSERT( direct3dImmediateContext );
	{
		EAE6320_ASSERT( m_renderTargetView );

		direct3dImmediateContext->ClearRenderTargetView( m_renderTargetView, i_clearColor );
	}
	// In addition to the color buffer there is also a hidden image called the "depth buffer"
	// which is used to make it less important which order draw calls are made.
	// It must also be "cleared" every frame just like the visible color buffer.
	{
		EAE6320_ASSERT( m_depthStencilView );

		constexpr float clearToFarDepth = 1.0f;
		constexpr uint8_t stencilValue = 0;	// Arbitrary if stencil isn't used
		direct3dImmediateContext->ClearDepthStencilView( m_depthStencilView, D3D11_CLEAR_DEPTH, clearToFarDepth, stencilValue );
	}
#elif defined( EAE6320_PLATFORM_GL )
	{
		{
			glClearColor( i_clearColor[0], i_clearColor[1], i_clearColor[2], i_clearColor[3] );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
		{
			constexpr GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
			glClear( clearColor );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
	}
	// In addition to the color buffer there is also a hidden image called the "depth buffer"
	// which is used to make it less important which order draw calls are made.
	// It must also be "cleared" every frame just like the visible color buffer.
	{
		{
			glDepthMask( GL_TRUE );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
			constexpr GLclampd clearToFarDepth = 1.0;
			glClearDepth( clearToFarDepth );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
		{
			constexpr GLbitfield clearDepth = GL_DEPTH_BUFFER_BIT;
			glClear( clearDepth );
			EAE6320_ASSERT( glGetError() == GL_NO_ERROR );
		}
	}
#endif
}

void eae6320::Graphics::cRenderTarget::Show()
{
	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it the contents of the back buffer must be "presented"
	// (or "swapped" with the "front buffer", which is the image that is actually being displayed)
#if defined( EAE6320_PLATFORM_D3D )
	{
		auto* const swapChain = sContext::g_context.swapChain;
		EAE6320_ASSERT( swapChain );
		constexpr unsigned int swapImmediately = 0;
		constexpr unsigned int presentNextFrame = 0;
		const auto result = swapChain->Present( swapImmediately, presentNextFrame );
		EAE6320_ASSERT( SUCCEEDED( result ) );
	}
#elif defined( EAE6320_PLATFORM_GL )
	{
		const auto deviceContext = sContext::g_context.deviceContext;
		EAE6320_ASSERT( deviceContext != NULL );
		const auto glResult = SwapBuffers( deviceContext );
		EAE6320_ASSERT( glResult != FALSE );
	}
#endif
}
