#include <iostream>
#include <string>

int main()
{
    long long budget = 0;
    int s = 0;
    std::cin >> budget >> s;

    int trompe = 0;
    for (int i = 0; i < s; ++i)
    {
        std::string nom;
        long long debug = 0;
        long long release = 0;
        if (!(std::cin >> nom >> debug >> release))
        {
            break;
        }

        long long facteur = release > 0 ? (debug + release / 2) / release : 0;

        bool tientRelease = release <= budget;
        bool tientDebug = debug <= budget;
        if (tientRelease && !tientDebug)
        {
            ++trompe;
        }

        std::cout << nom << ' ' << facteur << ' ' << (tientRelease ? "TIENT" : "DEPASSE") << '\n';
    }
    std::cout << "TROMPE " << trompe << '\n';
    return 0;
}
