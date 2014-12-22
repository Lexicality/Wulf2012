#include "Doors.h"

using namespace Wulf;
using namespace Wulf::Doors;


DoorInfo::DoorInfo(coord x, coord y, word data)
: x(x), y(y), tex(BaseTexture), dir(Horizontal), openPercent(0), locked(false)
{
	switch(data) {
	case VertNormal:
		tex += 1;
		dir  = Vertical;
	case HoriNormal:
		tex += DoorOffset;
		break;
	case VertGold:
	case VertSilver:
		tex += 1;
		dir  = Vertical;
	case HoriGold:
	case HoriSilver:
		tex += LockOffset;
		locked = true;
		break;
	case VertEle:
		tex += 1;
		dir  = Vertical;
	case HoriEle:
		tex += ElevOffset;
		break;
	}
	if (data == VertGold || data == HoriGold)
		key = GoldKey;
	else if (data == VertSilver || data == HoriNormal)
		key = SilverKey;
	// else key = random garbage woo fake tristate
}
