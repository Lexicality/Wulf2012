//
// WulfGame/Game/InputManager.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include "WulfConstants.h"

namespace Wulf {
	namespace Input {
		struct Data {
			bool Forwards;
			bool Backwards;
			bool Left;
			bool Right;
			bool Shoot;
			bool Use;
			bool InventoryPrev;
			bool InventoryNext;
			bool Slot1;
			bool Slot2;
			bool Slot3;
			bool Slot4;
			bool Exit;
#ifdef FREE_VIEW
			float DeltaViewY;
			float DeltaViewX;
#else
			float DeltaView;
#endif
		};
		class Manager {
		public:
			Manager();
			const Data GetInput();
		private:
			int lastx, lasty;
			bool lostFocus;
		};
	}
}
