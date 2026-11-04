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

	struct Geste
	{
		const char *nom;
		double amplitude;
		double duree;
		bool arretNet;
	};

	double Lacet(double t, const Geste &g)
	{
		if (t <= 0.0)
			return 0.0;
		if (t >= g.duree)
			return g.amplitude;
		if (g.arretNet)
			return g.amplitude * t / g.duree;
		return g.amplitude * 0.5 * (1.0 - std::cos(kPiD * t / g.duree));
	}

	struct Erreurs
	{
		double moyenneVitesse, maxVitesse, moyenneAcceleration, maxAcceleration;
	};

	Erreurs Mesurer(const Geste &g)
	{
		const double periode = 1.0 / 72.0;
		double sommeV = 0.0, sommeA = 0.0, maxV = 0.0, maxA = 0.0;
		int n = 0;
		for (int k = 2; (k + 1) * periode <= g.duree + 0.200 + 1e-9; ++k)
		{
			const double y0 = Lacet((k - 2) * periode, g);
			const double y1 = Lacet((k - 1) * periode, g);
			const double y2 = Lacet(k * periode, g);
			const double vitesse = (y2 - y1) / periode;
			const double acceleration = ((y2 - y1) - (y1 - y0)) / (periode * periode);
			const double predV = y2 + vitesse * periode;
			const double predA = predV + acceleration * periode * periode / 2.0;
			const double vrai = Lacet((k + 1) * periode, g);
			const double eV = std::fabs(predV - vrai);
			const double eA = std::fabs(predA - vrai);
			sommeV += eV;
			sommeA += eA;
			maxV = std::max(maxV, eV);
			maxA = std::max(maxA, eA);
			++n;
		}
		return {sommeV / n, maxV, sommeA / n, maxA};
	}

	void CalculModeles()
	{
		const Geste gestes[4] = {
			{"rapide", 90.0, 0.400, false},
			{"lent", 30.0, 1.000, false},
			{"demi-tour", 180.0, 0.300, false},
			{"arret", 90.0, 0.480, true},
		};
		for (const Geste &g : gestes)
		{
			const Erreurs e = Mesurer(g);
			char ligne[220];
			std::snprintf(ligne, sizeof ligne, "%s (%.0f deg en %.0f ms) : vitesse %.4f deg (max %.4f), acceleration %.4f deg (max %.4f)",
						  g.nom, g.amplitude, g.duree * 1000.0, e.moyenneVitesse, e.maxVitesse, e.moyenneAcceleration, e.maxAcceleration);
			Ligne(ligne);
		}
		for (int ms = 470; ms <= 530; ms += 5)
		{
			const Geste g = {"arret", 90.0, ms / 1000.0, true};
			const Erreurs e = Mesurer(g);
			std::printf("[balayage] arret en %d ms : vitesse %.4f (max %.4f), acceleration %.4f (max %.4f)\n",
						ms, e.moyenneVitesse, e.maxVitesse, e.moyenneAcceleration, e.maxAcceleration);
		}
	}
}

int nkmain(const NkEntryState &state)
{
	(void)state;
	CalculModeles();

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

	while (tourne && fenetre.IsOpen())
	{
		NkEvent *ev = nullptr;
		while (NkEvents().PollEvent(ev))
		{
			if (ev->As<NkWindowCloseEvent>() != nullptr)
			{
				tourne = false;
			}
		}
		if (!tourne)
			break;

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
			cd.target = {-0.70f, 1.10f, -kDemi};
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

	std::printf("[MaSalle] %llu images\n", (unsigned long long)image);

	NkRenderer::Destroy(renderer);
	NkRenderer::Destroy(rEcran);
	NkDeviceFactory::Destroy(device);
	return 0;
}
