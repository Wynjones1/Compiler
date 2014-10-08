#pragma once

#define X(name) ERROR_TYPE ## name
enum ERROR_TYPE
{
	ERROR_NONE = 0,
}
#undef X
