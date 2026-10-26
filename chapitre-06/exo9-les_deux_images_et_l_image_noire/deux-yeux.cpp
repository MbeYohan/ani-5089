#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkWindowEvent.h"

#include "NKImage/Core/NkImage.h"
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

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;
using namespace nkentseu::math;

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

	const uint32 kOeilL = 640;
	const uint32 kOeilH = 720;
	const float32 kEcartPupilles = 0.063f;
	const NkVec3f kTete = {0.30f, kYeux, 1.20f};

	struct Oeil
	{
		NkRenderer *renderer = nullptr;
		NkOffscreenTarget *cible = nullptr;
	};

	bool CreerOeil(NkIDevice *device, NkRenderer *rEcran, const char *nom, Oeil &o)
	{
		NkOffscreenDesc od;
		od.width = kOeilL;
		od.height = kOeilH;
		od.hasDepth = true;
		od.hdr = false;
		od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
		od.readable = true;
		od.readback = true;
		od.name = nom;
		o.cible = rEcran->CreateOffscreen(od);
		if (o.cible == nullptr)
			return false;

		NkRendererConfig cfg = NkRendererConfig::ForGame(device->GetApi(), 1280, 720);
		cfg.Disable(NK_SS_RENDER2D);
		cfg.Disable(NK_SS_TEXT);
		cfg.Disable(NK_SS_OVERLAY);
		cfg.Enable(NK_SS_OFFSCREEN);
		o.renderer = NkRenderer::Create(device, cfg);
		if (o.renderer == nullptr || !o.renderer->Initialize())
			return false;

		o.renderer->SetFinalColorTarget(rEcran->GetTextures()->GetRHIHandle(o.cible->GetColorResult()));
		o.renderer->SetRenderSizeOverride(kOeilL, kOeilH);
		return true;
	}

	void DessinerOeil(Oeil &o, NkICommandBuffer *cmd, const NkVec3f &position, uint64 image)
	{
		NkRender3D *r3d = o.renderer->GetRender3D();

		o.renderer->FlushGraphRebuilds();
		r3d->ResetFrame();
		if (auto *mc = o.renderer->GetMaterialCollection())
		{
			mc->Upload();
		}

		NkCamera3DData cd;
		cd.position = position;
		cd.target = {position.x, position.y, position.z - 1.f};
		cd.up = {0.f, 1.f, 0.f};
		cd.fovY = 70.f;
		cd.aspect = (float32)kOeilL / (float32)kOeilH;
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
		SoumettreLaSalle(r3d, o.renderer->GetMeshSystem());

		if (auto *graph = o.renderer->GetRenderGraph())
		{
			graph->Execute(cmd);
		}
	}

	struct Bilan
	{
		bool ok = false;
		uint64 pixels = 0;
		uint64 total = 0;
		int ecartMax = 0;
	};

	int Ecart(const NkColor &p, const NkColor &q)
	{
		int e = 0;
		const int dr = std::abs((int)p.r - (int)q.r);
		const int dg = std::abs((int)p.g - (int)q.g);
		const int db = std::abs((int)p.b - (int)q.b);
		if (dr > e)
			e = dr;
		if (dg > e)
			e = dg;
		if (db > e)
			e = db;
		return e;
	}

	double Moyenne(const NkImage &img)
	{
		double somme = 0.0;
		for (int y = 0; y < img.Height(); ++y)
		{
			for (int x = 0; x < img.Width(); ++x)
			{
				const NkColor c = img.GetPixel(x, y);
				somme += (double)c.r + (double)c.g + (double)c.b;
			}
		}
		return somme / (3.0 * (double)img.Width() * (double)img.Height());
	}

	Bilan Soustraire(const char *fichierA, const char *fichierB, const char *sortie)
	{
		Bilan bilan;
		NkImage a, b;
		if (!a.Load(fichierA) || !b.Load(fichierB))
		{
			std::printf("[deux-yeux] impossible de recharger %s ou %s\n", fichierA, fichierB);
			return bilan;
		}
		if (a.Width() != b.Width() || a.Height() != b.Height())
		{
			std::printf("[deux-yeux] les deux images n'ont pas la meme taille\n");
			return bilan;
		}
		std::printf("[deux-yeux] %s : moyenne %.1f sur 255, %s : moyenne %.1f sur 255\n", fichierA, Moyenne(a),
					fichierB, Moyenne(b));

		NkImage diff = NkImage::Create((uint32)a.Width(), (uint32)a.Height(), 4);
		bilan.total = (uint64)a.Width() * (uint64)a.Height();
		for (int y = 0; y < a.Height(); ++y)
		{
			for (int x = 0; x < a.Width(); ++x)
			{
				const int e = Ecart(a.GetPixel(x, y), b.GetPixel(x, y));
				if (e > 1)
					bilan.pixels++;
				if (e > bilan.ecartMax)
					bilan.ecartMax = e;
				const int v = (e * 4 > 255) ? 255 : e * 4;
				diff.SetPixel(x, y, NkColor((uint8)v, (uint8)v, (uint8)v, 255));
			}
		}
		bilan.ok = diff.SavePNG(sortie);
		return bilan;
	}

	NkIDevice *CreerPeripherique(NkDeviceInitInfo &info)
	{
		const char *nom = std::getenv("API");
		if (nom == nullptr)
			nom = "dx11";
		if (std::strcmp(nom, "sw") == 0)
			return NkDeviceFactory::CreateForApi(NkGraphicsApi::NK_GFX_API_SOFTWARE, info);
		if (std::strcmp(nom, "opengl") == 0)
			return NkDeviceFactory::CreateForApi(NkGraphicsApi::NK_GFX_API_OPENGL, info);
		if (std::strcmp(nom, "vulkan") == 0)
			return NkDeviceFactory::CreateForApi(NkGraphicsApi::NK_GFX_API_VULKAN, info);
		if (std::strcmp(nom, "dx12") == 0)
			return NkDeviceFactory::CreateForApi(NkGraphicsApi::NK_GFX_API_DX12, info);
		return NkDeviceFactory::CreateForApi(NkGraphicsApi::NK_GFX_API_DX11, info);
	}

}

int nkmain(const NkEntryState &state)
{
	(void)state;

	NkWindowConfig cfg;
	cfg.title = "Exo 9 - les deux yeux";
	cfg.width = 1280;
	cfg.height = 720;
	cfg.centered = true;
	cfg.vsync = true;

	NkWindow fenetre(cfg);
	if (!fenetre.IsValid())
	{
		std::printf("[deux-yeux] la fenetre n'a pas pu s'ouvrir\n");
		return 1;
	}

	NkDeviceInitInfo devInit{};
	devInit.surface = fenetre.GetSurfaceDesc();
	devInit.width = fenetre.GetSize().x;
	devInit.height = fenetre.GetSize().y;

	NkIDevice *device = CreerPeripherique(devInit);
	if (device == nullptr)
	{
		std::printf("[deux-yeux] aucune API graphique disponible\n");
		return 1;
	}

	NkRendererConfig cfgEcran = NkRendererConfig::For2D(device->GetApi(), devInit.width, devInit.height);
	NkRenderer *rEcran = NkRenderer::Create(device, cfgEcran);
	if (rEcran == nullptr || !rEcran->Initialize())
	{
		std::printf("[deux-yeux] le renderer d'ecran n'a pas pu s'initialiser\n");
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	Oeil gauche, droit;
	if (!CreerOeil(device, rEcran, "Exo9_OeilGauche", gauche) || !CreerOeil(device, rEcran, "Exo9_OeilDroit", droit))
	{
		std::printf("[deux-yeux] un des deux yeux n'a pas pu etre cree\n");
		NkRenderer::Destroy(droit.renderer);
		NkRenderer::Destroy(gauche.renderer);
		NkRenderer::Destroy(rEcran);
		NkDeviceFactory::Destroy(device);
		return 1;
	}

	const NkVec3f droiteTete = {1.f, 0.f, 0.f};
	const NkVec3f decalage = droiteTete * (kEcartPupilles * 0.5f);

	const bool rester = EnvU64("MASALLE_RESTER", 0) != 0;
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
			if (auto *touche = ev->As<NkKeyPressEvent>())
			{
				if (touche->GetKey() == NkKey::NK_ESCAPE)
					tourne = false;
			}
		}
		if (!tourne)
			break;

		const uint64 numero = image + 1;
		const bool oubli = (numero > 30 && numero <= 60);
		const NkVec3f posGauche = oubli ? kTete : kTete - decalage;
		const NkVec3f posDroit = oubli ? kTete : kTete + decalage;

		if (rEcran->BeginFrame())
		{
			NkICommandBuffer *cmd = rEcran->GetCmd();

			DessinerOeil(gauche, cmd, posGauche, image);
			DessinerOeil(droit, cmd, posDroit, image);

			if (auto *r2d = rEcran->GetRender2D())
			{
				r2d->DrawImage(gauche.cible->GetColorResult(), {0.f, 0.f, (float32)kOeilL, (float32)kOeilH});
				r2d->DrawImage(droit.cible->GetColorResult(), {(float32)kOeilL, 0.f, (float32)kOeilL, (float32)kOeilH});
			}

			rEcran->Present();
			rEcran->EndFrame();
		}

		++image;

		if (image == 30)
		{
			const bool g = gauche.cible->Capture("oeil-gauche.png");
			const bool d = droit.cible->Capture("oeil-droit.png");
			std::printf("[deux-yeux] capture image 30 : gauche %s, droit %s\n", g ? "ok" : "echec", d ? "ok" : "echec");
		}
		if (image == 60)
		{
			const bool g = gauche.cible->Capture("oubli-gauche.png");
			const bool d = droit.cible->Capture("oubli-droit.png");
			std::printf("[deux-yeux] capture image 60 : gauche %s, droit %s\n", g ? "ok" : "echec", d ? "ok" : "echec");
			if (!rester)
				tourne = false;
		}
	}

	std::printf("[deux-yeux] %llu images\n", (unsigned long long)image);

	int code = 0;
	if (image >= 60)
	{
		const Bilan yeux = Soustraire("oeil-gauche.png", "oeil-droit.png", "difference.png");
		const Bilan oubli = Soustraire("oubli-gauche.png", "oubli-droit.png", "difference-vue-oubliee.png");
		if (yeux.ok && oubli.ok)
		{
			std::FILE *f = std::fopen("difference.txt", "w");
			if (f != nullptr)
			{
				std::fprintf(f, "deux yeux : %llu pixels sur %llu, ecart max %d\n", (unsigned long long)yeux.pixels,
							 (unsigned long long)yeux.total, yeux.ecartMax);
				std::fprintf(f, "vue oubliee : %llu pixels sur %llu, ecart max %d\n", (unsigned long long)oubli.pixels,
							 (unsigned long long)oubli.total, oubli.ecartMax);
				std::fclose(f);
			}
			std::printf("deux yeux : %llu pixels sur %llu, ecart max %d\n", (unsigned long long)yeux.pixels,
						(unsigned long long)yeux.total, yeux.ecartMax);
			std::printf("vue oubliee : %llu pixels sur %llu, ecart max %d\n", (unsigned long long)oubli.pixels,
						(unsigned long long)oubli.total, oubli.ecartMax);
		}
		else
		{
			code = 2;
		}
		std::remove("oubli-gauche.png");
		std::remove("oubli-droit.png");
	}
	else
	{
		std::printf("[deux-yeux] fenetre fermee avant l'image 60 : rien a comparer\n");
		code = 3;
	}

	NkRenderer::Destroy(droit.renderer);
	NkRenderer::Destroy(gauche.renderer);
	NkRenderer::Destroy(rEcran);
	NkDeviceFactory::Destroy(device);
	return code;
}
