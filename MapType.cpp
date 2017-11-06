#include "MapType.h"

MapType::MapType(tinyobj::index_t index0)
{
	this->index0 = index0;

}

MapType::~MapType()
{

}

bool MapType::operator==(const MapType & b) const
{
	if (this->index0 == b.index0)
	{
		return true;
	}
	return false;
}
