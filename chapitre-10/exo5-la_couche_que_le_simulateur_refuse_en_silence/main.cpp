#include <iostream>
#include <string>

int main()
{
    int n;
    std::cin >> n;
    int refusees = 0, averties = 0;
    for (int i = 0; i < n; i++)
    {
        std::string nom;
        int commence, rend, gauche, droite;
        std::cin >> nom >> commence >> rend >> gauche >> droite;
        std::string verdict = "OK";
        if (commence == 0)
        {
            verdict = "REFUSEE";
            refusees++;
        }
        else
        {
            if (rend == 0)
            {
                if (gauche == 1 || droite == 1)
                    verdict = "COUCHE SANS RENDU";
            }
            else if (gauche == 0 && droite == 0)
            {
                verdict = "SOUMISSION VIDE";
            }
            else if (gauche == 0)
            {
                verdict = "OEIL NOIR G";
            }
            else if (droite == 0)
            {
                verdict = "OEIL NOIR D";
            }
            if (verdict != "OK")
                averties++;
        }
        std::cout << nom << " " << verdict << '\n';
    }
    std::cout << "REFUSEES " << refusees << '\n';
    std::cout << "AVERTIES " << averties << '\n';
}
