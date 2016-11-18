#include "stdafx.h"
#include "Grid.h"
#include <fstream>
#include <sstream>
#include "Searcher.h"

int main()
{
    {
        std::ifstream fin("data1p.bin", std::ios::in | std::ios::binary);
        fin.read(reinterpret_cast<char *>(SimulatorResultOnePos.data()->data()->data()), sizeof(size_t) * N * N * N);
        fin.close();
    }
    {
        std::ifstream fin("data1n.bin", std::ios::in | std::ios::binary);
        fin.read(reinterpret_cast<char *>(SimulatorResultOneNeg.data()->data()->data()), sizeof(size_t) * N * N * N);
        fin.close();
    }
    {
        std::ifstream fin("data2.bin", std::ios::in | std::ios::binary);
        fin.read(reinterpret_cast<char *>(SimulatorResultTwo.data()->data()->data()->data()), sizeof(size_t) * N * N * N * N);
        fin.close();
    }

    while (true)
    {
        auto g = std::make_shared<Grid>();


#ifdef _DEBUG
        auto ss = std::stringstream(R"(
1>(table)
0  7  4  1  0  0  0  0  0  
5  2  0  6  0  0  0  0  0  
1  0  8  5  0  9  0  7  4  
7  6  0  4  0  0  0  0  2  
0  5  0  0  6  0  0  3  0  
4  8  3  0  5  0  0  6  1  
0  0  0  8  1  7  0  0  5  
8  9  5  0  0  6  0  1  0  
2  0  0  0  0  0  0  0  0  
1>(floor)(以下四行分别是左右上下四方向的楼层数）
4  2  4  0  1  3  5  0  3  
1  0  3  3  2  4  2  2  0  
3  0  2  0  0  3  3  3  1  
0  2  0  0  1  5  2  0  4  
)");
        std::string str;
        ss >> str;
        g->LoadGrid(ss);
        ss >> str;
        g->LoadAscendings(ss);
#else
        g->LoadGrid(std::cin);
        g->LoadAscendings(std::cin);
#endif

        g = Searcher::Search(g);

        if (g == nullptr)
            std::cout << "Invalid grid.";
        else
            std::cout << *g;

#ifdef _DEBUG
        break;
#endif
    }
    system("pause");
    return 0;
}
