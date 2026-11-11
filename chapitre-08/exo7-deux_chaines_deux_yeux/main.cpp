#include <cstdlib>
#include <iostream>
#include <string>

struct Chaine
{
    int suivant = 0;
    bool emprunte = false;
    int rendus = 0;
};

int main()
{
    int images, n;
    std::cin >> images >> n;
    Chaine chaine[2];
    int desync = 0;
    for (int tour = 1; tour <= n; ++tour)
    {
        std::string oeil, commande;
        std::cin >> oeil >> commande;
        Chaine &c = chaine[oeil == "G" ? 0 : 1];
        std::cout << tour << ' ' << oeil << ' ' << commande << ' ';
        if (commande == "EMPRUNTE")
        {
            if (c.emprunte)
            {
                std::cout << "REFUSE\n";
            }
            else
            {
                c.emprunte = true;
                std::cout << c.suivant << '\n';
            }
        }
        else
        {
            if (!c.emprunte)
            {
                std::cout << "REFUSE\n";
            }
            else
            {
                c.emprunte = false;
                ++c.rendus;
                std::cout << c.suivant << '\n';
                c.suivant = (c.suivant + 1) % images;
            }
        }
        int ecart = std::abs(chaine[0].rendus - chaine[1].rendus);
        if (ecart > 1 && desync == 0)
            desync = tour;
    }
    std::cout << "DECALAGE " << std::abs(chaine[0].rendus - chaine[1].rendus) << '\n';
    std::cout << "DESYNC " << desync << '\n';
    return 0;
}
