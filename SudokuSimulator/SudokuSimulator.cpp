#include "stdafx.h"
#include <random>
#include <chrono>
#include <fstream>

int LCre(arrN_t &arr)
{
    auto val = 0;
    auto lv = 0;

    for (auto i = 0; i < N; i++)
        if (arr[i] > val)
        {
            lv++;
            val = arr[i];
        }

    return lv;
}

int RCre(arrN_t &arr)
{
    auto val = 0;
    auto rv = 0;

    for (auto i = N - 1; i >= 0; i--)
        if (arr[i] > val)
        {
            rv++;
            val = arr[i];
        }

    return rv;
}

void SequentialFill(arrN_t &arr)
{
    for (auto i = 0; i < N; i++)
        arr[i] = N - i - 1;
}

bool Tick(arrN_t &arr)
{
    arrN<bool> co = {false};

    auto i = 0;
    for (; i < N; i++)
        if (arr[i] != i)
            break;
    if (i == N)
        return false;

    auto j = N - 2;
    for (; j > i; j--)
        if (arr[j] > arr[j + 1])
            break;

    auto k = i;
    for (; k < j; k++)
        co[arr[k]] = true;

    for (k = arr[j] - 1; k >= i; k--)
        if (!co[k])
            break;

    arr[j++] = k;
    co[k] = true;
    for (k = N - 1; j < N; j++)
    {
        while (co[k])
            k--;

        arr[j] = k--;
    }

    return true;
}

void RandomShuffle(arrN_t &arr)
{
    static auto random = std::mt19937_64(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    std::shuffle(arr.begin(), arr.end(), random);
}

void Write(arrN_t &arr, arrNN<size_t> &res)
{
    for (auto i = 0; i < N; i++)
        res[arr[i]][i]++;
}

int main()
{
    arrNN<arrNN<size_t>> result;
    memset(result.data()->data()->data()->data(), 0, sizeof(size_t) * N * N * N * N);

    arrN_t buff;
    SequentialFill(buff);

    size_t NN = 0;
    while (true)
    {
        auto lv = LCre(buff);
        auto rv = RCre(buff);
        Write(buff, result[lv - 1][rv - 1]);
        NN++;

        if (!Tick(buff))
            break;
    }

    std::cout << NN << std::endl;
    std::cout << "Done. Writing to file2..." << std::endl;

    {
        std::ofstream fout("data2.bin", std::ios::out | std::ios::binary);
        fout.write(reinterpret_cast<char *>(result.data()->data()->data()->data()), sizeof(size_t) * N * N * N * N);
        fout.close();
    }

    std::cout << "Done. Writing to file1p..." << std::endl;

    arrN<arrNN<size_t>> result1;
    memset(result1.data()->data()->data(), 0, sizeof(size_t) * N * N * N);
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            for (auto k = 0; k < N; k++)
                for (auto l = 0; l < N; l++)
                    result1[i][k][l] += result[i][j][k][l];

    {
        std::ofstream fout("data1p.bin", std::ios::out | std::ios::binary);
        fout.write(reinterpret_cast<char *>(result1.data()->data()->data()), sizeof(size_t) * N * N * N);
        fout.close();
    }

    std::cout << "Done. Writing to file1n..." << std::endl;

    memset(result1.data()->data()->data(), 0, sizeof(size_t) * N * N * N);
    for (auto i = 0; i < N; i++)
        for (auto j = 0; j < N; j++)
            for (auto k = 0; k < N; k++)
                for (auto l = 0; l < N; l++)
                    result1[j][k][l] += result[i][j][k][l];

    {
        std::ofstream fout("data1n.bin", std::ios::out | std::ios::binary);
        fout.write(reinterpret_cast<char *>(result1.data()->data()->data()), sizeof(size_t) * N * N * N);
        fout.close();
    }

    return 0;
}
