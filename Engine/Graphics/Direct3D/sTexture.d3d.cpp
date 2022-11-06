// Includes
//=========

#include "../sTexture.h"
#include "Includes.h"

eae6320::cResult eae6320::Graphics::sTexture::CleanUp()
{
	{
		auto result = Results::Success;

		if ( m_texture )
		{
			m_texture->Release();
			m_texture = nullptr;
		}
		if ( m_shaderResourceView )
		{
			m_shaderResourceView->Release();
			m_shaderResourceView = nullptr;
		}
		if ( m_samplerState )
		{
			m_samplerState->Release();
			m_samplerState = nullptr;
		}
		return result;
	}
}
