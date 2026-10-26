#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKMath/NkAngle.h"
#include "NKMath/NkMat.h"
#include "NKRenderer/Core/NkCamera.h"
#include <cmath>
#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::math;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d)
{
	d.appName = "S6Exo10";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const float kPi = 3.14159265f;

static float EnRadians(float degres)
{
	return degres * kPi / 180.f;
}

static NkMat4f Projection(float gauche, float droite, float haut, float bas, float nearPlane, float farPlane)
{
	const float tanLeft = std::tan(gauche);
	const float tanRight = std::tan(droite);
	const float tanUp = std::tan(haut);
	const float tanDown = std::tan(bas);
	const float tanWidth = tanRight - tanLeft;
	const float tanHeight = tanUp - tanDown;

	NkMat4f result = NkMat4f::Zero();
	result.mat[0][0] = 2.f / tanWidth;
	result.mat[1][1] = 2.f / tanHeight;
	result.mat[2][0] = (tanRight + tanLeft) / tanWidth;
	result.mat[2][1] = (tanUp + tanDown) / tanHeight;
	result.mat[2][3] = -1.f;
	result.mat[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
	result.mat[3][2] = 2.f * farPlane * nearPlane / (nearPlane - farPlane);
	return result;
}

static float Comparer(std::FILE *f, const char *epreuve, const NkMat4f &a, const NkMat4f &b)
{
	float plusGrand = 0.f;
	for (int ligne = 0; ligne < 4; ligne++)
	{
		float e[4];
		for (int colonne = 0; colonne < 4; colonne++)
		{
			e[colonne] = std::fabs(a.mat[colonne][ligne] - b.mat[colonne][ligne]);
			if (e[colonne] > plusGrand)
			{
				plusGrand = e[colonne];
			}
		}
		std::fprintf(f, "%s, ligne %d : %.3e %.3e %.3e %.3e\n", epreuve, ligne, e[0], e[1], e[2], e[3]);
		std::printf("%s, ligne %d : %.3e %.3e %.3e %.3e\n", epreuve, ligne, e[0], e[1], e[2], e[3]);
	}
	return plusGrand;
}

static void ImageDuPoint(const NkMat4f &m, float tx, float ty, float d, float &ndcX, float &ndcY)
{
	const float x = tx * d;
	const float y = ty * d;
	const float z = -d;
	const float clipX = m.mat[0][0] * x + m.mat[1][0] * y + m.mat[2][0] * z + m.mat[3][0];
	const float clipY = m.mat[0][1] * x + m.mat[1][1] * y + m.mat[2][1] * z + m.mat[3][1];
	const float clipW = m.mat[0][3] * x + m.mat[1][3] * y + m.mat[2][3] * z + m.mat[3][3];
	ndcX = clipX / clipW;
	ndcY = clipY / clipW;
}

int nkmain(const NkEntryState &state)
{
	(void)state;

	const float nearPlane = 0.05f;
	const float farPlane = 100.f;

	const float rapport = 1280.f / 720.f;
	const float tanHaut = std::tan(EnRadians(70.f) * 0.5f);
	const float tanDroite = rapport * tanHaut;
	const float haut = std::atan(tanHaut);
	const float droite = std::atan(tanDroite);
	const NkMat4f maSym = Projection(-droite, droite, haut, -haut, nearPlane, farPlane);
	const NkMat4f moteurSym = NkMat4f::Perspective(NkAngle(70.f), rapport, nearPlane, farPlane);

	const float gauche = EnRadians(-52.f);
	const float droiteQ = EnRadians(45.f);
	const float hautQ = EnRadians(48.f);
	const float basQ = EnRadians(-50.f);
	const NkMat4f maQuest = Projection(gauche, droiteQ, hautQ, basQ, nearPlane, farPlane);
	NkCamera3D cam;
	cam.SetNearFar(nearPlane, farPlane);
	cam.SetFovAsym(gauche, droiteQ, hautQ, basQ);
	const NkMat4f moteurQuest = cam.GetProj();

	std::FILE *f = std::fopen("ecarts.txt", "w");
	if (!f)
	{
		return 1;
	}
	const float maxSym = Comparer(f, "symetrique", maSym, moteurSym);
	const float maxQuest = Comparer(f, "quest", maQuest, moteurQuest);
	std::fprintf(f, "symetrique : plus grand ecart %.3e\n", maxSym);
	std::fprintf(f, "quest : plus grand ecart %.3e\n", maxQuest);
	std::printf("symetrique : plus grand ecart %.3e\n", maxSym);
	std::printf("quest : plus grand ecart %.3e\n", maxQuest);
	std::fclose(f);

	std::printf("\nsymetrique : mat[2][0] = %.3e, mat[2][1] = %.3e\n", maSym.mat[2][0], maSym.mat[2][1]);
	std::printf("quest : mat[2][0] = %.4f, mat[2][1] = %.4f\n", maQuest.mat[2][0], maQuest.mat[2][1]);

	float nx, ny;
	ImageDuPoint(maQuest, std::tan(gauche), 0.f, 2.f, nx, ny);
	std::printf("bord gauche : x = %.4f\n", nx);
	ImageDuPoint(maQuest, std::tan(droiteQ), 0.f, 2.f, nx, ny);
	std::printf("bord droit : x = %.4f\n", nx);
	ImageDuPoint(maQuest, 0.f, std::tan(hautQ), 2.f, nx, ny);
	std::printf("bord haut : y = %.4f\n", ny);
	ImageDuPoint(maQuest, 0.f, std::tan(basQ), 2.f, nx, ny);
	std::printf("bord bas : y = %.4f\n", ny);
	return 0;
}
