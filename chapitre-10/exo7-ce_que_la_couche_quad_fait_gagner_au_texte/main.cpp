#include <iostream>
#include <string>

int main()
{
    long long r1, r2, r3, seuil;
    int n;
    std::cin >> r1 >> r2 >> r3 >> seuil >> n;
    int gagnes = 0, perdus = 0;
    for (int i = 0; i < n; i++)
    {
        std::string nom;
        long long pixels;
        std::cin >> nom >> pixels;
        long long scene = pixels * r1 / 1000;
        scene = scene * r2 / 1000;
        scene = scene * r3 / 1000;
        long long quad = pixels;
        bool lisibleScene = scene >= seuil;
        bool lisibleQuad = quad >= seuil;
        if (!lisibleScene && lisibleQuad)
            gagnes++;
        if (lisibleScene && !lisibleQuad)
            perdus++;
        std::cout << nom << " " << scene << " " << quad << " "
                  << (lisibleScene ? "LISIBLE" : "FLOU") << " "
                  << (lisibleQuad ? "LISIBLE" : "FLOU") << '\n';
    }
    std::cout << "GAGNES " << gagnes << '\n';
    std::cout << "PERDUS " << perdus << '\n';
}
