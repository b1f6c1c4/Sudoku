#include "stdafx.h"
#include "Grid.h"
#include <fstream>
#include <sstream>

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
        Grid g;


#ifdef _DEBUG
        auto ss = std::stringstream(R"(
1>(table)
1  0  0  0  0  0  0  4  9  
0  4  0  1  3  6  2  7  0  
0  7  0  0  0  9  0  1  0  
0  0  2  7  0  0  9  0  4  
9  0  0  0  0  0  0  0  0  
7  0  8  0  9  0  0  0  0  
5  2  0  9  0  0  0  0  3  
0  0  0  4  2  1  5  9  7  
0  0  0  0  0  0  0  0  0  
1>(floor)(以下四行分别是左右上下四方向的楼层数）
0  0  4  4  0  3  2  3  2  
1  3  2  2  2  3  0  0  5  
3  4  0  3  2  3  2  4  0  
4  0  3  0  2  0  3  2  0  
)");
        std::string str;
        ss >> str;
        g.LoadGrid(ss);
        ss >> str;
        g.LoadAscendings(ss);
#else
        g.LoadGrid(std::cin);
        g.LoadAscendings(std::cin);
#endif

        if (g.FullSimplify())
            std::cout << g;
        else
            std::cout << "Invalid grid.";
#ifdef _DEBUG
        break;
#endif
    }
    system("pause");
    return 0;
}
