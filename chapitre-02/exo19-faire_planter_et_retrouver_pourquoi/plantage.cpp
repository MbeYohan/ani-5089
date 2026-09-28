#include <cstdio>

#ifdef __ANDROID__
#include <android/log.h>
#endif

int *volatile gHauteurPlafond = nullptr;

__attribute__((noinline)) int LireHauteurPlafond()
{
    return *gHauteurPlafond;
}

#ifdef __ANDROID__

extern "C" void android_main(void *)
{
    __android_log_print(ANDROID_LOG_INFO, "MaSalle", "lecture de la hauteur du plafond");
    int hauteur = LireHauteurPlafond();
    __android_log_print(ANDROID_LOG_INFO, "MaSalle", "hauteur %d", hauteur);
}

#else

int main()
{
    std::printf("lecture de la hauteur du plafond\n");
    std::fflush(stdout);
    int hauteur = LireHauteurPlafond();
    std::printf("hauteur %d\n", hauteur);
    return 0;
}

#endif
