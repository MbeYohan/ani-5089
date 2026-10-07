#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkMouseEvent.h"
#include "NKEvent/NkKeyboardEvent.h"

#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

using namespace nkentseu;

namespace
{

    struct Couple
    {
        int x;
        int y;
    };

    struct Accumulateur
    {
        long long x = 0;
        long long y = 0;

        void Ajouter(int dx, int dy)
        {
            x += dx;
            y += dy;
        }

        Couple Consommer()
        {
            Couple total{static_cast<int>(x), static_cast<int>(y)};
            x = 0;
            y = 0;
            return total;
        }
    };

    void Ecrire(const char *chemin, const std::vector<Couple> &lignes)
    {
        std::FILE *f = std::fopen(chemin, "w");
        if (!f)
            return;
        for (const Couple &c : lignes)
        {
            std::fprintf(f, "%d %d\n", c.x, c.y);
        }
        std::fclose(f);
    }

}

int nkmain(const NkEntryState &)
{
    NkWindowConfig config;
    config.title = "Accumulateur : Espace, bougez vite, arretez-vous";
    config.width = 800;
    config.height = 450;

    NkWindow fenetre(config);
    if (!fenetre.IsValid())
    {
        return 1;
    }

    Accumulateur accumulateur;
    auto garde = NkEvents().AddEventCallbackGuard<NkMouseRawEvent>(
        [&](NkMouseRawEvent *e)
        { accumulateur.Ajouter(e->GetDeltaX(), e->GetDeltaY()); });

    bool enregistre = false;
    auto gardeTouche = NkEvents().AddEventCallbackGuard<NkKeyPressEvent>(
        [&](NkKeyPressEvent *e)
        {
            if (e->GetKey() == NkKey::NK_SPACE)
                enregistre = true;
        });

    std::vector<Couple> avant, apres;
    const std::size_t images = 10;

    while (fenetre.IsOpen() && (avant.size() < images))
    {
        NkEvents().PollEvents();

        const auto &souris = NkEvents().GetInputState().mouse;
        Couple direct{souris.rawDeltaX, souris.rawDeltaY};
        Couple somme = accumulateur.Consommer();

        if (enregistre)
        {
            avant.push_back(direct);
            apres.push_back(somme);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    Ecrire("avant.txt", avant);
    Ecrire("apres.txt", apres);
    return 0;
}
