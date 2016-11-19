#pragma once
#include "stdafx.h"
#include "Grid.h"

class Searcher
{
public:
    static std::shared_ptr<Grid> Search(std::shared_ptr<Grid> grid, int depth = 0);
    static std::shared_ptr<Grid> WSearch(std::shared_ptr<Grid> grid, int depth = 0);
};
