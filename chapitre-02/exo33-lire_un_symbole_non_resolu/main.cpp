#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

static bool LireLigne(std::string &ligne)
{
    if (!std::getline(std::cin, ligne))
    {
        return false;
    }
    if (!ligne.empty() && ligne.back() == '\r')
    {
        ligne.pop_back();
    }
    return true;
}

static int LireEntier()
{
    std::string ligne;
    while (LireLigne(ligne))
    {
        if (ligne.find_first_not_of(" \t") != std::string::npos)
        {
            return std::stoi(ligne);
        }
    }
    return 0;
}

int main()
{
    std::vector<std::pair<std::string, std::string>> prefixes;
    int p = LireEntier();
    for (int i = 0; i < p; ++i)
    {
        std::string ligne;
        if (!LireLigne(ligne))
        {
            break;
        }
        std::istringstream flux(ligne);
        std::string prefixe;
        std::string module;
        if (flux >> prefixe >> module)
        {
            prefixes.emplace_back(prefixe, module);
        }
        else
        {
            --i;
        }
    }

    const std::string marque = "undefined reference to '";
    std::set<std::string> modules;
    std::set<std::string> inconnus;

    int l = LireEntier();
    for (int i = 0; i < l; ++i)
    {
        std::string ligne;
        if (!LireLigne(ligne))
        {
            break;
        }
        std::size_t debut = ligne.find(marque);
        if (debut == std::string::npos)
        {
            continue;
        }
        debut += marque.size();
        std::size_t fin = ligne.find('\'', debut);
        std::string symbole = ligne.substr(debut, fin == std::string::npos ? std::string::npos : fin - debut);

        std::size_t meilleur = 0;
        std::string module;
        for (const auto &[prefixe, nom] : prefixes)
        {
            if (prefixe.size() > meilleur && symbole.compare(0, prefixe.size(), prefixe) == 0)
            {
                meilleur = prefixe.size();
                module = nom;
            }
        }
        if (meilleur > 0)
        {
            modules.insert(module);
        }
        else
        {
            inconnus.insert(symbole);
        }
    }

    for (const std::string &nom : modules)
    {
        std::cout << nom << '\n';
    }
    if (!inconnus.empty())
    {
        std::cout << "INCONNU " << inconnus.size() << '\n';
    }
    return 0;
}
