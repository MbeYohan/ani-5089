#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKMath/NkAngle.h"
#include "NKMath/NkMat.h"
#include "NKMath/NkQuat.h"
#include <cmath>
#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::math;

static void ConfigureAppData(NkAppData &d)
{
	d.appName = "S6Exo5";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static float Propre(float v)
{
	return std::fabs(v) < 0.00005f ? 0.f : v;
}

static void Regard(std::FILE *f, const char *nom, const NkVec3f &r)
{
	std::fprintf(f, "%s : (%.4f, %.4f, %.4f)\n", nom, Propre(r.x), Propre(r.y), Propre(r.z));
	std::printf("%s : (%.4f, %.4f, %.4f)\n", nom, Propre(r.x), Propre(r.y), Propre(r.z));
}

int nkmain(const NkEntryState &state)
{
	(void)state;

	const float lacetDeg = 45.f;
	const float tangageDeg = 30.f;
	const NkVec3f avant(0.f, 0.f, -1.f);

	const NkQuatf qLacet = NkQuatf::RotateY(NkAngle(lacetDeg));
	const NkQuatf qTangage = NkQuatf::RotateX(NkAngle(tangageDeg));

	const NkVec3f lacetTangage = (qLacet * qTangage) * avant;
	const NkVec3f tangageLacet = (qTangage * qLacet) * avant;

	const NkMat4f rotation = NkMat4f::RotationY(NkAngle(lacetDeg)) * NkMat4f::RotationX(NkAngle(tangageDeg));
	const NkQuatf qMatrices = NkQuatf(rotation).Normalized();
	const NkVec3f matrices = qMatrices * avant;

	std::FILE *f = std::fopen("regards.txt", "w");
	if (!f)
	{
		return 1;
	}
	Regard(f, "lacet*tangage", lacetTangage);
	Regard(f, "tangage*lacet", tangageLacet);
	Regard(f, "matrices", matrices);
	std::fclose(f);

	const float yMain = std::sin(tangageDeg * 3.14159265f / 180.f);
	std::printf("\nverification a la main : y = sin(30 deg) = %.4f\n", yMain);
	std::printf("ecart sur y, lacet*tangage : %.3e\n", std::fabs(lacetTangage.y - yMain));
	std::printf("ecart sur y, tangage*lacet : %.3e\n", std::fabs(tangageLacet.y - yMain));
	std::printf("ecart sur y, matrices      : %.3e\n", std::fabs(matrices.y - yMain));
	return 0;
}
