#include "stdafx.h"
#include "Grid.h"
#include <fstream>
#include <sstream>
#include "Searcher.h"

int main()
{
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
        i++;
    }
}
