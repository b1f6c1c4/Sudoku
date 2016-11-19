#include "stdafx.h"
#include "Searcher.h"
#include <string>

//#undef _DEBUG

#ifdef _DEBUG
#define LOG (std::cerr << std::string(depth, ' '))
#define SUGG std::get<0>(sugg) << " " << std::get<1>(sugg) << " " << std::get<2>(sugg)
#endif

std::shared_ptr<Grid> Searcher::Search(std::shared_ptr<Grid> grid, int depth)
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

#ifdef _DEBUG
        LOG << SUGG << " begin" << std::endl;
#endif

        gr = Search(gr, depth + 1);

#ifdef _DEBUG
        if (gr != nullptr)
        LOG << SUGG << " SUCEED" << std::endl;
        else
        LOG << SUGG << " die" << std::endl;
#endif

        if (gr != nullptr)
            return gr;

        if (!grid->Invalidate(sugg))
            return nullptr;
    }
}

std::shared_ptr<Grid> Searcher::WSearch(std::shared_ptr<Grid> grid, int depth)
{
    if (grid->GetDone() > 40)
        return Search(grid, depth);

    while (true)
    {
        if (!grid->FullSimplify())
            return nullptr;

        if (grid->IsDone())
            return grid;

        std::vector<sugg_t> badSuggs;
        std::vector<std::pair<std::shared_ptr<Grid>, sugg_t>> grs;
        for (auto i = 0; i < grid->SuggestionsLength(); i++)
        {
            auto gr = std::make_shared<Grid>(*grid);
            auto sugg = grid->Suggestions()[i].first;
            if (!gr->Apply(sugg))
            {
#ifdef _DEBUG
                LOG << SUGG << " die imme" << std::endl;
#endif
                badSuggs.push_back(sugg);
                continue;
            }
            if (!gr->Simplify())
            {
#ifdef _DEBUG
                LOG << SUGG << " die imme" << std::endl;
#endif
                badSuggs.push_back(sugg);
                continue;
            }
#ifdef _DEBUG
            LOG << SUGG << " queued" << std::endl;
#endif
            if (gr->IsDone())
            {
                if (!gr->FullSimplify())
                {
#ifdef _DEBUG
                    LOG << SUGG << " die imme" << std::endl;
#endif
                    badSuggs.push_back(sugg);
                    continue;
                }

#ifdef _DEBUG
                LOG << SUGG << " SUCEED imme" << std::endl;
#endif
                return gr;
            }
            grs.push_back(std::make_pair(gr, sugg));
        }
        std::sort(grs.begin(), grs.end(), [](std::pair<std::shared_ptr<Grid>, sugg_t> lhs, std::pair<std::shared_ptr<Grid>, sugg_t> rhs)
                  {
                      return lhs.first->GetDone() > rhs.first->GetDone();
                  });

#ifdef _DEBUG
        for (auto it = grs.begin(); it != grs.end(); ++it)
        {
            auto sugg = it->second;
            LOG << "QUEUE: ";
            std::cerr << SUGG << " @ " << it->first->GetDone() << std::endl;
        }
#endif

        for (auto i = 0; i < grs.size(); i++)
        {
            auto gr = grs[i].first;
            auto sugg = grs[i].second;

            for (auto it = badSuggs.begin(); it != badSuggs.end(); ++it)
                gr->Invalidate(*it);

            if (!gr->FullSimplify())
            {
#ifdef _DEBUG
                LOG << SUGG << " die" << std::endl;
#endif
                badSuggs.push_back(sugg);
                continue;
            }
#ifdef _DEBUG
            LOG << SUGG << " TRY @ " << gr->GetDone() << std::endl;
#endif

            gr = WSearch(gr, depth + 1);

            if (gr != nullptr)
            {
#ifdef _DEBUG
                LOG << SUGG << " SUCEED" << std::endl;
#endif
                return gr;
            }

#ifdef _DEBUG
            LOG << SUGG << " die" << std::endl;
#endif
            badSuggs.push_back(sugg);
        }

        for (auto it = badSuggs.begin(); it != badSuggs.end(); ++it)
            grid->Invalidate(*it);
    }
}
