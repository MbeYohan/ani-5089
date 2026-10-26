#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKMath/NkAngle.h"
#include "NKMath/NkQuat.h"
#include <cmath>
#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::math;

static void ConfigureAppData(NkAppData &d)
{
	d.appName = "S6Exo8";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

struct PoseTete
{
	const char *nom;
	NkQuatf orientation;
};

struct Yeux
{
	NkVec3f gauche;
	NkVec3f droite;
};

static const NkVec3f kPositionTete(0.30f, 1.70f, 1.20f);
static const float kEcartPupilles = 0.063f;

static float Propre(float v)
{
	return std::fabs(v) < 0.00005f ? 0.f : v;
}

static Yeux YeuxSelon(const NkVec3f &axe)
{
	const float demiEcart = kEcartPupilles * 0.5f;
	Yeux y;
	y.gauche = kPositionTete - axe * demiEcart;
	y.droite = kPositionTete + axe * demiEcart;
	return y;
}

static void Ligne(std::FILE *f, const char *pose, const char *methode, const Yeux &y)
{
	char tampon[200];
	std::snprintf(tampon, sizeof(tampon), "%s, %s : G (%.4f, %.4f, %.4f) D (%.4f, %.4f, %.4f)\n", pose, methode,
				  Propre(y.gauche.x), Propre(y.gauche.y), Propre(y.gauche.z), Propre(y.droite.x),
				  Propre(y.droite.y), Propre(y.droite.z));
	std::fputs(tampon, f);
	std::fputs(tampon, stdout);
}

int nkmain(const NkEntryState &state)
{
	(void)state;

	const PoseTete poses[3] = {
		{"neutre", NkQuatf::Identity()},
		{"roulis 30", NkQuatf::RotateZ(NkAngle(30.f))},
		{"lacet 90", NkQuatf::RotateY(NkAngle(90.f))},
	};

	std::FILE *f = std::fopen("poses.txt", "w");
	if (!f)
	{
		return 1;
	}

	float ecartsMm[3];
	for (int i = 0; i < 3; i++)
	{
		const NkVec3f droiteTete = poses[i].orientation * NkVec3f(1.f, 0.f, 0.f);
		const NkVec3f droiteMonde(1.f, 0.f, 0.f);

		const Yeux tete = YeuxSelon(droiteTete);
		const Yeux monde = YeuxSelon(droiteMonde);
		Ligne(f, poses[i].nom, "tete", tete);
		Ligne(f, poses[i].nom, "monde", monde);

		const NkVec3f d = tete.gauche - monde.gauche;
		ecartsMm[i] = std::sqrt(d.x * d.x + d.y * d.y + d.z * d.z) * 1000.f;
	}

	for (int i = 0; i < 3; i++)
	{
		std::fprintf(f, "%s : ecart %.2f mm\n", poses[i].nom, ecartsMm[i]);
		std::printf("%s : ecart %.2f mm\n", poses[i].nom, ecartsMm[i]);
	}
	std::fclose(f);
	return 0;
}
