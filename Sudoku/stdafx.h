#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <functional>

#define M 3
#define N 9

typedef int num_t;

template <typename T, int P>
using array1 = std::array<T, P>;

template <typename T, int Q, int P>
using array2 = std::array<std::array<T, P>, Q>;

template <typename T>
using arrN = array1<T, N>;

template <typename T>
using arrMM = array2<T, M, M>;

typedef arrN<num_t> arrN_t;

#ifdef _DEBUG
#define ASSERT(x) do { if (!(x)) throw; } while (false)
#else
#define ASSERT(x)
#endif
