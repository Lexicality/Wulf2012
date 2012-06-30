//
// WulfGame/Map/CompressionTools.h
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#pragma once
#include <vector>
#include "WulfConstants.h"

namespace Wulf {
	namespace CompressionTools {
		std::vector<word> CarmackExpand(const std::vector<byte>& rawdata);
		std::vector<word> RLEWExpand(const std::vector<word>& data, const word tag);
	}
}
