#include "stdafx.h"
#include "Grid.h"

Cover::Cover() : Number(0), Filled{false}, Valid(true) { }

Grid::Grid() : m_Data{0}, m_Valid(true)
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

    if (!Set(m_Rows[y], x, value))
        return m_Valid = false;
    if (!Set(m_Cols[x], y, value))
        return m_Valid = false;
    if (!Set(m_Blks[y / M][x / M], (y % M) * M + (x % M), value))
        return m_Valid = false;

    if (!Set(m_Rows[y], m_RowA[y], x, value))
        return m_Valid = false;
    if (!Set(m_Cols[x], m_ColA[x], y, value))
        return m_Valid = false;

    return true;
}

bool Grid::Reduce()
{
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

    bool filled[N] = {false};
    for (auto i = 0; i < N; i++)
        filled[i] |= m_Rows[y].Filled[i];
    for (auto i = 0; i < N; i++)
        filled[i] |= m_Cols[x].Filled[i];
    for (auto i = 0; i < N; i++)
        filled[i] |= m_Blks[y / M][x / M].Filled[i];

    if (!m_RowA[y].Solutions.empty())
        for (auto i = 0; i < N; i++)
            filled[i] |= m_RowA[y].Probs[x][i] == 0;
    if (!m_ColA[y].Solutions.empty())
        for (auto i = 0; i < N; i++)
            filled[i] |= m_ColA[y].Probs[x][i] == 0;

    auto number = 0;
    for (auto i = 0; i < N; i++)
        if (filled[i])
            number++;

    if (number == N - 1)
    {
        auto flag = false;
        for (auto i = 0; i < N; i++)
            if (!filled[i])
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

std::istream &operator>>(std::istream &is, Grid &grid)
{
    int value;
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            is >> value;
            if (value != 0)
                grid.Set(j, i, value);
        }
    return is;
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
