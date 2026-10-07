#include <iostream>
#include <string>

int main()
{
    int p = 0, f = 0;
    std::cin >> p >> f;
    int sansReleve = 0;
    int premierMort = 0;
    for (int tour = 1; tour <= f; ++tour)
    {
        std::string action;
        std::cin >> action;
        bool morte = false;
        if (action == "releve")
        {
            sansReleve = 0;
        }
        else
        {
            ++sansReleve;
            morte = sansReleve >= p;
        }
        if (morte && premierMort == 0)
        {
            premierMort = tour;
        }
        std::cout << sansReleve << (morte ? " MORTE" : " VIVANTE") << '\n';
    }
    std::cout << "PREMIER " << premierMort << '\n';
    return 0;
}
