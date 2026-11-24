#include <iostream>

int main()
{
    int images, trames;
    std::cin >> images >> trames;
    int index = 0, rendues = 0;
    for (int t = 1; t <= trames; t++)
    {
        int dessine;
        std::cin >> dessine;
        if (dessine == 1)
        {
            std::cout << t << " OUI 1 " << index << '\n';
            rendues++;
            if (images > 0)
                index = (index + 1) % images;
        }
        else
        {
            std::cout << t << " NON 0 -\n";
        }
    }
    std::cout << "RENDUES " << rendues << '\n';
    std::cout << "FINS " << trames << '\n';
}
