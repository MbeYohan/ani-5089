#include <iostream>
#include <sstream>
#include <string>

int main()
{
    int n;
    std::cin >> n;
    std::string reste;
    std::getline(std::cin, reste);
    int survivants = 0;
    for (int i = 0; i < n; ++i)
    {
        std::string ligne;
        std::getline(std::cin, ligne);
        std::istringstream mots(ligne);
        std::string nom, mot;
        mots >> nom;
        int rang = 0;
        int rangDetruit = 0;
        int rangRecree = 0;
        bool reenregistre = false;
        while (mots >> mot)
        {
            ++rang;
            if (mot == "DETRUIT" && rangDetruit == 0)
                rangDetruit = rang;
            if (mot == "RECREE" && rangRecree == 0)
                rangRecree = rang;
            if (mot == "REENREGISTRE")
                reenregistre = true;
        }
        std::string verdict;
        if (rangDetruit != 0 && rangRecree != 0 && rangRecree < rangDetruit)
        {
            verdict = "MAUVAIS ORDRE";
        }
        else if (rangDetruit == 0 || rangRecree == 0)
        {
            verdict = "CHAINE PERIMEE";
        }
        else if (!reenregistre)
        {
            verdict = "POIGNEES PERDUES";
        }
        else
        {
            verdict = "SURVIT";
            ++survivants;
        }
        std::cout << nom << ' ' << verdict << '\n';
    }
    std::cout << "SURVIVANTS " << survivants << '\n';
    return 0;
}
