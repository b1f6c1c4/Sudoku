#include "stdafx.h"
#include "Searcher.h"

std::shared_ptr<Grid> Searcher::Search(std::shared_ptr<Grid> grid)
{
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

        gr = Search(gr);

        if (gr != nullptr)
            return gr;

        if (!grid->Invalidate(sugg))
            return nullptr;
    }
}
