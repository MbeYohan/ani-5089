#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"

#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Core/NkRenderGraph.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterialCollection.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render2D/NkRender2D.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include "NKMath/NkQuat.h"
#include "NKTime/NkChrono.h"
#include "NKEvent/NkMouseEvent.h"

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d) { d.appName = "MaSalle"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace
{

	const float32 kPiece = 4.00f;
	const float32 kPlafond = 2.50f;
	const float32 kMur = 0.10f;
	const float32 kPorteL = 0.90f;
	const float32 kPorteH = 2.00f;
	const float32 kFenL = 1.20f;
	const float32 kFenH = 1.00f;
	const float32 kFenY = 1.40f;
	const float32 kTableL = 1.20f;
	const float32 kTableP = 0.80f;
	const float32 kTableH = 0.75f;
	const float32 kYeux = 1.70f;
	const float32 kDemi = kPiece * 0.5f;

	void Pave(NkRender3D *r3d, NkMeshSystem *meshes, const NkVec3f &centre,
			  const NkVec3f &taille, const NkVec3f &teinte, float32 rugosite = 1.f)
	{
		NkDrawCall3D dc;
		dc.mesh = meshes->GetCube();
		dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
		dc.aabb = {{centre.x - taille.x * 0.5f, centre.y - taille.y * 0.5f,
					centre.z - taille.z * 0.5f},
				   {centre.x + taille.x * 0.5f, centre.y + taille.y * 0.5f,
					centre.z + taille.z * 0.5f}};
		dc.tint = teinte;
		dc.metallic = 0.f;
		dc.roughness = rugosite;
		r3d->Submit(dc);
	}

	void SoumettreLaSalle(NkRender3D *r3d, NkMeshSystem *meshes)
	{
		const NkVec3f grisMur = {0.72f, 0.70f, 0.66f};
		const NkVec3f grisSol = {0.38f, 0.36f, 0.34f};
		const NkVec3f bois = {0.55f, 0.38f, 0.22f};
		const NkVec3f vitre = {0.75f, 0.85f, 0.92f};

		{
			NkDrawCall3D dc;
			dc.mesh = meshes->GetPlane();
			dc.transform = NkMat4f::Scale({kPiece, 1.f, kPiece});
			dc.aabb = {{-kDemi, 0.f, -kDemi}, {kDemi, 0.f, kDemi}};
			dc.castShadow = false;
			dc.tint = grisSol;
			dc.metallic = 0.f;
			dc.roughness = 1.f;
			r3d->Submit(dc);
		}

		const float32 yMur = kPlafond * 0.5f;
		const float32 bord = kDemi + kMur * 0.5f;

		Pave(r3d, meshes, {0.f, yMur, -bord}, {kPiece, kPlafond, kMur}, grisMur);
		Pave(r3d, meshes, {0.f, yMur, bord}, {kPiece, kPlafond, kMur}, grisMur);
		Pave(r3d, meshes, {-bord, yMur, 0.f}, {kMur, kPlafond, kPiece}, grisMur);
		Pave(r3d, meshes, {bord, yMur, 0.f}, {kMur, kPlafond, kPiece}, grisMur);

		Pave(r3d, meshes, {0.f, kPlafond + kMur * 0.5f, 0.f},
			 {kPiece, kMur, kPiece}, grisMur);

		Pave(r3d, meshes, {-0.70f, kPorteH * 0.5f, -kDemi + 0.02f},
			 {kPorteL, kPorteH, 0.04f}, bois, 0.6f);

		Pave(r3d, meshes, {-kDemi + 0.02f, kFenY, -0.90f},
			 {0.04f, kFenH, kFenL}, vitre, 0.15f);
		Pave(r3d, meshes, {-kDemi + 0.02f, kFenY, 0.90f},
			 {0.04f, kFenH, kFenL}, vitre, 0.15f);

		const float32 tx = 0.60f, tz = 0.70f;
		const float32 ep = 0.05f;
		Pave(r3d, meshes, {tx, kTableH - ep * 0.5f, tz},
			 {kTableL, ep, kTableP}, bois, 0.5f);

		const float32 pied = 0.06f;
		const float32 hp = kTableH - ep;
		for (int i = 0; i < 4; ++i)
		{
			const float32 dx = ((i & 1) ? 1.f : -1.f) * (kTableL * 0.5f - pied);
			const float32 dz = ((i & 2) ? 1.f : -1.f) * (kTableP * 0.5f - pied);
			Pave(r3d, meshes, {tx + dx, hp * 0.5f, tz + dz},
				 {pied, hp, pied}, bois, 0.6f);
		}

		Pave(r3d, meshes, {-1.10f, 0.50f, 1.20f}, {1.f, 1.f, 1.f},
			 {0.85f, 0.80f, 0.30f}, 0.8f);
	}

	uint64 EnvU64(const char *nom, uint64 defaut)
	{
		const char *t = getenv(nom);
		if (t == nullptr || *t == '\0')
			return defaut;
		return (uint64)strtoull(t, nullptr, 10);
	}

}

namespace
{

	const double kPiD = 3.14159265358979323846;

	void Ligne(const char *texte)
	{
		std::printf("%s\n", texte);
		const char *chemin = getenv("MASALLE_OUT");
		if (chemin != nullptr && *chemin != '\0')
		{
			FILE *f = std::fopen(chemin, "a");
			if (f != nullptr)
			{
				std::fprintf(f, "%s\n", texte);
				std::fclose(f);
			}
		}
	}

	double Net(double x, double seuil = 5e-4)
	{
		return (std::fabs(x) < seuil) ? 0.0 : x;
	}
}

namespace
{

	NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to, float32 dtSeconds)
	{
		if (dtSeconds <= 0.f)
		{
			return NkVec3f(0.f, 0.f, 0.f);
		}
		NkQuatf delta = (to * from.Conjugate()).Normalized();
		const float32 angleRad = delta.Angle().Rad();
		if (angleRad <= 1e-6f)
		{
			return NkVec3f(0.f, 0.f, 0.f);
		}
		return delta.Axis() * (angleRad / dtSeconds);
	}

	NkQuatf Extrapoler(const NkQuatf &orientation, const NkVec3f &vitesse, float32 dtSeconds)
	{
		const float32 speedRad = vitesse.Len();
		if (speedRad > 1e-6f)
		{
			const NkVec3f axis = vitesse * (1.f / speedRad);
			const NkQuatf delta(NkAngle::FromRad(speedRad * dtSeconds), axis);
			return (delta * orientation).Normalized();
		}
		return orientation;
	}

	double ErreurDeg(const NkQuatf &a, const NkQuatf &b)
	{
		const NkQuatf d = (b * a.Conjugate()).Normalized();
		const double v = std::sqrt((double)d.x * d.x + (double)d.y * d.y + (double)d.z * d.z);
		return 2.0 * std::atan2(v, std::fabs((double)d.w)) * 180.0 / kPiD;
	}
}

int nkmain(const NkEntryState &state)
{
	(void)state;

	NkWindowConfig cfg;
	cfg.title = "Ma salle";
	cfg.width = 1280;
	cfg.height = 720;
	cfg.centered = true;
	cfg.vsync = true;

	NkWindow fenetre(cfg);
	if (!fenetre.IsValid())
	{
		std::printf("[MaSalle] la fenetre n'a pas pu s'ouvrir\n");
		return 1;
	}

	NkDeviceInitInfo devInit{};
	devInit.surface = fenetre.GetSurfaceDesc();
	devInit.width = fenetre.GetSize().x;
	devInit.height = fenetre.GetSize().y;

	NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInit);
	if (device == nullptr)
	{
		std::printf("[MaSalle] aucune API graphique disponible\n");
		return 1;
	}

	NkRendererConfig cfgEcran =
		NkRendererConfig::For2D(devInit.api, devInit.width, devInit.height);
	NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
	if (rEcran == nullptr || !rEcran->Initialize())
	{
		std::printf("[MaSalle] le renderer d'ecran n'a pas pu s'initialiser\n");
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkOffscreenDesc od;
	od.width = devInit.width;
	od.height = devInit.height;
	od.hasDepth = true;
	od.hdr = false;
	od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
	od.readable = true;
	od.readback = true;
	od.name = "MaSalle_Cible";

	NkOffscreenTarget *cible = rEcran->CreateOffscreen(od);
	if (cible == nullptr)
	{
		std::printf("[MaSalle] cible hors ecran refusee\n");
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	NkRendererConfig cfgSalle =
		NkRendererConfig::ForGame(devInit.api, devInit.width, devInit.height);
	cfgSalle.Disable(NK_SS_RENDER2D);
	cfgSalle.Disable(NK_SS_TEXT);
	cfgSalle.Disable(NK_SS_OVERLAY);
	cfgSalle.Enable(NK_SS_OFFSCREEN);

	if (EnvU64("MASALLE_POST", 1) == 0)
	{
		cfgSalle.Disable(NK_SS_POST_PROCESS);
	}

	NkRenderer *renderer = NkRenderer::Create(device, cfgSalle);
	if (renderer == nullptr || !renderer->Initialize())
	{
		std::printf("[MaSalle] le renderer de la salle n'a pas pu s'initialiser\n");
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}
	renderer->SetFinalColorTarget(
		rEcran->GetTextures()->GetRHIHandle(cible->GetColorResult()));

	std::printf("[MaSalle] salle de %.1f m sur %.1f m, plafond a %.2f m\n",
				(double)kPiece, (double)kPiece, (double)kPlafond);

	const uint64 imageDeSortie = EnvU64("MASALLE_EXIT", 0);
	const uint64 imageDeCapture = EnvU64("MASALLE_SHOT", 0);

	uint64 image = 0;
	bool tourne = true;
	const double tourDegS = (double)EnvU64("MASALLE_TOUR", 20);
	double lacetRad = 0.0, tangageRad = 0.0;
	int32 cumulX = 0, cumulY = 0;
	const double origineNs = NkChrono::Now().nanoseconds;
	NkQuatf orientationPrecedente = NkQuatf::Identity();
	double debutPrecedentNs = 0.0;
	char avant[200] = "", fautive[200] = "";
	int etat = 0;
	const char *etiquette = getenv("MASALLE_ETIQUETTE");
	if (etiquette == nullptr || *etiquette == '\0')
	{
		etiquette = "sans le test";
	}

	while (tourne && fenetre.IsOpen())
	{
		const double debutNs = NkChrono::Now().nanoseconds;
		NkEvent *ev = nullptr;
		while (NkEvents().PollEvent(ev))
		{
			if (ev->As<NkWindowCloseEvent>() != nullptr)
			{
				tourne = false;
			}
			if (auto *brut = ev->As<NkMouseRawEvent>())
			{
				cumulX += brut->GetDeltaX();
				cumulY += brut->GetDeltaY();
			}
		}
		if (!tourne)
			break;

		if (tourDegS > 0.0)
		{
			lacetRad = tourDegS * (debutNs - origineNs) * 1e-9 * kPiD / 180.0;
		}
		else
		{
			lacetRad -= (double)cumulX * 0.0022;
			tangageRad -= (double)cumulY * 0.0022;
			tangageRad = std::max(-1.55, std::min(1.55, tangageRad));
		}
		cumulX = 0;
		cumulY = 0;
		const NkMat4f rotation = NkMat4f::RotationY(NkAngle::FromRad((float32)lacetRad)) *
								 NkMat4f::RotationX(NkAngle::FromRad((float32)tangageRad));
		const NkQuatf orientation = NkQuatf(rotation).Normalized();
		if (image >= 1)
		{
			const float32 dt = (float32)((debutNs - debutPrecedentNs) * 1e-9);
			const NkVec3f v = VitesseAngulaire(orientationPrecedente, orientation, dt);
			const double wBrut = (double)(orientation * orientationPrecedente.Conjugate()).Normalized().w;
			const double degS = 180.0 / kPiD;
			char ligne[200];
			std::snprintf(ligne, sizeof ligne, "image %llu : lacet = %.3f deg, vitesse = (%.3f, %.3f, %.3f) deg/s, delta.w = %.6f",
						  (unsigned long long)image, lacetRad * degS,
						  Net(v.x * degS), Net(v.y * degS), Net(v.z * degS), wBrut);
			std::printf("%s\n", ligne);
			if (etat == 1)
			{
				Ligne(etiquette);
				Ligne(avant);
				Ligne(fautive);
				Ligne(ligne);
				etat = 2;
			}
			else if (etat == 0 && wBrut < 0.0)
			{
				std::snprintf(fautive, sizeof fautive, "%s", ligne);
				etat = 1;
			}
			if (etat == 0)
			{
				std::snprintf(avant, sizeof avant, "%s", ligne);
			}
			if (lacetRad * degS >= 360.0)
			{
				tourne = false;
			}
		}
		orientationPrecedente = orientation;
		debutPrecedentNs = debutNs;

		if (rEcran->BeginFrame())
		{
			NkICommandBuffer *cmd = rEcran->GetCmd();

			NkRender3D *r3d = renderer->GetRender3D();

			renderer->FlushGraphRebuilds();
			r3d->ResetFrame();
			if (auto *mc = renderer->GetMaterialCollection())
			{
				mc->Upload();
			}

			NkCamera3DData cd;

			cd.position = {0.30f, kYeux, 1.20f};
			cd.target = cd.position + orientation * NkVec3f(0.f, 0.f, -1.f);
			cd.up = {0.f, 1.f, 0.f};
			cd.fovY = 70.f;
			cd.aspect = (float32)devInit.width / (float32)devInit.height;
			cd.nearPlane = 0.05f;
			cd.farPlane = 100.f;

			NkSceneContext sctx;
			sctx.camera = NkCamera3D(cd);
			sctx.time = (float32)image / 60.f;
			r3d->SetIBLStrength(0.35f);

			NkLightDesc soleil;
			soleil.type = NkLightType::NK_DIRECTIONAL;
			soleil.direction = {-0.5f, -1.f, -0.35f};
			soleil.color = {1.f, 0.96f, 0.88f};
			soleil.intensity = 3.f;
			soleil.castShadow = false;
			sctx.lights.PushBack(soleil);

			r3d->BeginScene(sctx);
			SoumettreLaSalle(r3d, renderer->GetMeshSystem());

			if (auto *graph = renderer->GetRenderGraph())
			{
				graph->Execute(cmd);
			}

			if (auto *r2d = rEcran->GetRender2D())
			{
				r2d->DrawImage(cible->GetColorResult(),
							   {0.f, 0.f, (float32)devInit.width,
								(float32)devInit.height});
			}

			rEcran->Present();
			rEcran->EndFrame();
		}

		++image;

		if (imageDeCapture != 0 && image == imageDeCapture)
		{
			const char *chemin = getenv("MASALLE_SHOT_PATH");
			if (chemin == nullptr || *chemin == '\0')
			{
				chemin = "masalle.png";
			}
			const bool ok = cible->Capture(chemin);
			std::printf("[MaSalle] capture image %llu -> %s (%s)\n",
						(unsigned long long)image, chemin, ok ? "ok" : "echec");
		}

		if (imageDeSortie != 0 && image >= imageDeSortie)
		{
			tourne = false;
		}
	}

	if (etat != 2)
	{
		std::printf("[MaSalle] aucune image fautive trouvee (etat %d)\n", etat);
	}
	std::printf("[MaSalle] %llu images\n", (unsigned long long)image);

	NkRenderer::Destroy(renderer);
	NkRenderer::Destroy(rEcran);
	NkDeviceFactory::Destroy(device);
	return 0;
}
