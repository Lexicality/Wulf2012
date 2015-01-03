#include "Doors.h"

using namespace Wulf;
using namespace Wulf::Doors;

DoorInfo::DoorInfo(coord x, coord y, word data)
	: x(x), y(y), tex(BaseTexture), dir(Horizontal), status(OpeningStatus::Closed), openPercentReal(0), locked(false)
{
	switch (data) {
	case VertNormal:
		tex += 1;
		dir = Vertical;
	case HoriNormal:
		tex += DoorOffset;
		break;
	case VertGold:
	case VertSilver:
		tex += 1;
		dir = Vertical;
	case HoriGold:
	case HoriSilver:
		tex += LockOffset;
		locked = true;
		break;
	case VertEle:
		tex += 1;
		dir = Vertical;
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

void DoorInfo::Open()
{
	locked = false;
	status = OpeningStatus::Opening;
	closeCountdown = CloseAfter;
}

void DoorInfo::Think(double dtime)
{
	if (status == OpeningStatus::Closed)
		return;
	if (status == OpeningStatus::Open) {
		closeCountdown -= dtime;
		if (closeCountdown > 0)
			return;
		status = OpeningStatus::Closing;
	}
	double openAmt = dtime * OpenSpeed;
	if (status == OpeningStatus::Opening) {
		openPercentReal += openAmt;
		if (openPercentReal >= 1) {
			status = OpeningStatus::Open;
			openPercentReal = 1;
		}
	} else {
		openPercentReal -= openAmt;
		if (openPercentReal <= 0) {
			status = OpeningStatus::Closed;
			openPercentReal = 0;
			// DOCTOR DEBUG
			Open();
		}
	}
}

byte DoorInfo::openPercent() const
{
	return static_cast<byte>(openPercentReal * 100 + 0.5);
}

bool DoorInfo::isSolid() const
{
	if (status == OpeningStatus::Open)
		return false;
	if (status == OpeningStatus::Closed)
		return true;
	return openPercentReal < 0.8;
}
