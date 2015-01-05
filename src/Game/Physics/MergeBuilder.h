#pragma once
#include <array>
#include <vector>
#include <utility>
#include "WulfConstants.h"
#include "Map/Map.h"
#include "Game/Physics/TileData.h"

namespace Wulf {
	namespace Physics {
		class MergeNode {
		public:
			coords topLeft;
			TileType type;

			coord width, height;
			bool done;

			MergeNode(Map::Node const& node);
			// Checks if other is stuitable for absorbsion. Returns false for nullptr
			bool compatible(MergeNode const* other) const;
			// This returns the tiledata for the merged mass.
			// It is safe to call multiple times as it only returns one TileData.
			TileData* toTile();
		private:
			// Deletion is handled elsewhere
			TileData* mTileData;
		};
		class MergeBuilder {
		public:
			MergeBuilder(Map::Map const& map);
			~MergeBuilder();
			std::vector<std::pair<coords, TileData*>> getTileData() const;

		private:
			// Insert all viable tiles into a static 64x64 array
			// Leaves unviable (ie wall cavities) as nullptr;
			void loadMap(Map::Map const& map);
			// Go through everything and start merging.
			// This replaces merged tiles with multiple pointers to the same tiledata.
			void performMerges();

			int verticalMerge(int x, int y);
			void horizontalMerge(int x, int y);

			static const size_t xsize = Map::Map::width;
			static const size_t ysize = Map::Map::height;
			std::array<std::array<MergeNode*, xsize>, ysize> nodes;
		};
	}
}
