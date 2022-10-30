/*
	This file provides an extremely basic interface for querying user input from the keyboard
*/

#ifndef EAE6320_USERINPUT_H
#define EAE6320_USERINPUT_H

// Includes
//=========

#include <cstdint>

// Interface
//==========

namespace eae6320
{
	namespace UserInput
	{
		// Returns if the specified key is currently pressed

		// For standard letter or number keys, the representative ascii char can be used:
		// IsKeyPressed( 'A' ) or IsKeyPressed( '6' )

		// For special keys use one of the KeyCodes enumerations below

		bool IsKeyPressed( const uint_fast8_t i_keyCode );

		namespace KeyCodes
		{
			// These values are what the Windows-specific function expects, for simplicity
			enum eKeyCodes
			{
				Left = 0x25,
				Up = 0x26,
				Right = 0x27,
				Down = 0x28,

				Space = 0x20,

				Escape = 0x1b,

				Shift = 0x10,
				Control = 0x11,
				Alt = 0x12,

				Tab = 0x09,
				CapsLock = 0x14,

				BackSpace = 0x08,
				Enter = 0x0d,
				Delete = 0x2e,

				PageUp = 0x21,
				PageDown = 0x22,
				End = 0x23,
				Home = 0x24,

				Num_0 = 0x30,
				Num_1 = 0x31,
				Num_2 = 0x32,
				Num_3 = 0x33,
				Num_4 = 0x34,
				Num_5 = 0x35,
				Num_6 = 0x36,
				Num_7 = 0x37,
				Num_8 = 0x38,
				Num_9 = 0x39,

				A = 0x41,
				B = 0x42,
				C = 0x43,
				D = 0x44,
				E = 0x45,
				F = 0x46,
				G = 0x47,
				H = 0x48,
				I = 0x49,
				J = 0x4A,
				K = 0x4B,
				L = 0x4C,
				M = 0x4D,
				N = 0x4E,
				O = 0x4F,
				P = 0x50,
				Q = 0x51,
				R = 0x52,
				S = 0x53,
				T = 0x54,
				U = 0x55,
				V = 0x56,
				W = 0x57,
				X = 0x58,
				Y = 0x59,
				Z = 0x5A,


				F1 = 0x70,
				F2 = 0x71,
				F3 = 0x72,
				F4 = 0x73,
				F5 = 0x74,
				F6 = 0x75,
				F7 = 0x76,
				F8 = 0x77,
				F9 = 0x78,
				F10 = 0x79,
				F11 = 0x7a,
				F12 = 0x7b,
			};
		}
	}
}

#endif	// EAE6320_USERINPUT_H
