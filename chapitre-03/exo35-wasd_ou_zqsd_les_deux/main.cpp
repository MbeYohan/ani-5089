#include <iostream>
#include <sstream>
#include <string>

int main()
{
    int n = 0;
    std::cin >> n;
    std::string ligne;
    std::getline(std::cin, ligne);
    for (int i = 0; i < n; ++i)
    {
        std::getline(std::cin, ligne);
        std::istringstream flux(ligne);
        bool avant = false, arriere = false, gauche = false, droite = false;
        std::string touche;
        while (flux >> touche)
        {
            if (touche == "W" || touche == "Z")
                avant = true;
            else if (touche == "S")
                arriere = true;
            else if (touche == "A" || touche == "Q")
                gauche = true;
            else if (touche == "D")
                droite = true;
        }
        int avance = (avant ? 1 : 0) - (arriere ? 1 : 0);
        int cote = (droite ? 1 : 0) - (gauche ? 1 : 0);
        std::cout << avance << ' ' << cote << '\n';
    }
    return 0;
}
