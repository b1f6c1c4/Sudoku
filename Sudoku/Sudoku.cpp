#include "stdafx.h"
#include "Grid.h"

int main()
{
    while (true)
    {
        Grid g;
        g.LoadGrid(std::cin);
        g.LoadAscendings(std::cin);

        g.Reduce();
        g.Update();
        g.Reduce();

        std::cout << g;
    }
    system("pause");
    return 0;
}
