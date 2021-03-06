#include "stdafx.h"
#include "Grid.h"

Cover::Cover() : Number(0), Filled{false}, Valid(true) { }

Grid::Grid() : m_Data{0}, m_Number{0}, m_Filled{{false}}, m_Dirty(false), m_Valid(true), m_Done(0)
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

    ASSERT(m_Done != N * N);
    m_Done++;

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

bool Grid::FullSimplify(int numSugg)
{
    if (!Simplify())
        return false;

    while (true)
    {
        if (!Update())
            return false;

        if (!Reduce())
            return false;

        if (!m_Dirty)
            break;

        if (!Simplify())
            return false;
    }

    if (IsDone())
        return true;

    return EstimateProbs(numSugg);
}

sugg_t Grid::Suggestion() const
{
    if (!m_Valid)
        return std::make_tuple(N, N, 0);

    if (m_Done == N * N)
        return std::make_tuple(0, 0, 0);

    ASSERT(m_SuggestionsLength > 0);

    return m_Suggestions[0].first;
}

int Grid::SuggestionsLength() const
{
    return m_SuggestionsLength;
}

const array1<std::pair<sugg_t, double>, WX> &Grid::Suggestions() const
{
    return m_Suggestions;
}

bool Grid::EstimateProbs(int numSugg)
{
    arrNN<size_t> rowA = {{0}};
    arrNN<size_t> colA = {{0}};
    for (auto i = 0; i < N; i++)
    {
        if (m_RowA[i].Done)
            continue;
        for (auto j = 0; j < N; j++)
        {
            if (m_Data[i][j] != 0)
                continue;

            if (m_Number[i][j] == N)
                return false;

            for (auto k = 0; k < N; k++)
            {
                if (m_Filled[i][j][k])
                    continue;

                rowA[i][j] += m_RowA[i].Probs[j][k];
            }
        }
    }
    for (auto i = 0; i < N; i++)
    {
        if (m_ColA[i].Done)
            continue;
        for (auto j = 0; j < N; j++)
        {
            if (m_Data[j][i] != 0)
                continue;

            if (m_Number[j][i] == N)
                return false;

            for (auto k = 0; k < N; k++)
            {
                if (m_Filled[j][i][k])
                    continue;

                colA[i][j] += m_ColA[i].Probs[j][k];
            }
        }
    }

    auto lng = 0;

    for (auto j = 0; j < N; j++)
        for (auto i = 0; i < N; i++)
        {
            if (m_Data[j][i] != 0)
                continue;

            if (m_Number[j][i] == N)
                return false;

            auto flag = false;
            for (auto k = 0; k < N; k++)
            {
                if (m_Filled[j][i][k])
                    continue;

                auto prob = 1.0;
                prob /= N - m_Rows[j].Number;
                prob /= N - m_Cols[i].Number;
                prob /= N - m_Blks[j / M][i / M].Number;
                if (!m_RowA[j].Done)
                {
                    prob *= m_RowA[j].Probs[i][k];
                    prob /= rowA[j][i];
                    ASSERT(rowA[j][i] > 0);
                }
                if (!m_ColA[i].Done)
                {
                    prob *= m_ColA[i].Probs[j][k];
                    prob /= colA[i][j];
                    ASSERT(colA[i][j] > 0);
                }

                ASSERT(!isnan(prob));
                ASSERT(prob > 0);

                auto id = 0;
                for (; id < lng; id++)
                    if (prob > m_Suggestions[id].second)
                        break;

                if (id != lng)
                {
                    auto n = lng - id;
                    if (lng == numSugg)
                        n--;
                    memmove(reinterpret_cast<void *>(m_Suggestions.data() + id + 1), reinterpret_cast<void *>(m_Suggestions.data() + id), sizeof(*m_Suggestions.data()) * n);
                    m_Suggestions[id] = std::make_pair(std::make_tuple(i, j, k + 1), prob);
                    if (lng != numSugg)
                        lng++;
                }
                else if (lng != numSugg)
                {
                    m_Suggestions[id] = std::make_pair(std::make_tuple(i, j, k + 1), prob);
                    lng++;
                }

                flag = true;
            }
            ASSERT(flag);
        }

    ASSERT(lng != 0);
    m_SuggestionsLength = lng;

    return true;
}

bool Grid::Invalidate(int p, num_t value)
{
    return Invalidate(p % N, p / N, value);
}

bool Grid::Invalidate(int x, int y, num_t value)
{
    if (!m_Valid)
        return false;

    if (m_Filled[y][x][value - 1])
        return true;

    m_Dirty = true;

    m_Filled[y][x][value - 1] = true;

    ASSERT(m_Number[y][x] != N);
    m_Number[y][x]++;

    if (m_Number[y][x] == N)
        return false;

    if (m_Number[y][x] == N - 1)
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

bool Grid::Simplify()
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

    if (m_RowA[y].HasProbs)
        for (auto i = 0; i < N; i++)
            m_Filled[y][x][i] |= m_RowA[y].Probs[x][i] == 0;
    if (m_ColA[x].HasProbs)
        for (auto i = 0; i < N; i++)
            m_Filled[y][x][i] |= m_ColA[x].Probs[y][i] == 0;

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

bool Grid::IsDone() const
{
    return m_Done == N * N;
}

int Grid::GetDone() const
{
    return m_Done;
}

bool Grid::Apply(sugg_t sugg)
{
    return Set(std::get<0>(sugg), std::get<1>(sugg), std::get<2>(sugg));
}

bool Grid::Invalidate(sugg_t sugg)
{
    return Invalidate(std::get<0>(sugg), std::get<1>(sugg), std::get<2>(sugg));
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

    for (auto i = 0; i < N; i++)
    {
        auto x = cover.Ref[i] % N, y = cover.Ref[i] / N;

        if (m_Filled[y][x][value - 1])
            continue;
        m_Filled[y][x][value - 1] = true;
        ASSERT(m_Number[y][x] != N);
        m_Number[y][x]++;
    }

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
