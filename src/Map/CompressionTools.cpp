//
// WulfGame/Map/CompressionTools.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "Map/CompressionTools.h"

#include <stdexcept>

using Wulf::word;
using Wulf::byte;
using namespace Wulf;

word LEconcat(byte low, byte high);

static const byte NEARTAG = 0xA7;
static const byte  FARTAG = 0xA8;

typedef std::vector<byte>::const_iterator byte_itr;
typedef std::vector<word>::const_iterator word_itr;

std::vector<word> CompressionTools::CarmackExpand(const std::vector<byte>& rawdata)
{
	/*
	This is a series of two byte numbers.
	The first two byte number is the number of uncompressed bytes we will end up with.
	The rest are compressed dat awith occasional missfits:
	Every so often, a three byte number will turn up in the form
	xx A7 yy.
	If xx is 0, the number is the two byte number yyA7.
	Otherwise, this means
		"Repeat 0xXX two byte numbers from the uncompressed data, starting 0xYY numbers ago."
	Sometimes you will also get four byte numbers in the form
	xx A8 yy yy.
	If xx is 0, it is only a three byte number, the same as 00A7 but now 00A8.
	Otherwise, this is
		"repeat 0xXX numbers from the uncompressed data, starting 0xYYYY numbers from the start."
	Easy peasy
	*/
	if (rawdata.size() < 2)
		throw std::runtime_error("Not enough raw data!");

	std::vector<word> output;

	byte_itr itr = rawdata.begin();
	byte_itr end = rawdata.end();

	word length = LEconcat(*itr++, *itr++);
	if (length == 0)
		return output;
	output.reserve(length / 2);

	byte lowbyte  = 0;
	byte highbyte = 0;
	bool getlower = true;
	for (; itr < end; ++itr) {
		if (getlower) {
			// low byte not got
			lowbyte = *itr;
			getlower = false;
		} else {
			getlower = true;
			highbyte = *itr;
			if (highbyte == NEARTAG) {
				byte ptr = *++itr;
				if (lowbyte == 0) {
					output.push_back(LEconcat(ptr, highbyte));
					continue;
				}
				word_itr outdata = output.end() - ptr;
				while (lowbyte-- > 0)
					output.push_back(*outdata++);
			} else if (highbyte == FARTAG) {
				byte lptr = *++itr;
				if (lowbyte == 0) {
					output.push_back(LEconcat(lptr, highbyte));
					continue;
				}
				word ptr = LEconcat(lptr, *++itr);
				word_itr outdata = output.begin() + ptr;
				while (lowbyte-- > 0)
					output.push_back(*outdata++);
			} else {
				output.push_back(LEconcat(lowbyte, highbyte));
			}
		}
	}
	return output;
}

std::vector<word> CompressionTools::RLEWExpand(const std::vector<word>& data, const word tag)
{
	/*
	 This is RLEW. It's very simple
	 1) Read a word
	 2) Is it the tag?
		NO:
			Add it to the output and restart
		YES:
			Read the next two words.
			Add word #2 to the output word #1 times and restart
	 Simple eh?
	*/
	if (data.size() < 2)
		throw std::runtime_error("Not enough data!");
	std::vector<word> output;

	word_itr itr = data.begin();
	word_itr end = data.end();

	word size = *itr++;
	if (size == 0)
		return std::vector<word>();
	output.reserve(size);

	for (; itr < end; ++itr) {
		word cword = *itr;
		if (cword == tag) {
			word reps = *++itr;
			word data = *++itr;
			while (reps-- > 0)
				output.push_back(data);
		} else {
			output.push_back(cword);
		}
	}
	return output;
}

// Little endian concatination.
inline word LEconcat(byte low, byte high)
{
	return low | high << 8;
}
