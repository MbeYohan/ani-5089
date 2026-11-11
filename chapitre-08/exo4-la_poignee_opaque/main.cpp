#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    int images, n;
    std::cin >> images >> n;
    std::vector<std::uint64_t> valeur(images, 0);
    std::vector<bool> posee(images, false);
    int nbPosees = 0;
    for (int i = 0; i < n; ++i)
    {
        std::string commande;
        long long index;
        std::cin >> commande >> index;
        bool dansAnneau = index >= 0 && index < images;
        if (commande == "POSE")
        {
            std::uint64_t v;
            std::cin >> v;
            if (!dansAnneau)
            {
                std::cout << "HORS ANNEAU\n";
            }
            else
            {
                if (!posee[index])
                    ++nbPosees;
                posee[index] = true;
                valeur[index] = v;
            }
        }
        else
        {
            if (dansAnneau && posee[index])
                std::cout << valeur[index] << '\n';
            else
                std::cout << "VIDE\n";
        }
    }
    std::cout << "POSEES " << nbPosees << '\n';
    return 0;
}
