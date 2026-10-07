#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"

using namespace nkentseu;

int nkmain(const NkEntryState &)
{
    NkWindowConfig config;
    config.title = "Ma salle";
    config.width = 1280;
    config.height = 720;

    NkWindow fenetre(config);
    if (!fenetre.IsValid())
    {
        return 1;
    }

    while (fenetre.IsOpen())
    {
        NkEvents().PollEvents();
    }
    return 0;
}
