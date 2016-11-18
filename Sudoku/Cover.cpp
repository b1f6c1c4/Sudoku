#include "stdafx.h"
#include "Grid.h"

Cover::Cover() : Number(0), Filled{false}, Valid(true) { }

Grid::Grid() : m_Data{0}, m_Number{0}, m_Filled{{false}}, m_Dirty(false), m_Valid(true)
{
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            m_Rows[i].Ref[j] = i * N + j;

    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            m_Cols[i].Ref[j] = j * N + i;

    for (auto i = 0; i < M; i++)
        for (auto j = 0; j < M; j++)
            for (auto p = 0; p < M; p++)
                for (auto q = 0; q < M; q++)
                    m_Blks[i][j].Ref[p * M + q] = (i * M + p) * N + (j * M + q);
}

bool Grid::LoadGrid(std::istream &is)
{
    int value;
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            is >> value;
            if (value != 0)
                Set(j, i, value);
        }

    return m_Valid;
}

int Grid::Get(int p) const
{
    return Get(p % N, p / N);
}

int Grid::Get(int x, int y) const
{
    return m_Data[y][x];
}

bool Grid::Set(int p, num_t value)
{
    return Set(p % N, p / N, value);
}

bool Grid::Set(int x, int y, num_t value)
{
    if (!m_Valid)
        return false;

    ASSERT(value != 0);

    if (m_Data[y][x] == 0)
        m_Data[y][x] = value;
    else if (m_Data[y][x] == value)
        return true;
    else
        return m_Valid = false;

    ASSERT(!m_Filled[y][x][value - 1]);
    m_Filled[y][x][value - 1] = true;
    ASSERT(m_Number[y][x] != N);
    m_Number[y][x]++;

    m_Dirty = true;

    if (!Set(m_Rows[y], x, value))
        return m_Valid = false;
    if (!Set(m_Cols[x], y, value))
        return m_Valid = false;
    if (!Set(m_Blks[y / M][x / M], (y % M) * M + (x % M), value))
        return m_Valid = false;

    if (!m_RowA[y].Set(x, value))
        return m_Valid = false;
    if (!m_ColA[x].Set(y, value))
        return m_Valid = false;

    return true;
}

bool Grid::FullSimplify()
{
    if (!ReduceInf())
        return false;

    if (!Update())
        return false;

    if (!ReduceInf())
        return false;

    // TODO: calculate probs

    return true;
}

bool Grid::ReduceInf()
{
    do
        if (!Reduce())
            return false;
    while (m_Dirty);
    return true;
}

bool Grid::Reduce()
{
    m_Dirty = false;
    for (auto i = 0; i < N * N; i++)
        if (!Reduce(i))
            return false;
    return true;
}

bool Grid::Reduce(int p)
{
    return Reduce(p % N, p / N);
}

bool Grid::Reduce(int x, int y)
{
    if (!m_Valid)
        return false;

    if (m_Data[y][x] != 0)
        return true;

    if (!m_RowA[y].Solutions.empty())
        for (auto i = 0; i < N; i++)
            m_Filled[y][x][i] |= m_RowA[y].Probs[x][i] == 0;
    if (!m_ColA[y].Solutions.empty())
        for (auto i = 0; i < N; i++)
            m_Filled[y][x][i] |= m_ColA[y].Probs[x][i] == 0;

    auto number = 0;
    for (auto i = 0; i < N; i++)
        if (m_Filled[y][x][i])
            number++;

    m_Number[y][x] = number;

    if (number == N - 1)
    {
        auto flag = false;
        for (auto i = 0; i < N; i++)
            if (!m_Filled[y][x][i])
            {
                ASSERT(!flag);
                flag = true;

                if (!Set(x, y, i + 1))
                    return false;

#ifndef _DEBUG
                break;
#endif
            }

        ASSERT(flag);
    }

    return true;
}

bool Grid::IsValid() const
{
    return m_Valid;
}

bool Grid::Set(Cover &cover, int ref, int value)
{
    if (!cover.Valid)
        return false;

    if (cover.Filled[value - 1])
        return cover.Valid = false;

    ASSERT(cover.Number < N);
    cover.Filled[value - 1] = true;
    cover.Number++;

    if (cover.Number == N - 1)
    {
        auto flag = false;
        int val;
        for (auto i = 0; i < N; i++)
            if (!cover.Filled[i])
            {
                ASSERT(!flag);
                flag = true;

                val = i;

#ifndef _DEBUG
                break;
#endif
            }

        ASSERT(flag);

        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        val = val + 1;

        flag = false;
        for (auto i = 0; i < N; i++)
            if (Get(cover.Ref[i]) == 0)
            {
                ASSERT(!flag);
                flag = true;

                if (!Set(cover.Ref[i], val))
                    return false;

#ifndef _DEBUG
                break;
#endif
            }

        // don't assert here
        // because `bool Set(int p, num_t value)` may change m_Data
        // ASSERT(flag);
    }

    return true;
}

std::ostream &operator<<(std::ostream &os, const Grid &grid)
{
    ASSERT(grid.IsValid());
    for (auto i = 0; i < N; i++)
    {
        for (auto j = 0; j < N; j++)
            os << grid.Get(j, i) << "  ";
        os << std::endl;
    }
    return os;
}
