#include <iostream>
#include <string>

int main()
{
    int vol, n;
    std::cin >> vol >> n;
    int enVol = 0;
    int premierBlocage = 0;
    for (int tour = 1; tour <= n; ++tour)
    {
        std::string commande;
        std::cin >> commande;
        bool attend = false;
        if (commande == "SOUMET")
        {
            if (enVol >= vol)
                attend = true;
            else
                ++enVol;
        }
        else
        {
            if (enVol > 0)
                --enVol;
        }
        if (attend && premierBlocage == 0)
            premierBlocage = tour;
        std::cout << tour << ' ' << commande << ' ' << enVol << ' '
                  << (attend ? "ATTEND" : "PASSE") << '\n';
    }
    std::cout << "BLOQUE " << premierBlocage << '\n';
    return 0;
}
