
#ifndef EAE6320_GRAPHICS_CRENDER_TARGET_H
#define EAE6320_GRAPHICS_CRENDER_TARGET_H

// Includes
//=========

#include <Engine/Assets/ReferenceCountedAssets.h>

#include <Engine/Results/Results.h>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#if defined( EAE6320_PLATFORM_D3D )
		#include <d3d11.h>
		#include <dxgi.h>
	#endif
#endif

namespace eae6320
{
	namespace Graphics
	{
		struct sInitializationParameters;
	}
}

namespace eae6320
{
	namespace Graphics
	{
		class cRenderTarget
		{
			// Interface
			//==========

		public:

			// Initialization / Clean Up
			//--------------------------

			static cResult Load( cRenderTarget*& o_renderTarget );

			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS( cRenderTarget );

			// Reference Counting
			//-------------------

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();

			// Data
			//=====

		protected:

		private:
#if defined( EAE6320_PLATFORM_D3D )
			ID3D11RenderTargetView* m_renderTargetView = nullptr;

			ID3D11DepthStencilView* m_depthStencilView = nullptr;
#endif

			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			// Initialization / Clean Up
			//--------------------------

			cResult Initialize();
			cResult CleanUp();

			cRenderTarget();
			~cRenderTarget();

		public:
			cResult InitializeViews( const Graphics::sInitializationParameters& o_initializationParameters ) ;

			void ClearBackBuffer( const float i_clearColor[4] );

			void Show();
		};
	}
}

#endif