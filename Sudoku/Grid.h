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

extern arrN<arrNN<size_t>> SimulatorResultOnePos;
extern arrN<arrNN<size_t>> SimulatorResultOneNeg;
extern arrNN<arrNN<size_t>> SimulatorResultTwo;

struct Ascending
{
    Ascending();

    int PosConstriant;
    int NegConstriant;

    arrN_t Val;

    bool Valid;
    bool Done;

    bool HasProbs;
    arrNN<size_t> Probs;
    std::vector<arrN_t> Solutions;

    void Init();
    bool Set(int ref, int value);
    void UpdateProbs();
};

class Grid sealed
{
public:
    Grid();
    Grid(const Grid &other);

    bool LoadGrid(std::istream &is);
    bool LoadAscendings(std::istream &is);
    void WriteGrid(std::ostream &os) const;
    void WriteAscendings(std::ostream &os) const;

    int Get(int p) const;
    int Get(int x, int y) const;

    bool FullSimplify();

    std::tuple<int, int, num_t> Suggestion() const;

    bool IsValid() const;
    bool IsDone() const;
    int GetDone() const;

    bool Apply(std::tuple<int, int, num_t> sugg);
    bool Invalidate(std::tuple<int, int, num_t> sugg);
private:
    arrNN<num_t> m_Data;

    arrN<Cover> m_Rows;
    arrN<Cover> m_Cols;
    arrMM<Cover> m_Blks;

    arrN<Ascending> m_RowA;
    arrN<Ascending> m_ColA;

    arrNN<int> m_Number;
    arrNN<arrN<bool>> m_Filled;
    std::unique_ptr<arrNN<arrN<double>>> m_Probs;
    bool m_Dirty;

    bool m_Valid;
    int m_Done;

    std::tuple<int, int, num_t> m_Suggested;

    bool Invalidate(int p, num_t value);
    bool Invalidate(int x, int y, num_t value);

    bool Set(int p, num_t value);
    bool Set(int x, int y, num_t value);

    bool ReduceInf();
    bool Reduce();
    bool Reduce(int p);
    bool Reduce(int x, int y);

    bool Update();

    bool Set(Cover &cover, int ref, int value);

    bool Init(Cover &cover, Ascending &asc);
    bool Update(Cover &cover, Ascending &asc);
    void Fill(Cover &cover, Ascending &asc);
};
