#pragma once

#include "Blam\Common\Common.h"

#define NONE -1

// floating point definitions
typedef float real;

typedef unsigned int datum;

#define DATUM_INDEX_NONE ((unsigned int)(NONE))

#define DATUM_INDEX_NEW(_absolute_index, _salt) (datum)((_absolute_index) | ((_salt) << 16))

#define DATUM_IS_NONE(_datum_index) ((_datum_index) == DATUM_INDEX_NONE)
#define DATUM_INDEX_TO_ABSOLUTE_INDEX(_datum_index) ((unsigned short)((_datum_index) & 0xFFFF))
#define DATUM_INDEX_TO_IDENTIFIER(_datum_index) ((unsigned short)(((_datum_index) >> 16) & 0xFFFF))
