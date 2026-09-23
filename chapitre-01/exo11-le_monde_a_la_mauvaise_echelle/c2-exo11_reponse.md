## Code

#include <iostream>
#include <iomanip>
#include <string>

struct Objet
{
    const char *nom;
    double largeur;
    double hauteur;
    double profondeur;
};

int main()
{
    const Objet salle[] = {
        {"salle", 4.00, 2.50, 3.50},
        {"porte", 0.90, 2.00, 0.05},
        {"fenetre", 1.20, 1.10, 0.05},
        {"table", 1.40, 0.75, 0.80},
        {"chaise", 0.45, 0.90, 0.45},
        {"tasse", 0.08, 0.10, 0.08}};

    double facteur = 1.0;
    if (!(std::cin >> facteur))
    {
        return 1;
    }

    std::cout << std::fixed << std::setprecision(2);
    for (const Objet &o : salle)
    {
        std::cout << o.nom << ' '
                  << o.largeur * facteur << ' '
                  << o.hauteur * facteur << ' '
                  << o.profondeur * facteur << '\n';
    }
    return 0;
}


## Exemple d'exécution

Entrée : 2.5

Sortie :
salle 10.00 6.25 8.75
porte 2.25 5.00 0.12
fenetre 3.00 2.75 0.12
table 3.50 1.88 2.00
chaise 1.12 2.25 1.12
tasse 0.20 0.25 0.20

## Descriptions

Facteur = 2.5
Un ami "La salle est trop grande, je me sens tout petit, en faite ca fait peur, je ne suis pas habitué"

Facteur = 0.5
Une amie "la plafond est tres bas, etouffant meme, c'est petit, le meuble est minuscule"

Facteur = 1
Un ami "ca me fait penser a une salle de reception, ca ma l'aire quand meme grand pour le mobilier"
