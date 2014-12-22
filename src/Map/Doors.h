#pragma once
#include "WulfConstants.h"

namespace Wulf {
	namespace Doors {
		// Thextures
		static const word BaseTexture = 98;
		static const word WallTexture = BaseTexture + 2;
		static const word DoorOffset  = 0;
		static const word ElevOffset  = 4;
		static const word LockOffset  = 6;
		// blockdata values
		// Normal
		static const word VertNormal = 0x5A;
		static const word HoriNormal = 0x5B;
		// Gold lock
		static const word VertGold   = 0x5C;
		static const word HoriGold   = 0x5D;
		// Silver lock
		static const word VertSilver = 0x5E;
		static const word HoriSilver = 0x5F;
		// Elevator
		static const word VertEle    = 0x64;
		static const word HoriEle    = 0x65;
		// Keys
		typedef bool KeyType;
		static const KeyType GoldKey   = true;
		static const KeyType SilverKey = false;
		// Directions
		typedef bool Direction;
		static const Direction Vertical   = true;
		static const Direction Horizontal = false;
		// Opening/shutting
		enum class OpeningStatus {
			Opening,
			Open,
			Closing,
			Closed
		};
		static const double OpenSpeed = 1.0 / 3; // 3 second opening time
		static const double CloseAfter = 10; // Automatically close door after 10s


		class DoorInfo {
		public:
			word doorID;

			coord x, y;
			Material tex;
			Direction dir;

			byte openPercent() const;
			bool isSolid() const;

			bool locked;
			KeyType key; // Do not check this value if locked == false.

			DoorInfo(coord x, coord y, word data);
			void Open();
			void Think(double dtime);

		private:
			OpeningStatus status;
			double closeCountdown;
			double openPercentReal;
		};

		static inline bool IsDoor(const word data) {
			return (data >= 0x5a && data <= 0x5F)
				 || data == 0x64 || data == 0x65;
		}

	}
}
