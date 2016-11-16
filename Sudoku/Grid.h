#pragma once
#include "stdafx.h"

#define NONE (-1)

struct Cover
{
    Cover();

    int Number;
    bool Filled[N];
    int Ref[N];

    bool Valid;
};

class Grid sealed
{
public:
    Grid();

    int Get(int p) const;
    int Get(int x, int y) const;
    bool Set(int p, num_t value);
    bool Set(int x, int y, num_t value);

    bool Reduce();
    bool Reduce(int p);
    bool Reduce(int x, int y);

    bool IsValid() const;
private:
    num_t m_Data[N][N];

    Cover m_Rows[N];
    Cover m_Cols[N];
    Cover m_Blks[M][M];

    bool m_Valid;

    bool Set(Cover &cover, int ref, int value);

};

std::istream &operator>>(std::istream &is, Grid &grid);
std::ostream &operator<<(std::ostream &os, const Grid &grid);
