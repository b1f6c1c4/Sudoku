#pragma once

#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>

#include <string>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <functional>

#define M 3
#define N 9
#include <sstream>

typedef int num_t;

template <typename T, int P>
using array1 = std::array<T, P>;

template <typename T, int Q, int P>
using array2 = std::array<std::array<T, P>, Q>;

template <typename T>
using arrN = array1<T, N>;

template <typename T>
using arrNN = array2<T, N, N>;

template <typename T>
using arrMM = array2<T, M, M>;

typedef arrN<num_t> arrN_t;

struct Grid
{
    arrNN<num_t> table;
    array1<arrN<int>, 4> floor;
};

std::istream &operator >>(std::istream &is, Grid &grid)
{
    std::string s;
    is >> s;
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            is >> grid.table[i][j];

    is >> s;
    for (auto i = 0; i < 4; i++)
        for (auto j = 0; j < N; j++)
            is >> grid.floor[i][j];

    return is;
}

std::ostream &operator <<(std::ostream &os, Grid &grid)
{
    os << ">(table)" << std::endl;
    for (auto i = 0; i < N; i++)
    {
        for (auto j = 0; j < N; j++)
            os << grid.table[i][j] << "  ";
        os << std::endl;
    }

    os << ">(floor)" << std::endl;
    for (auto i = 0; i < 4; i++)
    {
        for (auto j = 0; j < N; j++)
            os << grid.floor[i][j] << "  ";
        os << std::endl;
    }

    return os;
}

void Validate(const Grid &grid, const Grid &grMask)
{
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            if (grid.table[i][j] >= 1 && grid.table[i][j] <= 9)
                continue;

            std::ostringstream ss;
            ss << "Table not filled: ";
            ss << "Row " << i + 1 << ", ";
            ss << "Col " << j + 1 << ", ";
            ss << "got " << grid.table[i][j];
            throw std::runtime_error(ss.str());
        }

    for (auto i = 0; i < N; i++)
    {
        arrN<bool> filled = {false};

        for (auto j = 0; j < N; j++)
        {
            if (!filled[grid.table[i][j] - 1])
            {
                filled[grid.table[i][j] - 1] = true;
                continue;
            }

            std::ostringstream ss;
            ss << "Row " << i + 1 << " has conflct: ";
            ss << "at least two " << grid.table[i][j];
            throw std::runtime_error(ss.str());
        }
    }

    for (auto i = 0; i < N; i++)
    {
        arrN<bool> filled = {false};

        for (auto j = 0; j < N; j++)
        {
            if (!filled[grid.table[j][i] - 1])
            {
                filled[grid.table[j][i] - 1] = true;
                continue;
            }

            std::ostringstream ss;
            ss << "Col " << i + 1 << " has conflct: ";
            ss << "at least two " << grid.table[j][i];
            throw std::runtime_error(ss.str());
        }
    }

    for (auto i = 0; i < M; i++)
        for (auto j = 0; j < M; j++)
        {
            arrN<bool> filled = {false};

            for (auto k = 0; k < M; k++)
                for (auto l = 0; l < M; l++)
                {
                    if (!filled[grid.table[i * M + k][j * M + l] - 1])
                    {
                        filled[grid.table[i * M + k][j * M + l] - 1] = true;
                        continue;
                    }

                    std::ostringstream ss;
                    ss << "Blk (" << i + 1 << "," << j + 1 << ") has conflct: ";
                    ss << "at least two " << grid.table[i * M + k][j * M + l];
                    throw std::runtime_error(ss.str());
                }
        }

    for (auto j = 0; j < N; j++)
    {
        {
            auto val = 0;
            auto lv = 0;

            for (auto i = 0; i < N; i++)
                if (grid.table[j][i] > val)
                {
                    lv++;
                    val = grid.table[j][i];
                }

            if (grid.floor[0][j] != lv)
            {
                std::ostringstream ss;
                ss << "Floor incorrect: ";
                ss << "Row " << j + 1 << " (Left) ";
                ss << "should be " << lv << ", ";
                ss << "got " << grid.floor[0][j];
                throw std::runtime_error(ss.str());
            }
        }
        {
            auto val = 0;
            auto rv = 0;

            for (auto i = N - 1; i >= 0; i--)
                if (grid.table[j][i] > val)
                {
                    rv++;
                    val = grid.table[j][i];
                }

            if (grid.floor[1][j] != rv)
            {
                std::ostringstream ss;
                ss << "Floor incorrect: ";
                ss << "Row " << j + 1 << " (Right) ";
                ss << "should be " << rv << ", ";
                ss << "got " << grid.floor[1][j];
                throw std::runtime_error(ss.str());
            }
        }
    }

    for (auto j = 0; j < N; j++)
    {
        {
            auto val = 0;
            auto lv = 0;

            for (auto i = 0; i < N; i++)
                if (grid.table[i][j] > val)
                {
                    lv++;
                    val = grid.table[i][j];
                }

            if (grid.floor[2][j] != lv)
            {
                std::ostringstream ss;
                ss << "Floor incorrect: ";
                ss << "Col " << j + 1 << " (Top) ";
                ss << "should be " << lv << ", ";
                ss << "got " << grid.floor[2][j];
                throw std::runtime_error(ss.str());
            }
        }
        {
            auto val = 0;
            auto rv = 0;

            for (auto i = N - 1; i >= 0; i--)
                if (grid.table[i][j] > val)
                {
                    rv++;
                    val = grid.table[i][j];
                }

            if (grid.floor[3][j] != rv)
            {
                std::ostringstream ss;
                ss << "Floor incorrect: ";
                ss << "Col " << j + 1 << " (Bottom) ";
                ss << "should be " << rv << ", ";
                ss << "got " << grid.floor[3][j];
                throw std::runtime_error(ss.str());
            }
        }
    }

    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
        {
            if (grMask.table[i][j] == 0)
                continue;
            if (grid.table[i][j] == grMask.table[i][j])
                continue;

            std::ostringstream ss;
            ss << "Table not match: ";
            ss << "Row " << i + 1 << ", ";
            ss << "Col " << j + 1 << ", ";
            ss << "should be " << grMask.table[i][j] << " ";
            ss << "but got " << grid.table[i][j];
            throw std::runtime_error(ss.str());
        }

    for (auto i = 0; i < 4; i++)
        for (auto j = 0; j < N; j++)
        {
            if (grMask.floor[i][j] == 0)
                continue;
            if (grid.floor[i][j] == grMask.floor[i][j])
                continue;

            std::ostringstream ss;
            ss << "Floor not match: ";
            switch (i)
            {
            case 0:
                ss << "Row " << j + 1 << " (Left) ";
                break;
            case 1:
                ss << "Row " << j + 1 << " (Right) ";
                break;
            case 2:
                ss << "Col " << j + 1 << " (Top) ";
                break;
            case 3:
                ss << "Col " << j + 1 << " (Bottom) ";
                break;
            }
            ss << "should be " << grMask.floor[i][j] << " ";
            ss << "but got " << grid.floor[i][j];
            throw std::runtime_error(ss.str());
        }
}

int main()
{
    std::string s;

    std::vector<Grid> grids;
    {
        std::ifstream fin("problem.txt");

        Grid grMask;
        while (!fin.eof())
        {
            fin >> grMask;
            grids.push_back(grMask);
        }
    }

    Grid grid;
    for (auto i = 0; i < grids.size(); i++)
    {
        try
        {
            std::cin >> grid;
            Validate(grid, grids[i]);
        }
        catch (const std::exception &err)
        {
            std::cerr << grid;
            std::cerr << err.what() << std::endl;
        }
    }

    return 0;
}
