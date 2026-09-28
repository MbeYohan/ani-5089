#include <iostream>
#include <string>

static bool CommencePar(const std::string &texte, const std::string &debut)
{
    return texte.compare(0, debut.size(), debut) == 0 && texte.size() >= debut.size();
}

static bool FinitPar(const std::string &texte, const std::string &fin)
{
    return texte.size() >= fin.size() && texte.compare(texte.size() - fin.size(), fin.size(), fin) == 0;
}

static std::string Nettoyer(const std::string &texte)
{
    std::size_t debut = texte.find_first_not_of(" \t\r");
    if (debut == std::string::npos)
    {
        return "";
    }
    std::size_t fin = texte.find_last_not_of(" \t\r");
    return texte.substr(debut, fin - debut + 1);
}

static bool LireLigne(std::string &ligne)
{
    while (std::getline(std::cin, ligne))
    {
        ligne = Nettoyer(ligne);
        if (!ligne.empty())
        {
            return true;
        }
    }
    return false;
}

int main()
{
    std::string ligne;
    std::string abi;
    if (LireLigne(ligne))
    {
        abi = ligne;
    }
    int f = 0;
    if (LireLigne(ligne))
    {
        f = std::stoi(ligne);
    }

    const std::string dossierVoulu = "lib/" + abi + "/";

    unsigned long long total = 0;
    bool signe = false;
    bool abiPresente = false;
    int inutiles = 0;

    for (int i = 0; i < f && LireLigne(ligne); ++i)
    {
        std::size_t espace = ligne.find_last_of(" \t");
        std::string chemin = espace == std::string::npos ? ligne : Nettoyer(ligne.substr(0, espace));
        std::string taille = espace == std::string::npos ? "0" : ligne.substr(espace + 1);
        total += std::stoull(taille);

        if (CommencePar(chemin, "META-INF/") && (FinitPar(chemin, ".RSA") || FinitPar(chemin, ".DSA") || FinitPar(chemin, ".EC")))
        {
            signe = true;
        }
        if (CommencePar(chemin, dossierVoulu))
        {
            abiPresente = true;
        }
        else if (CommencePar(chemin, "lib/"))
        {
            ++inutiles;
        }
    }

    std::cout << total << '\n';
    std::cout << (signe ? "SIGNE" : "NON SIGNE") << '\n';
    std::cout << (abiPresente ? "ABI OUI" : "ABI NON") << '\n';
    std::cout << "INUTILE " << inutiles << '\n';
    return 0;
}
