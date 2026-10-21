#include "NKAudio/NkAudio.h"
#include <chrono>
#include <cstdio>
#include <thread>

using namespace nkentseu::audio;

int main()
{
    AudioEngineConfig cfg;
    cfg.sampleRate = 48000;
    AudioEngine &moteur = AudioEngine::Instance();
    if (!moteur.Initialize(cfg))
    {
        std::printf("initialisation du moteur de son impossible\n");
        return 1;
    }
    std::printf("dorsal : %s\n", moteur.GetBackendName());
    std::printf("frequence : %d\n", moteur.GetSampleRate());
    std::printf("tampon : %d\n", moteur.GetBufferSize());
    std::printf("latence : %.2f\n", moteur.GetLatencyMs());

    AudioSample son = AudioGenerator::GenerateTone(440.f, 1.f);
    moteur.Play(son);

    for (int image = 0; image < 180; ++image)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(11));
    }

    moteur.Shutdown();
    AudioLoader::Free(son);
    return 0;
}
