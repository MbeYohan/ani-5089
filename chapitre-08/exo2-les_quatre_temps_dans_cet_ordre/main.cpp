#include <iostream>
#include <string>

int main()
{
    int n;
    std::cin >> n;
    bool acquise = false;
    bool attendSoumission = false;
    int fautes = 0;
    for (int tour = 1; tour <= n; ++tour)
    {
        std::string appel;
        std::cin >> appel;
        std::string verdict = "OK";
        if (appel == "ACQUIERT")
        {
            if (acquise)
                verdict = "DEJA ACQUISE";
            else
                acquise = true;
        }
        else if (appel == "DESSINE")
        {
            if (!acquise)
                verdict = "PAS ACQUISE";
        }
        else if (appel == "REND")
        {
            if (!acquise)
            {
                verdict = "RIEN ACQUIS";
            }
            else
            {
                acquise = false;
                attendSoumission = true;
            }
        }
        else if (appel == "SOUMET")
        {
            if (!attendSoumission)
                verdict = "PAS RENDUE";
            else
                attendSoumission = false;
        }
        if (verdict != "OK")
            ++fautes;
        std::cout << tour << ' ' << appel << ' ' << verdict << '\n';
    }
    std::cout << "FAUTES " << fautes << '\n';
    return 0;
}
