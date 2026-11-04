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

	NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to, float32 dtSeconds)
	{
		if (dtSeconds <= 0.f)
		{
			return NkVec3f(0.f, 0.f, 0.f);
		}
		NkQuatf delta = (to * from.Conjugate()).Normalized();
		if (delta.w < 0.f)
		{
			delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
		}
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

namespace
{

	const double kPeriode = 1.0 / 72.0;
	const double kDepartGeste = 1.0;

	double Lacet(double t)
	{
		const double amplitude = 90.0, duree = 0.400;
		const double s = t - kDepartGeste;
		if (s <= 0.0)
			return 0.0;
		if (s >= duree)
			return amplitude;
		return amplitude * 0.5 * (1.0 - std::cos(kPiD * s / duree));
	}

	NkQuatf Pose(double t)
	{
		return NkQuatf::RotateY(NkAngle((float32)Lacet(t)));
	}

	double LacetDuQuaternion(const NkQuatf &q)
	{
		return 2.0 * std::atan2((double)q.y, (double)q.w) * 180.0 / kPiD;
	}

	struct Echantillon
	{
		double temps;
		NkQuatf orientation;
		NkVec3f vitesse;
	};

	const int kMaxEchantillons = 64;

	struct Historique
	{
		Echantillon echantillons[kMaxEchantillons];
		int tete = 0;
		int compte = 0;

		void Ajouter(double temps, const NkQuatf &orientation)
		{
			NkVec3f vitesse(0.f, 0.f, 0.f);
			if (compte > 0)
			{
				const Echantillon &avant = echantillons[tete];
				vitesse = VitesseAngulaire(avant.orientation, orientation, (float32)(temps - avant.temps));
			}
			tete = (compte == 0) ? 0 : (tete + 1) % kMaxEchantillons;
			echantillons[tete] = {temps, orientation, vitesse};
			if (compte < kMaxEchantillons)
				++compte;
		}

		const Echantillon &SampleAt(double maintenant, double latence) const
		{
			if (latence <= 0.0 || compte <= 1)
			{
				return echantillons[tete];
			}
			const double cible = maintenant - latence;
			int index = tete;
			for (int i = 0; i < compte; ++i)
			{
				if (echantillons[index].temps <= cible)
				{
					return echantillons[index];
				}
				index = (index + kMaxEchantillons - 1) % kMaxEchantillons;
			}
			return echantillons[(tete + kMaxEchantillons - (compte - 1)) % kMaxEchantillons];
		}
	};

	NkQuatf Predire(const Echantillon &e, double instant, bool borne, double *horizonUtilise)
	{
		double dt = instant - e.temps;
		if (borne)
		{
			dt = std::max(-0.1, std::min(0.1, dt));
		}
		*horizonUtilise = dt;
		return Extrapoler(e.orientation, e.vitesse, (float32)dt);
	}

	void CalculLatences()
	{
		const double latences[5] = {0.0, 0.010, 0.020, 0.040, 0.080};
		for (int l = 0; l < 5; ++l)
		{
			Historique h;
			double sommeAvec = 0.0, sommeSans = 0.0, maxAvec = 0.0, maxSans = 0.0;
			int n = 0;
			for (int k = 0; k <= 216; ++k)
			{
				const double t = k * kPeriode;
				h.Ajouter(t, Pose(t));
				if (k < 72 || k > 136)
					continue;
				const Echantillon &e = h.SampleAt(t, latences[l]);
				const double quand = t + kPeriode;
				double horizon;
				const NkQuatf predite = Predire(e, quand, true, &horizon);
				const NkQuatf vraie = Pose(quand);
				const double avec = ErreurDeg(vraie, predite);
				const double sans = ErreurDeg(vraie, e.orientation);
				sommeAvec += avec;
				sommeSans += sans;
				maxAvec = std::max(maxAvec, avec);
				maxSans = std::max(maxSans, sans);
				++n;
			}
			char ligne[200];
			std::snprintf(ligne, sizeof ligne, "latence %.0f ms : avec %.4f deg (max %.4f), sans %.4f deg (max %.4f)",
						  latences[l] * 1000.0, sommeAvec / n, maxAvec, sommeSans / n, maxSans);
			Ligne(ligne);
		}

		for (int ms = 0; ms <= 400; ms += 20)
		{
			Historique hb;
			double sommeAvec = 0.0, sommeSans = 0.0;
			int n = 0;
			for (int k = 0; k <= 216; ++k)
			{
				const double t = k * kPeriode;
				hb.Ajouter(t, Pose(t));
				if (k < 72 || k > 136)
					continue;
				const Echantillon &e = hb.SampleAt(t, ms / 1000.0);
				double horizon;
				const NkQuatf predite = Predire(e, t + kPeriode, true, &horizon);
				sommeAvec += ErreurDeg(Pose(t + kPeriode), predite);
				sommeSans += ErreurDeg(Pose(t + kPeriode), e.orientation);
				++n;
			}
			std::printf("[balayage] latence %d ms : avec %.4f deg, sans %.4f deg, rapport %.2f\n", ms, sommeAvec / n, sommeSans / n, sommeSans / sommeAvec);
		}

		Historique h;
		const double latence = 1.0;
		int meilleure = -1;
		double ecartMini = 1e9;
		for (int k = 0; k <= 216; ++k)
		{
			const double t = k * kPeriode;
			h.Ajouter(t, Pose(t));
			const double ecart = std::fabs(h.SampleAt(t, latence).temps - 1.2);
			if (ecart < ecartMini)
			{
				ecartMini = ecart;
				meilleure = k;
			}
		}
		Historique h2;
		for (int k = 0; k <= meilleure; ++k)
		{
			h2.Ajouter(k * kPeriode, Pose(k * kPeriode));
		}
		const double t = meilleure * kPeriode;
		const Echantillon &e = h2.SampleAt(t, latence);
		const double quand = t + kPeriode;
		const double vrai = LacetDuQuaternion(Pose(quand));
		for (int avecBorne = 0; avecBorne < 2; ++avecBorne)
		{
			double horizon;
			const NkQuatf predite = Predire(e, quand, avecBorne == 1, &horizon);
			char ligne[200];
			std::snprintf(ligne, sizeof ligne, "%s : echantillon a %.3f s, horizon %.1f ms, predit %.1f deg, vrai %.1f deg",
						  avecBorne ? "avec borne" : "sans borne", e.temps, horizon * 1000.0,
						  Net(LacetDuQuaternion(predite), 0.05), Net(vrai, 0.05));
			Ligne(ligne);
		}
	}
}

int nkmain(const NkEntryState &state)
{
	(void)state;
	CalculLatences();

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
