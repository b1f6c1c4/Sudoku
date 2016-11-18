#include "stdafx.h"
#include "Searcher.h"
#include <string>

//#undef _DEBUG

std::shared_ptr<Grid> Searcher::Search(std::shared_ptr<Grid> grid)
{
#ifdef _DEBUG
    static int depth = 0;
#endif

    while (true)
    {
        if (!grid->FullSimplify())
            return nullptr;

        if (grid->IsDone())
            return grid;

        auto sugg = grid->Suggestion();
        auto gr = std::make_shared<Grid>(*grid);
        if (!gr->Apply(sugg))
            continue;

#ifdef _DEBUG
        std::cerr << std::string(depth, ' ') << std::get<0>(sugg) << " " << std::get<1>(sugg) << " " << std::get<2>(sugg) << " begin" << std::endl;
#endif

#ifdef _DEBUG
        depth++;
#endif
        gr = Search(gr);
#ifdef _DEBUG
        depth--;
#endif

#ifdef _DEBUG
        if (gr != nullptr)
            std::cerr << std::string(depth, ' ') << std::get<0>(sugg) << " " << std::get<1>(sugg) << " " << std::get<2>(sugg) << " SUCEED" << std::endl;
        else
            std::cerr << std::string(depth, ' ') << std::get<0>(sugg) << " " << std::get<1>(sugg) << " " << std::get<2>(sugg) << " die" << std::endl;
#endif

        if (gr != nullptr)
            return gr;

        if (!grid->Invalidate(sugg))
            return nullptr;
    }
}
