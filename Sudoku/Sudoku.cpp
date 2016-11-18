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
        auto ss = std::stringstream(R"(1  4  0  6  0  7  0  2  9  
0  2  0  0  4  8  0  3  7  
0  0  0  0  0  0  0  4  6  
4  1  6  0  5  9  0  7  2  
8  7  0  4  1  3  6  9  0  
5  0  3  7  0  0  4  0  0  
0  0  0  3  0  4  9  0  8  
0  3  8  0  0  0  2  5  0  
0  0  0  0  0  5  0  0  0  

7  2  3  4  2  2  4  3  1  
1  3  2  3  2  3  2  4  4  
4  3  2  3  3  3  2  5  1  
1  2  3  2  2  3  2  5  4  
)");
        g.LoadGrid(ss);
        g.LoadAscendings(ss);
#else
        g.LoadGrid(std::cin);
        g.LoadAscendings(std::cin);
#endif

        g.Reduce();
        g.Update();
        g.Reduce();

        std::cout << g;
#ifdef _DEBUG
        break;
#endif
    }
    system("pause");
    return 0;
}
