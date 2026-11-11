#include <iostream>
#include <string>

int main()
{
    int largeur, hauteur, n;
    std::cin >> largeur >> hauteur >> n;
    int bloquantes = 0;
    for (int i = 0; i < n; ++i)
    {
        std::string nom;
        int cible, taille, lc, hc;
        std::cin >> nom >> cible >> taille >> lc >> hc;
        int l = largeur;
        int h = hauteur;
        std::string verdict;
        if (cible == 0)
        {
            verdict = "PAS RELIEE";
        }
        else if (taille == 1)
        {
            l = lc;
            h = hc;
            verdict = "OK";
        }
        else if (largeur != lc || hauteur != hc)
        {
            verdict = "BLOQUE";
            ++bloquantes;
        }
        else
        {
            verdict = "SANS EFFET";
        }
        std::cout << nom << ' ' << l << ' ' << h << ' ' << verdict << '\n';
    }
    std::cout << "BLOQUANTES " << bloquantes << '\n';
    return 0;
}
