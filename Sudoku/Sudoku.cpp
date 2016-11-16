#include "stdafx.h"
#include "Grid.h"

int main()
{
    while (true)
    {
        Grid g;
        std::cin >> g;
        g.Reduce();
        std::cout << g;
    }
    system("pause");
    return 0;
}
