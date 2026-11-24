#include <iostream>
#include <map>
#include <string>

int main()
{
    std::map<std::string, std::string> causes = {
        {"BLOQUE_AU_WAIT", "FIN DE TRAME"},
        {"PAS_DE_REPRISE", "FIN SANS RENDU"},
        {"OEIL_NOIR", "CHAINE MANQUANTE"},
        {"TREMBLEMENT", "MAUVAIS INSTANT"},
        {"CORRECTION_DE_TRAVERS", "POSE FRAICHE"},
        {"SCENE_DECALEE", "MAUVAIS ESPACE"},
    };
    int n;
    std::cin >> n;
    int diagnostiques = 0, inconnus = 0;
    for (int i = 0; i < n; i++)
    {
        std::string symptome;
        std::cin >> symptome;
        auto it = causes.find(symptome);
        if (it == causes.end())
        {
            std::cout << symptome << " INCONNU\n";
            inconnus++;
        }
        else
        {
            std::cout << symptome << " " << it->second << '\n';
            diagnostiques++;
        }
    }
    std::cout << "DIAGNOSTIQUES " << diagnostiques << '\n';
    std::cout << "INCONNUS " << inconnus << '\n';
}
