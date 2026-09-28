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
    std::map<std::string, std::set<std::string>> besoins;

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
        besoins[module];
        std::string besoin;
        while (flux >> besoin)
        {
            besoins[module].insert(besoin);
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
        for (const std::string &besoin : besoins[courant])
        {
            if (liste.insert(besoin).second)
            {
                aTraiter.push(besoin);
            }
        }
    }

    std::map<std::string, int> compte;
    for (const std::string &nom : liste)
    {
        compte[nom];
    }
    for (const std::string &nom : liste)
    {
        for (const std::string &besoin : besoins[nom])
        {
            ++compte[besoin];
        }
    }

    std::set<std::string> candidats;
    for (const auto &[nom, c] : compte)
    {
        if (c == 0)
        {
            candidats.insert(nom);
        }
    }

    std::vector<std::string> ordre;
    while (!candidats.empty())
    {
        std::string sorti = *candidats.begin();
        candidats.erase(candidats.begin());
        ordre.push_back(sorti);
        for (const std::string &besoin : besoins[sorti])
        {
            if (--compte[besoin] == 0)
            {
                candidats.insert(besoin);
            }
        }
    }

    if (ordre.size() < liste.size())
    {
        std::cout << "CYCLE\n";
        return 0;
    }
    for (const std::string &nom : ordre)
    {
        std::cout << nom << '\n';
    }
    return 0;
}
