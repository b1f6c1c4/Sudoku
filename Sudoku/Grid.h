#pragma once
#include "stdafx.h"

#define NONE (-1)

struct Cover
{
    Cover();

    int Number;
    array1<bool, N> Filled;
    array1<int, N> Ref;

    bool Valid;
};

struct Ascending
{
    Ascending();

    int PosConstriant;
    int NegConstriant;

    array1<num_t, N> Val;

    bool Valid;
    bool Done;

    array2<int,N,N> Probs;
    std::vector<array1<num_t, N>> Solutions;

    void Init();
    bool Set(int ref, int value);
    void UpdateProbs();
};

class Grid sealed
{
public:
    Grid();

    std::istream &LoadAscendings(std::istream &is);

    int Get(int p) const;
    int Get(int x, int y) const;
    bool Set(int p, num_t value);
    bool Set(int x, int y, num_t value);

    bool Reduce();
    bool Reduce(int p);
    bool Reduce(int x, int y);

    bool Update();

    bool IsValid() const;

private:
    array2<num_t, N, N> m_Data;

    array1<Cover, N> m_Rows;
    array1<Cover, N> m_Cols;
    array2<Cover, M, M> m_Blks;

    array1<Ascending, N> m_RowA;
    array1<Ascending, N> m_ColA;

    bool m_Valid;

    bool Set(Cover &cover, int ref, int value);
    bool Set(Cover &cover, Ascending &asc, int ref, int value);

    bool Update(Cover &cover, Ascending &asc);
    void Fill(Cover &cover, Ascending &asc);
};

std::istream &operator>>(std::istream &is, Grid &grid);
std::ostream &operator<<(std::ostream &os, const Grid &grid);
