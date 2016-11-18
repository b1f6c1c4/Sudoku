#pragma once
#include "stdafx.h"

#define NONE (-1)

struct Cover
{
    Cover();

    int Number;
    arrN<bool> Filled;
    arrN<int> Ref;

    bool Valid;
};

struct Ascending
{
    Ascending();

    int PosConstriant;
    int NegConstriant;

    arrN_t Val;

    bool Valid;
    bool Done;

    array2<int,N,N> Probs;
    std::vector<arrN_t> Solutions;

    void Init();
    bool Set(int ref, int value);
    void UpdateProbs();
};

class Grid sealed
{
public:
    Grid();

    bool LoadGrid(std::istream &is);
    bool LoadAscendings(std::istream &is);

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

    arrN<Cover> m_Rows;
    arrN<Cover> m_Cols;
    arrMM<Cover> m_Blks;

    arrN<Ascending> m_RowA;
    arrN<Ascending> m_ColA;

    bool m_Valid;

    bool Set(Cover &cover, int ref, int value);

    bool Init(Cover &cover, Ascending &asc);
    bool Update(Cover &cover, Ascending &asc);
    void Fill(Cover &cover, Ascending &asc);
};

std::ostream &operator<<(std::ostream &os, const Grid &grid);
