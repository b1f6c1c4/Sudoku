#include "stdafx.h"
#include "Grid.h"

arrN<arrNN<size_t>> SimulatorResultOnePos;
arrN<arrNN<size_t>> SimulatorResultOneNeg;
arrNN<arrNN<size_t>> SimulatorResultTwo;

Ascending::Ascending() : PosConstriant(0), NegConstriant(0), Valid(true), Done(true), HasProbs(false) { }

void Ascending::Init()
{
    Done = PosConstriant == 0 && NegConstriant == 0;
}

bool Ascending::Set(int ref, int value)
{
    if (!Valid)
        return false;

    if (Done)
        return true;

    if (Solutions.empty())
        return true;

    Solutions.erase(std::remove_if(Solutions.begin(), Solutions.end(), [ref, value](auto &arr)
                                   {
                                       return arr[ref] != value;
                                   }), Solutions.end());
    if (Solutions.empty())
        return false;

    UpdateProbs();

    return true;
}

void Ascending::UpdateProbs()
{
    memset(reinterpret_cast<void *>(Probs.data()->data()), 0, sizeof(*Probs.data()->data()) * N * N);

    std::for_each(Solutions.begin(), Solutions.end(), [this](auto &arr)
                  {
                      for (auto i = 0; i < N; i++)
                          Probs[i][arr[i] - 1]++;
                  });
}

bool Grid::LoadAscendings(std::istream &is)
{
    if (!m_Valid)
        return false;

    for (auto i = 0; i < N; i++)
        is >> m_RowA[i].PosConstriant;
    for (auto i = 0; i < N; i++)
        is >> m_RowA[i].NegConstriant;
    for (auto i = 0; i < N; i++)
        is >> m_ColA[i].PosConstriant;
    for (auto i = 0; i < N; i++)
        is >> m_ColA[i].NegConstriant;

    for (auto i = 0; i < N; i++)
        if (!Init(m_Rows[i], m_RowA[i]))
            return false;
    for (auto i = 0; i < N; i++)
        if (!Init(m_Cols[i], m_ColA[i]))
            return false;

    return true;
}

void Grid::WriteGrid(std::ostream &os) const
{
    ASSERT(m_Valid);
    for (auto i = 0; i < N; i++)
    {
        for (auto j = 0; j < N; j++)
            os << m_Data[i][j] << "  ";
        os << std::endl;
    }
}

void Grid::WriteAscendings(std::ostream &os) const
{
    auto LCre = [this](const Cover &cover)
        {
            auto val = 0;
            auto lv = 0;

            for (auto i = 0; i < N; i++)
                if (Get(cover.Ref[i]) > val)
                {
                    lv++;
                    val = Get(cover.Ref[i]);
                }

            return lv;
        };

    auto RCre = [this](const Cover &cover)
        {
            auto val = 0;
            auto rv = 0;

            for (auto i = N - 1; i >= 0; i--)
                if (Get(cover.Ref[i]) > val)
                {
                    rv++;
                    val = Get(cover.Ref[i]);
                }

            return rv;
        };

    for (auto i = 0; i < N; i++)
        os << LCre(m_Rows[i]) << "  ";
    os << std::endl;
    for (auto i = 0; i < N; i++)
        os << RCre(m_Rows[i]) << "  ";
    os << std::endl;
    for (auto i = 0; i < N; i++)
        os << LCre(m_Cols[i]) << "  ";
    os << std::endl;
    for (auto i = 0; i < N; i++)
        os << RCre(m_Cols[i]) << "  ";
    os << std::endl;
}

bool Grid::Init(Cover &cover, Ascending &asc)
{
    if (asc.PosConstriant + asc.NegConstriant > N + 1)
        return false;

    if (asc.PosConstriant == N)
    {
        for (auto j = 0; j < N; j++)
            if (!Set(cover.Ref[j], j + 1))
                return false;
        return true;
    }

    if (asc.NegConstriant == N)
    {
        for (auto j = 0; j < N; j++)
            if (!Set(cover.Ref[N - j], j + 1))
                return false;
        return true;
    }

    if (asc.PosConstriant + asc.NegConstriant == N + 1)
        return Set(cover.Ref[asc.PosConstriant - 1], N);

    if (asc.PosConstriant == 1)
    {
        if (!Set(cover.Ref[0], N))
            return false;
        asc.PosConstriant = 0;
        asc.Init();
        return true;
    }

    if (asc.NegConstriant == 1)
    {
        if (!Set(cover.Ref[N - 1], N))
            return false;
        asc.NegConstriant = 0;
        asc.Init();
        return true;
    }

    asc.Init();
    return true;
}

bool Grid::Check(Cover &cover, Ascending &asc)
{
    if (!asc.Valid)
        return false;

    if (asc.Done)
        return true;

    if (cover.Number == 9)
    {
        if (asc.PosConstriant != 0)
        {
            auto val = 0;
            auto lv = 0;

            for (auto i = 0; i < N; i++)
                if (Get(cover.Ref[i]) > val)
                {
                    lv++;
                    val = Get(cover.Ref[i]);
                }

            if (lv != asc.PosConstriant)
                return asc.Valid = false;
        }

        if (asc.NegConstriant != 0)
        {
            auto val = 0;
            auto rv = 0;

            for (auto i = N - 1; i >= 0; i--)
                if (Get(cover.Ref[i]) > val)
                {
                    rv++;
                    val = Get(cover.Ref[i]);
                }

            if (rv != asc.NegConstriant)
                return asc.Valid = false;
        }

        return asc.Done = true;
    }

    return true;
}

bool Grid::Update()
{
    for (auto i = 0; i < N; i++)
        if (!Update(m_Rows[i], m_RowA[i]))
            return false;

    for (auto i = 0; i < N; i++)
        if (!Update(m_Cols[i], m_ColA[i]))
            return false;

    return true;
}

void Fill(arrN<bool> filled, std::vector<arrN_t> &solutions, arrN_t state, int index, std::function<bool(arrN_t &)> pred)
{
    auto indexX = N;
    for (auto i = index + 1; i < N; i++)
        if (state[i] == 0)
        {
            indexX = i;
            break;
        }

    for (auto i = 0; i < N; i++)
        if (!filled[i])
        {
            filled[i] = true;
            state[index] = i + 1;
            if (indexX == N)
            {
                if (pred(state))
                    solutions.push_back(state);
            }
            else
                Fill(filled, solutions, state, indexX, pred);
            filled[i] = false;
        }
}

void Grid::Fill(Cover &cover, Ascending &asc)
{
    arrN_t state;

    auto index = N;

    for (auto i = 0; i < N; i++)
    {
        state[i] = Get(cover.Ref[i]);
        if (state[i] == 0 && index == N)
            index = i;
    }

    ::Fill(cover.Filled, asc.Solutions, state, index, [&asc](auto &sol)
           {
               if (asc.PosConstriant != 0)
               {
                   auto v = 0;
                   auto c = 0;
                   for (auto i = 0; i < N; i++)
                       if (sol[i] > v)
                       {
                           if (++c > asc.PosConstriant)
                               return false;
                           v = sol[i];
                       }
                   if (c != asc.PosConstriant)
                       return false;
               }

               if (asc.NegConstriant != 0)
               {
                   auto v = 0;
                   auto c = 0;
                   for (auto i = N - 1; i >= 0; i--)
                       if (sol[i] > v)
                       {
                           if (++c > asc.NegConstriant)
                               return false;
                           v = sol[i];
                       }
                   if (c != asc.NegConstriant)
                       return false;
               }

               return true;
           });

    asc.UpdateProbs();
}

bool Grid::Update(Cover &cover, Ascending &asc)
{
    if (!asc.Valid)
        return false;

    if (!Check(cover, asc))
        return false;

    if (asc.Done)
        return true;

    if (!asc.Solutions.empty())
        return true;

    if (cover.Number >= N - 5)
    {
        asc.HasProbs = true;

        Fill(cover, asc);

        if (asc.Solutions.empty())
            return asc.Valid = false;

        return true;
    }

    if (asc.HasProbs)
        return true;

    asc.HasProbs = true;

    if (asc.PosConstriant != 0 && asc.NegConstriant != 0)
    {
        asc.Probs = SimulatorResultTwo[asc.PosConstriant - 1][asc.NegConstriant - 1];
        return true;
    }

    m_Dirty = true;

    if (asc.PosConstriant != 0)
    {
        asc.Probs = SimulatorResultOnePos[asc.PosConstriant - 1];
    }
    else // if (asc.NegConstriant != 0)
    {
        asc.Probs = SimulatorResultOneNeg[asc.NegConstriant - 1];
    }

    return true;
}
