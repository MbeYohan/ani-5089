#include <iostream>
#include <string>

int main()
{
    int images, n;
    std::cin >> images >> n;
    int suivant = 0;
    bool emprunte = false;
    int prets = 0;
    for (int tour = 1; tour <= n; ++tour)
    {
        std::string commande;
        std::cin >> commande;
        std::cout << tour << ' ' << commande << ' ';
        if (commande == "EMPRUNTE")
        {
            if (emprunte)
            {
                std::cout << "REFUSE\n";
            }
            else
            {
                emprunte = true;
                ++prets;
                std::cout << suivant << '\n';
            }
        }
        else
        {
            if (!emprunte)
            {
                std::cout << "REFUSE\n";
            }
            else
            {
                emprunte = false;
                std::cout << suivant << '\n';
                suivant = (suivant + 1) % images;
            }
        }
    }
    std::cout << "PRETS " << prets << '\n';
    return 0;
}
