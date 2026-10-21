#include "NKAudio/NkAudio.h"
#include <cmath>
#include <cstdio>
#include <vector>

using namespace nkentseu::audio;

static const int kTrames = 48000;
static const int kBloc = 256;

static double MesurerRms(AudioEngine &moteur)
{
    std::vector<float> bloc(kBloc * 2);
    double somme = 0.0;
    long long compte = 0;
    int reste = kTrames;
    while (reste > 0)
    {
        int n = reste < kBloc ? reste : kBloc;
        moteur.RenderToBuffer(bloc.data(), n, 2);
        for (int i = 0; i < n * 2; ++i)
        {
            somme += (double)bloc[i] * bloc[i];
        }
        compte += (long long)n * 2;
        reste -= n;
    }
    return std::sqrt(somme / (double)compte);
}

static double MesurerUne(AudioEngine &moteur, const AudioSample &son,
                         AttenuationModel modele, float rolloff, float distance)
{
    VoiceParams p;
    p.looping = true;
    p.source3d.positional = true;
    p.source3d.minDistance = 1.0f;
    p.source3d.maxDistance = 100.0f;
    p.source3d.rolloffFactor = rolloff;
    p.source3d.attenuationModel = modele;
    p.source3d.position[0] = 0.0f;
    p.source3d.position[1] = 0.0f;
    p.source3d.position[2] = -distance;
    AudioHandle h = moteur.Play(son, p);
    double rms = MesurerRms(moteur);
    moteur.Stop(h);
    return rms;
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

    AudioSample son = AudioGenerator::GenerateTone(440.f, 2.f, WaveformType::SINE, 48000, 0.1f);

    const float distances[5] = {0.5f, 1.0f, 2.0f, 4.0f, 8.0f};
    const float rolloffs[2] = {1.0f, 2.0f};
    const AttenuationModel modeles[4] = {AttenuationModel::NONE, AttenuationModel::INVERSE,
                                         AttenuationModel::LINEAR, AttenuationModel::EXPONENTIAL};
    const char *noms[4] = {"NONE", "INVERSE", "LINEAR", "EXPONENTIAL"};

    for (int r = 0; r < 2; ++r)
    {
        double reference[5];
        for (int d = 0; d < 5; ++d)
        {
            reference[d] = MesurerUne(moteur, son, AttenuationModel::NONE, rolloffs[r], distances[d]);
        }
        for (int m = 0; m < 4; ++m)
        {
            for (int d = 0; d < 5; ++d)
            {
                double rms = (m == 0) ? reference[d]
                                      : MesurerUne(moteur, son, modeles[m], rolloffs[r], distances[d]);
                double gain = rms / reference[d];
                std::printf("%s, rolloff %d, %g m : %.3f\n", noms[m], (int)rolloffs[r], distances[d], gain);
            }
        }
    }

    moteur.Shutdown();
    AudioLoader::Free(son);
    return 0;
}
