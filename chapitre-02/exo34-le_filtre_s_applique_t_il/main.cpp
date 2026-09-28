#include <iostream>
#include <map>
#include <string>

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

static std::string Nettoyer(const std::string &texte)
{
    std::size_t debut = texte.find_first_not_of(" \t");
    if (debut == std::string::npos)
    {
        return "";
    }
    std::size_t fin = texte.find_last_not_of(" \t");
    return texte.substr(debut, fin - debut + 1);
}

static int LireEntier()
{
    std::string ligne;
    while (LireLigne(ligne))
    {
        if (!Nettoyer(ligne).empty())
        {
            return std::stoi(ligne);
        }
    }
    return 0;
}

static bool TermeVrai(std::string terme, const std::map<std::string, std::string> &machine)
{
    bool renverse = false;
    terme = Nettoyer(terme);
    while (!terme.empty() && terme[0] == '!')
    {
        renverse = !renverse;
        terme = Nettoyer(terme.substr(1));
    }

    bool vrai = false;
    std::size_t egal = terme.find('=');
    if (egal != std::string::npos)
    {
        auto trouve = machine.find(terme.substr(0, egal));
        vrai = trouve != machine.end() && trouve->second == terme.substr(egal + 1);
    }
    return renverse ? !vrai : vrai;
}

int main()
{
    std::map<std::string, std::string> machine;
    int v = LireEntier();
    for (int i = 0; i < v; ++i)
    {
        std::string ligne;
        if (!LireLigne(ligne))
        {
            break;
        }
        ligne = Nettoyer(ligne);
        std::size_t egal = ligne.find('=');
        if (egal != std::string::npos)
        {
            machine[ligne.substr(0, egal)] = ligne.substr(egal + 1);
        }
    }

    int f = LireEntier();
    for (int i = 0; i < f; ++i)
    {
        std::string condition;
        if (!LireLigne(condition))
        {
            break;
        }
        bool applique = true;
        std::size_t debut = 0;
        while (true)
        {
            std::size_t et = condition.find("&&", debut);
            std::string terme = condition.substr(debut, et == std::string::npos ? std::string::npos : et - debut);
            if (!TermeVrai(terme, machine))
            {
                applique = false;
            }
            if (et == std::string::npos)
            {
                break;
            }
            debut = et + 2;
        }
        std::cout << (applique ? "OUI" : "NON") << '\n';
    }
    return 0;
}
