#ifndef _MAPTYPE_H_
#define _MAPTYPE_H_

#include "glm/glm.hpp"
#include "external_code/tiny_obj_loader.h"

class MapType
{
public:
	tinyobj::index_t index0;

public:
	MapType(tinyobj::index_t index0);
	~MapType();
	bool operator==(const MapType &b) const;
};

namespace std {

	template <> struct hash<MapType>
	{
		std::size_t operator()(const MapType& k) const
		{
			using std::size_t;
			using std::hash;

			return hash<tinyobj::index_t>()(k.index0);
		}
	};
}

#endif 