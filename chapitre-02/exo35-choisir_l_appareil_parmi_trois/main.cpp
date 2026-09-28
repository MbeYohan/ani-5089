#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Appareil
{
    std::string serie;
    std::string etat;
};

static bool LireMots(std::vector<std::string> &mots)
{
    std::string ligne;
    while (std::getline(std::cin, ligne))
    {
        std::istringstream flux(ligne);
        mots.clear();
        std::string mot;
        while (flux >> mot)
        {
            mots.push_back(mot);
        }
        if (!mots.empty())
        {
            return true;
        }
    }
    return false;
}

int main()
{
    std::vector<std::string> mots;
    int d = 0;
    if (LireMots(mots))
    {
        d = std::stoi(mots[0]);
    }

    std::vector<Appareil> appareils;
    for (int i = 0; i < d && LireMots(mots); ++i)
    {
        Appareil a;
        a.serie = mots[0];
        a.etat = mots.size() > 1 ? mots[1] : "";
        appareils.push_back(a);
    }

    std::string cible = "-";
    if (LireMots(mots))
    {
        cible = mots[0];
    }

    if (cible != "-")
    {
        for (const Appareil &a : appareils)
        {
            if (a.serie == cible)
            {
                if (a.etat == "device")
                {
                    std::cout << a.serie << '\n';
                }
                else
                {
                    std::cout << "ERREUR " << a.serie << " est " << a.etat << '\n';
                }
                return 0;
            }
        }
        std::cout << "ERREUR cible introuvable\n";
        return 0;
    }

    std::vector<std::string> prets;
    for (const Appareil &a : appareils)
    {
        if (a.etat == "device")
        {
            prets.push_back(a.serie);
        }
    }

    if (prets.empty())
    {
        std::cout << "ERREUR aucun appareil\n";
    }
    else if (prets.size() == 1)
    {
        std::cout << prets[0] << '\n';
    }
    else
    {
        std::sort(prets.begin(), prets.end());
        std::cout << "ERREUR plusieurs appareils\n";
        for (const std::string &serie : prets)
        {
            std::cout << serie << '\n';
        }
    }
    return 0;
}
