// Includes
//=========

#include "../UserInput.h"

#include <Engine/Windows/Includes.h>
#include <Engine/Logging/Logging.h>

// Interface
//==========

bool eae6320::UserInput::IsKeyPressed( const uint_fast8_t i_keyCode )
{
	const auto keyState = GetAsyncKeyState( i_keyCode );
	constexpr short isKeyDownMask = ~1;

	bool pressed = (keyState & isKeyDownMask) != 0;
	if(pressed)
		eae6320::Logging::OutputMessage("Key code 0x%04x pressed", i_keyCode);
	return pressed;
}
