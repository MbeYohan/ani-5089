#include <iostream>
#include <string>

int main()
{
    long long hauteur;
    int n;
    std::cin >> hauteur >> n;
    int decalees = 0;
    for (int i = 0; i < n; i++)
    {
        std::string nom, pose, couche;
        std::cin >> nom >> pose >> couche;
        long long decalage = 0;
        std::string verdict = "OK";
        if (pose == "STAGE" && couche == "LOCAL")
        {
            decalage = hauteur;
            verdict = "LEVITATION";
        }
        else if (pose == "LOCAL" && couche == "STAGE")
        {
            decalage = -hauteur;
            verdict = "ENTERRE";
        }
        if (verdict != "OK")
            decalees++;
        std::cout << nom << " " << decalage << " " << verdict << '\n';
    }
    std::cout << "DECALEES " << decalees << '\n';
}
