#pragma once
#include "stdafx.h"
#include "Grid.h"

class Searcher
{
public:
    static std::shared_ptr<Grid> Search(std::shared_ptr<Grid> grid);
};
