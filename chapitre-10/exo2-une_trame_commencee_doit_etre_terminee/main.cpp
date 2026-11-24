#include <iostream>
#include <string>

enum Etat
{
    REPOS,
    ATTENDUE,
    COMMENCEE
};

int main()
{
    int n;
    std::cin >> n;
    Etat etat = REPOS;
    int fautes = 0, bloque = 0;
    for (int i = 1; i <= n; i++)
    {
        std::string geste;
        std::cin >> geste;
        std::string verdict = "OK";
        if (geste == "ATTEND")
        {
            if (etat == REPOS)
                etat = ATTENDUE;
            else if (etat == ATTENDUE)
                verdict = "DEJA ATTENDUE";
            else
                verdict = "TRAME EN COURS";
        }
        else if (geste == "COMMENCE")
        {
            if (etat == ATTENDUE)
                etat = COMMENCEE;
            else if (etat == REPOS)
                verdict = "PAS ATTENDUE";
            else
                verdict = "DEJA COMMENCEE";
        }
        else if (geste == "FINIT")
        {
            if (etat == COMMENCEE)
                etat = REPOS;
            else
                verdict = "PAS COMMENCEE";
        }
        if (verdict != "OK")
        {
            fautes++;
            if (verdict == "TRAME EN COURS" && bloque == 0)
                bloque = i;
        }
        std::cout << i << " " << geste << " " << verdict << '\n';
    }
    std::cout << "FAUTES " << fautes << '\n';
    std::cout << "BLOQUE " << bloque << '\n';
}
