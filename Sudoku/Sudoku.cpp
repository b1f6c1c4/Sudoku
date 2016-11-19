#include "stdafx.h"
#include "Grid.h"
#include <fstream>
#include <chrono>
#include <sstream>
#include "Searcher.h"

int main(int argc, char *argv[])
{
    auto timing = false;
    if (argc == 2 && strcmp(argv[1], "-t") == 0)
        timing = true;

    {
        std::ifstream fin("data1p.bin", std::ios::in | std::ios::binary | std::ios::_Nocreate);
        fin.read(reinterpret_cast<char *>(SimulatorResultOnePos.data()->data()->data()), sizeof(size_t) * N * N * N);
        fin.close();
    }
    {
        std::ifstream fin("data1n.bin", std::ios::in | std::ios::binary | std::ios::_Nocreate);
        fin.read(reinterpret_cast<char *>(SimulatorResultOneNeg.data()->data()->data()), sizeof(size_t) * N * N * N);
        fin.close();
    }
    {
        std::ifstream fin("data2.bin", std::ios::in | std::ios::binary | std::ios::_Nocreate);
        fin.read(reinterpret_cast<char *>(SimulatorResultTwo.data()->data()->data()->data()), sizeof(size_t) * N * N * N * N);
        fin.close();
    }

    auto i = 1;
    while (true)
    {
        auto g = std::make_shared<Grid>();

        std::string str;
        std::cin >> str;
        if (str.empty())
            return 0;

        auto t1 = std::chrono::high_resolution_clock::now();

        g->LoadGrid(std::cin);
        std::cin >> str;
        g->LoadAscendings(std::cin);

        g = Searcher::Search(g);

        if (g == nullptr)
            std::cout << "Invalid grid." << std::endl;
        else
        {
            std::cout << i << ">(table)" << std::endl;
            g->WriteGrid(std::cout);
            std::cout << i << ">(floor)" << std::endl;
            g->WriteAscendings(std::cout);
        }

        auto t2 = std::chrono::high_resolution_clock::now();

        if (timing)
            std::cerr << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << std::endl;

        i++;
    }
}
