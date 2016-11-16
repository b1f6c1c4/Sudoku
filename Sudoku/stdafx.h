#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>

#define M 3
#define N (M*M) // size of grid

typedef int num_t;

#ifdef _DEBUG
#define ASSERT(x) do { if (!(x)) throw; } while (false)
#else
#define ASSERT(x)
#endif
