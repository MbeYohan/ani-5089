#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    int c = 0;
    std::cin >> c;
    std::vector<long long> echelle(c), seuil(c);
    for (int i = 0; i < c; ++i)
    {
        std::string nom;
        std::cin >> nom >> echelle[i] >> seuil[i];
    }
    int t = 0;
    std::cin >> t;
    for (int tour = 0; tour < t; ++tour)
    {
        long long axe = 0;
        for (int i = 0; i < c; ++i)
        {
            long long brut = 0;
            std::cin >> brut;
            long long contribution = brut * echelle[i] / 1000;
            if (std::llabs(contribution) < seuil[i])
            {
                continue;
            }
            axe += contribution;
        }
        std::cout << axe << '\n';
    }
    return 0;
}
