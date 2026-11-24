#include <cstdio>
#include "couches.h"

const int IMAGES = 3;
const int TRAMES = 6;

struct Session
{
    bool ouverte = false;
    int debuts = 0;
    int fins = 0;

    void attendreTrame() {}
    void commencerTrame()
    {
        ouverte = true;
        debuts++;
    }
    bool finirTrame(int nbCouches, const CoucheProjection *couche)
    {
        if (!ouverte)
            return false;
        (void)nbCouches;
        (void)couche;
        ouverte = false;
        fins++;
        return true;
    }
};

int main()
{
    Session session;
    int index = 0;
    int rendues = 0;

    std::printf("trame rend couches index\n");
    for (int trame = 1; trame <= TRAMES; trame++)
    {
        bool onRend = (trame != 3 && trame != 4);
        double lacetTete = 1.0 * trame;

        session.attendreTrame();
        session.commencerTrame();

        CoucheProjection couche;
        int nbCouches = 0;
        int indexEmprunte = -1;

        if (onRend)
        {
            indexEmprunte = index;
            index = (index + 1) % IMAGES;

            couche.espace = Espace::STAGE;
            for (int oeil = 0; oeil < 2; oeil++)
            {
                couche.vues[oeil] = {oeil, indexEmprunte, lacetTete, 90.0};
            }
            nbCouches = 1;
            rendues++;
        }

        session.finirTrame(nbCouches, nbCouches ? &couche : nullptr);

        if (onRend)
            std::printf("%d OUI %d %d\n", trame, nbCouches, indexEmprunte);
        else
            std::printf("%d NON %d -\n", trame, nbCouches);
    }
    std::printf("RENDUES %d\n", rendues);
    std::printf("FINS %d\n", session.fins);
}
