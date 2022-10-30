#ifndef EAE6320_RUNTIME_IGAMEOBJECT_H
#define EAE6320_RUNTIME_IGAMEOBJECT_H

// Includes
//=========

#include <Engine/Results/Results.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Runtime
	{
		class iGameobject
		{
			// Interface
			//==========
		public:
			// Initialize / Clean Up
			//----------------------

			virtual void UpdateBasedOnInput() {}
			virtual void UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) {}

			virtual void UpdateSimulationBasedOnInput() {}
			virtual void UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) {}

			iGameobject();

			virtual ~iGameobject();

			virtual cResult CleanUp() = 0;
		};
	}
}

#endif // EAE6320_RUNTIME_IGAMEOBJECT_H