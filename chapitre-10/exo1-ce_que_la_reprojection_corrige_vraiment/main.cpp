#include <cstdlib>
#include <iostream>
#include <string>

int main()
{
    int n;
    std::cin >> n;
    int justes = 0, pires = 0;
    for (int i = 0; i < n; i++)
    {
        long long rendu, reel, soumis;
        std::cin >> rendu >> reel >> soumis;
        long long sans = rendu - reel;
        long long avec = rendu - soumis;
        std::string verdict;
        if (avec == 0)
        {
            verdict = "JUSTE";
            justes++;
        }
        else if (std::llabs(avec) < std::llabs(sans))
        {
            verdict = "PARTIEL";
        }
        else if (std::llabs(avec) == std::llabs(sans))
        {
            verdict = "SANS EFFET";
        }
        else
        {
            verdict = "PIRE";
            pires++;
        }
        std::cout << "SANS " << sans << " AVEC " << avec << " " << verdict << '\n';
    }
    std::cout << "JUSTES " << justes << '\n';
    std::cout << "PIRES " << pires << '\n';
}
