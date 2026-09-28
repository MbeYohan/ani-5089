#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

static bool LireLigne(std::string &ligne)
{
    while (std::getline(std::cin, ligne))
    {
        if (!ligne.empty() && ligne.back() == '\r')
        {
            ligne.pop_back();
        }
        if (ligne.find_first_not_of(" \t") != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

int main()
{
    std::map<std::string, std::vector<std::string>> besoins;

    std::string ligne;
    int n = 0;
    if (LireLigne(ligne))
    {
        n = std::stoi(ligne);
    }
    for (int i = 0; i < n && LireLigne(ligne); ++i)
    {
        std::istringstream flux(ligne);
        std::string module;
        flux >> module;
        std::string besoin;
        while (flux >> besoin)
        {
            besoins[module].push_back(besoin);
        }
    }

    int m = 0;
    std::cin >> m;
    std::set<std::string> liste;
    std::queue<std::string> aTraiter;
    for (int i = 0; i < m; ++i)
    {
        std::string nom;
        if (!(std::cin >> nom))
        {
            break;
        }
        if (liste.insert(nom).second)
        {
            aTraiter.push(nom);
        }
    }

    while (!aTraiter.empty())
    {
        std::string courant = aTraiter.front();
        aTraiter.pop();
        auto trouve = besoins.find(courant);
        if (trouve == besoins.end())
        {
            continue;
        }
        for (const std::string &besoin : trouve->second)
        {
            if (liste.insert(besoin).second)
            {
                aTraiter.push(besoin);
            }
        }
    }

    for (const std::string &nom : liste)
    {
        std::cout << nom << '\n';
    }
    return 0;
}
