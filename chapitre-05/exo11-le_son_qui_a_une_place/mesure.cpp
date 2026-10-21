#include "NKAudio/NkAudio.h"
#include <cmath>
#include <cstdio>
#include <vector>

using namespace nkentseu::audio;

static const int kTrames = 48000;
static const int kBloc = 256;

static void MesurerOreilles(AudioEngine &moteur, double &rmsG, double &rmsD)
{
    std::vector<float> bloc(kBloc * 2);
    double sG = 0.0, sD = 0.0;
    int reste = kTrames;
    while (reste > 0)
    {
        int n = reste < kBloc ? reste : kBloc;
        moteur.RenderToBuffer(bloc.data(), n, 2);
        for (int i = 0; i < n; ++i)
        {
            sG += (double)bloc[2 * i] * bloc[2 * i];
            sD += (double)bloc[2 * i + 1] * bloc[2 * i + 1];
        }
        reste -= n;
    }
    rmsG = std::sqrt(sG / kTrames);
    rmsD = std::sqrt(sD / kTrames);
}

int main()
{
    AudioEngineConfig cfg;
    cfg.backend = AudioBackendType::NULL_OUTPUT;
    cfg.sampleRate = 48000;
    AudioEngine &moteur = AudioEngine::Instance();
    if (!moteur.Initialize(cfg))
    {
        std::printf("initialisation du moteur impossible\n");
        return 1;
    }
    moteur.SetListenerPosition(0.0f, 0.0f, 0.0f);
    moteur.SetListenerOrientation(0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    AudioSample pluie = AudioGenerator::GenerateNoise(4.f, WaveformType::NOISE_PINK);

    const char *noms[3] = {"gauche", "devant", "droite"};
    const float pos[3][3] = {{-2.f, 0.f, 0.f}, {0.f, 0.f, -2.f}, {2.f, 0.f, 0.f}};

    for (int i = 0; i < 3; ++i)
    {
        for (int positional = 1; positional >= 0; --positional)
        {
            VoiceParams p;
            p.looping = true;
            p.source3d.positional = (positional == 1);
            p.source3d.minDistance = 1.0f;
            p.source3d.maxDistance = 20.0f;
            p.source3d.position[0] = pos[i][0];
            p.source3d.position[1] = pos[i][1];
            p.source3d.position[2] = pos[i][2];
            AudioHandle h = moteur.Play(pluie, p);
            double g = 0.0, d = 0.0;
            MesurerOreilles(moteur, g, d);
            moteur.Stop(h);
            std::printf("%s %d : G %.4f D %.4f\n", noms[i], positional, g, d);
        }
    }

    moteur.Shutdown();
    AudioLoader::Free(pluie);
    return 0;
}
