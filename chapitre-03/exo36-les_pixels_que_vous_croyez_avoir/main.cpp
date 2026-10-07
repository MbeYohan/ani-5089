#include <iostream>

int main()
{
    int n = 0;
    std::cin >> n;
    int lisibles = 0;
    for (int i = 0; i < n; ++i)
    {
        long long largeur = 0, hauteur = 0, echelle = 0, champ = 0;
        std::cin >> largeur >> hauteur >> echelle >> champ;
        long long largeurReelle = largeur * echelle / 100;
        long long hauteurReelle = hauteur * echelle / 100;
        long long parDegre = (largeurReelle + champ / 2) / champ;
        if (parDegre >= 15)
        {
            ++lisibles;
        }
        std::cout << largeurReelle << ' ' << hauteurReelle << ' ' << parDegre << '\n';
    }
    std::cout << "LISIBLE " << lisibles << '\n';
    return 0;
}
