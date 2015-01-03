//
// WulfGame/main.cpp
// Copyright (C) 2012 Lexi Robinson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <iostream>
#include "Wrappers/System.h"
#include "Wrappers/PhysFS.h"
#include "Game/Game.h"

// PhysFS
void start_physfs(char *arg0);
void close_physfs();
bool check_content();

/*
 * Note: This program will not run on anything but a little endian system.
 */
int main(int argc, char **argv)
{
	// Virtual file system
	try {
		start_physfs(argv[0]);
	} catch (PhysFS::Exception& e) {
		System::ErrorBox("Could not start PhysFS:", e.what());
		return -1;
	}

	// Checkity check for contentity tent
	if (!check_content()) {
		System::ErrorBox("Content error:", "You are missing some required content.");
		return -1;
	}

	try {
		Wulf::Game game(Wulf::Difficulty::MEDI);
		game.LoadMap(0);
		while (game.IsRunning())
			game.Run();
	} catch (std::exception& e) {
		System::ErrorBox("Uncaught exception: ", e.what());
		throw;
	}
	return 0;
}

void start_physfs(char *arg0)
{
	PhysFS::init(arg0);
	std::atexit(close_physfs);
	std::string path = PhysFS::getBaseDir();
	PhysFS::mount(path, "", false);
	if (!PhysFS::exists("wolf.pak")) {
		// Sigh, I don't know why it is, but VS2012 seems to set the base dir one lower than
		//  it should be when debugging. Try to deal with this.
		if (PhysFS::isDirectory("game") && PhysFS::exists("game/wolf.pak")) {
			PhysFS::removeFromSearchPath(path);
			path += "/game";
			PhysFS::mount(path, "", false);
		} else {
			throw PhysFS::Exception("wolf.pak is not in the base direcory! (" + path + ")");
		}
	}
	PhysFS::mount(path + "/wolf.pak", "");

	// Debulartes
#ifdef PHYSFS_DEBUG
	auto dir = PhysFS::getDirListing("/");
	for (auto i = dir.begin(), end = dir.end(); i < end; ++i)
		std::cout << *i << std::endl;
#endif
}

void close_physfs()
{
	try {
		PhysFS::deinit();
	} catch (PhysFS::Exception&) {}
}

bool check_content()
{
	return PhysFS::isDirectory("lsfx")
		&& PhysFS::isDirectory("maps")
		&& PhysFS::isDirectory("music")
		&& PhysFS::isDirectory("pics")
		&& PhysFS::isDirectory("script")
		&& PhysFS::isDirectory("sfx")
		&& PhysFS::isDirectory("shaders")
		&& PhysFS::isDirectory("sprites")
		&& PhysFS::isDirectory("walls");
}
