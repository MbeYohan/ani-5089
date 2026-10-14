#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKLogger/NkLog.h"
#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKRenderer/Tools/Shadow/NkVirtualShadowMaps.h"
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d)
{
	d.appName = "Exo27";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static NkIDevice *CreerPeripherique(NkDeviceInitInfo &info)
{
	const char *nom = std::getenv("API");
	if (!nom)
		return NkDeviceFactory::CreateAutoDetect(info);
	struct Entree
	{
		const char *nom;
		NkGraphicsApi api;
	};
	static const Entree table[] = {
		{"sw", NkGraphicsApi::NK_GFX_API_SOFTWARE},
		{"opengl", NkGraphicsApi::NK_GFX_API_OPENGL},
		{"vulkan", NkGraphicsApi::NK_GFX_API_VULKAN},
		{"dx11", NkGraphicsApi::NK_GFX_API_DX11},
		{"dx12", NkGraphicsApi::NK_GFX_API_DX12},
	};
	for (const Entree &e : table)
		if (std::strcmp(e.nom, nom) == 0)
			return NkDeviceFactory::CreateForApi(e.api, info);
	logger.Errorf("API inconnue : %s (sw, opengl, vulkan, dx11, dx12)", nom);
	return nullptr;
}

static NkAABB Boite(NkVec3f c, NkVec3f t)
{
	NkAABB b;
	b.min = {c.x - t.x * 0.5f, c.y - t.y * 0.5f, c.z - t.z * 0.5f};
	b.max = {c.x + t.x * 0.5f, c.y + t.y * 0.5f, c.z + t.z * 0.5f};
	return b;
}

static void Cube(NkRender3D *r3d, NkMeshHandle cube, NkVec3f centre, NkVec3f taille, NkVec3f teinte, bool ombre = true)
{
	NkDrawCall3D dc;
	dc.mesh = cube;
	dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
	dc.aabb = Boite(centre, taille);
	dc.tint = teinte;
	dc.roughness = 0.9f;
	dc.castShadow = ombre;
	r3d->Submit(dc);
}

static void Sol(NkRender3D *r3d, NkMeshHandle plan)
{
	NkDrawCall3D dc;
	dc.mesh = plan;
	dc.transform = NkMat4f::Scale({4.f, 1.f, 4.f});
	dc.aabb = {{-2.f, 0.f, -2.f}, {2.f, 0.f, 2.f}};
	dc.castShadow = false;
	dc.tint = {0.35f, 0.33f, 0.30f};
	dc.roughness = 0.9f;
	r3d->Submit(dc);
}

static NkLightDesc Soleil(bool ombres)
{
	NkLightDesc sun;
	sun.type = NkLightType::NK_DIRECTIONAL;
	sun.direction = {-0.4f, -1.f, -0.3f};
	sun.color = {1.f, 0.95f, 0.85f};
	sun.intensity = 3.f;
	sun.castShadow = ombres;
	sun.shadowStatic = false;
	return sun;
}

static NkSceneContext Contexte(float aspect, NkVec3f position, NkVec3f cible, const NkLightDesc *soleil)
{
	NkCamera3DData camData;
	camData.up = {0.f, 1.f, 0.f};
	camData.fovY = 70.f;
	camData.aspect = aspect;
	camData.nearPlane = 0.05f;
	camData.farPlane = 200.f;
	NkCamera3D cam(camData);
	cam.SetPosition(position);
	cam.SetTarget(cible);
	NkSceneContext sctx;
	sctx.camera = cam;
	if (soleil)
		sctx.lights.PushBack(*soleil);
	return sctx;
}

static void DessinerSalle(NkRender3D *r3d, NkMeshHandle cube, NkMeshHandle plan, float decalage)
{
	Sol(r3d, plan);
	const NkVec3f mur = {0.72f, 0.70f, 0.66f};
	Cube(r3d, cube, {0.f, 1.25f, -2.f}, {4.f, 2.5f, 0.1f}, mur);
	Cube(r3d, cube, {0.f, 1.25f, 2.f}, {4.f, 2.5f, 0.1f}, mur);
	Cube(r3d, cube, {-2.f, 1.25f, 0.f}, {0.1f, 2.5f, 4.f}, mur);
	Cube(r3d, cube, {2.f, 1.25f, 0.f}, {0.1f, 2.5f, 4.f}, mur);
	Cube(r3d, cube, {0.f, 2.55f, 0.f}, {4.1f, 0.1f, 4.1f}, {0.85f, 0.85f, 0.85f}, false);
	const NkVec3f porte = {0.40f, 0.25f, 0.12f};
	const NkVec3f vitre = {0.45f, 0.65f, 0.85f};
	const float face = 1.95f;
	const float e = 0.02f;
	Cube(r3d, cube, {-0.70f, 1.f, -face + decalage - e * 0.5f}, {0.9f, 2.f, e}, porte);
	Cube(r3d, cube, {-face + decalage - e * 0.5f, 1.4f, -0.9f}, {e, 1.f, 1.2f}, vitre);
	Cube(r3d, cube, {-face + decalage - e * 0.5f, 1.4f, 0.9f}, {e, 1.f, 1.2f}, vitre);
	const NkVec3f bois = {0.45f, 0.30f, 0.18f};
	const float longueur = 1.20f, profondeur = 0.80f;
	const float xTable = 0.60f, zTable = 0.70f;
	const float dessus = 0.75f, epPlateau = 0.05f, section = 0.06f;
	const float yPlateau = dessus - epPlateau * 0.5f;
	const float hPied = dessus - epPlateau;
	const float yPied = hPied * 0.5f;
	const float dx = longueur * 0.5f - section * 0.5f;
	const float dz = profondeur * 0.5f - section * 0.5f;
	Cube(r3d, cube, {xTable, yPlateau, zTable}, {longueur, epPlateau, profondeur}, bois);
	Cube(r3d, cube, {xTable - dx, yPied, zTable - dz}, {section, hPied, section}, bois);
	Cube(r3d, cube, {xTable + dx, yPied, zTable - dz}, {section, hPied, section}, bois);
	Cube(r3d, cube, {xTable - dx, yPied, zTable + dz}, {section, hPied, section}, bois);
	Cube(r3d, cube, {xTable + dx, yPied, zTable + dz}, {section, hPied, section}, bois);
}

struct Grille
{
	NkVector<NkMat4f> transforms;
	NkVector<NkVec3f> tints;
	NkVector<NkVec3f> centres;
	NkVec3f taille;
	NkAABB boite;
};

static Grille ConstruireGrille(uint32 n)
{
	Grille g;
	const float lx = 3.0f, ly = 1.6f, lz = 1.5f;
	const float k = std::cbrt((float)n / (lx * ly * lz));
	uint32 nx = (uint32)std::ceil(lx * k), ny = (uint32)std::ceil(ly * k), nz = (uint32)std::ceil(lz * k);
	if (nx < 1)
		nx = 1;
	if (ny < 1)
		ny = 1;
	if (nz < 1)
		nz = 1;
	const float px = lx / nx, py = ly / ny, pz = lz / nz;
	float cote = px < py ? px : py;
	cote = cote < pz ? cote : pz;
	cote *= 0.7f;
	const NkVec3f taille = {cote, cote, cote};
	g.taille = taille;
	uint32 fait = 0;
	for (uint32 i = 0; i < nx && fait < n; ++i)
	{
		for (uint32 j = 0; j < ny && fait < n; ++j)
		{
			for (uint32 l = 0; l < nz && fait < n; ++l, ++fait)
			{
				const NkVec3f c = {-1.5f + (i + 0.5f) * px, 0.8f + (j + 0.5f) * py, -1.8f + (l + 0.5f) * pz};
				g.centres.PushBack(c);
				g.transforms.PushBack(NkMat4f::Translate(c) * NkMat4f::Scale(taille));
				g.tints.PushBack({0.3f + 0.7f * (float)i / nx, 0.3f + 0.7f * (float)j / ny, 0.3f + 0.7f * (float)l / nz});
				if (fait == 0)
				{
					g.boite = Boite(c, taille);
				}
				else
				{
					const NkAABB b = Boite(c, taille);
					g.boite.Merge(b);
				}
			}
		}
	}
	return g;
}

int nkmain(const NkEntryState &state)
{
	(void)state;
	const float decalage = 0.002f;

	NkWindowConfig winCfg;
	winCfg.title = "Exo 27 - cent cubes";
	winCfg.width = 1280;
	winCfg.height = 720;
	winCfg.centered = true;
	winCfg.resizable = true;
	NkWindow window(winCfg);
	if (!window.IsValid())
	{
		logger.Error("[Exo27] fenetre KO");
		return 1;
	}

	NkDeviceInitInfo devInfo{};
	devInfo.surface = window.GetSurfaceDesc();
	devInfo.width = (uint32)window.GetSize().width;
	devInfo.height = (uint32)window.GetSize().height;
	NkIDevice *device = CreerPeripherique(devInfo);
	if (!device || !device->IsValid())
	{
		logger.Error("[Exo27] peripherique KO");
		window.Close();
		return 2;
	}
	logger.Infof("Interface graphique : %s", NkGraphicsApiName(device->GetApi()));

	NkRendererConfig cfg = NkRendererConfig::ForGame(device->GetApi(), devInfo.width, devInfo.height);
	cfg.subsystems = NK_SS_RENDER3D;
	cfg.vsync = false;
	NkRenderer *renderer = NkRenderer::Create(device, cfg);
	if (!renderer || !renderer->Initialize())
	{
		logger.Errorf("[Exo27] renderer KO : %s", NkRGetLastErrorMessage());
		NkRenderer::Destroy(renderer);
		NkDeviceFactory::Destroy(device);
		window.Close();
		return 3;
	}

	NkRender3D *r3d = renderer->GetRender3D();
	NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
	NkMeshHandle meshPlan = renderer->GetMeshSystem()->GetPlane();
	(void)meshPlan;

	bool running = true;
	uint32 W = devInfo.width;
	uint32 H = devInfo.height;
	NkEventSystem &events = NkEvents();
	events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *)
												{ running = false; });
	events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e)
											 {
		if (e->GetKey() == NkKey::NK_ESCAPE)
			running = false; });
	events.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e)
												 {
		const uint32 w = (uint32)e->GetWidth(), h = (uint32)e->GetHeight();
		if (w > 0 && h > 0 && (w != W || h != H)) {
			W = w;
			H = h;
			renderer->OnResize(w, h);
		} });

	const int echauffement = 120, mesure = 400;

	auto etape = [&](uint32 n, bool groupes, double &fps, uint32 &drawCalls) -> bool
	{
		Grille g = ConstruireGrille(n);
		NkDrawCallInstanced inst;
		inst.mesh = meshCube;
		inst.transforms = g.transforms;
		inst.tints = g.tints;
		inst.aabb = g.boite;
		std::chrono::steady_clock::time_point debut;
		for (int image = 0; image < echauffement + mesure; ++image)
		{
			events.PollEvents();
			if (!running || !window.IsOpen())
				return false;
			if (!renderer->BeginFrame())
				continue;
			if (image == echauffement)
				debut = std::chrono::steady_clock::now();
			const NkLightDesc sun = Soleil(false);
			r3d->BeginScene(Contexte((float)W / (float)H, {0.f, 1.7f, 1.5f}, {0.f, 1.4f, -2.f}, &sun));
			DessinerSalle(r3d, meshCube, meshPlan, decalage);
			if (groupes)
			{
				r3d->SubmitInstanced(inst);
			}
			else
			{
				for (uint32 i = 0; i < n; ++i)
				{
					NkDrawCall3D dc;
					dc.mesh = meshCube;
					dc.transform = g.transforms[i];
					dc.tint = g.tints[i];
					dc.aabb = Boite(g.centres[i], g.taille);
					r3d->Submit(dc);
				}
			}
			renderer->Present();
			renderer->EndFrame();
		}
		const double s = std::chrono::duration<double>(std::chrono::steady_clock::now() - debut).count();
		fps = mesure / s;
		drawCalls = renderer->GetStats().drawCalls;
		logger.Infof("%s : %u cubes, drawCalls %u, %.1f images par seconde", groupes ? "groupes" : "un par un", n,
					 drawCalls, fps);
		return true;
	};

	double base = 0.0;
	uint32 dcBase = 0;
	if (!etape(0, false, base, dcBase))
		return 0;
	const double cible = base < 60.0 ? base * 0.5 : 60.0;
	if (base < 60.0)
		logger.Infof("La salle seule tourne a %.1f images par seconde : seuil pris a %.1f", base, cible);

	auto chercher = [&](bool groupes, uint32 depart, uint32 &seuil, double &fps, uint32 &dc) -> bool
	{
		uint32 n = depart;
		for (int tour = 0; tour < 40; ++tour)
		{
			if (!etape(n, groupes, fps, dc))
				return false;
			if (fps < cible)
			{
				seuil = n;
				return true;
			}
			n = n + n / 2 + 50;
		}
		seuil = n;
		return true;
	};

	uint32 seuilA = 0, dcA = 0;
	double fpsA = 0.0;
	if (!chercher(false, 100, seuilA, fpsA, dcA))
		return 0;

	double fpsB = 0.0;
	uint32 dcB = 0;
	if (!etape(seuilA, true, fpsB, dcB))
		return 0;

	uint32 seuilB = 0, dcC = 0;
	double fpsC = 0.0;
	if (!chercher(true, seuilA, seuilB, fpsC, dcC))
		return 0;

	if (FILE *f = std::fopen("cadence.txt", "w"))
	{
		std::fprintf(f, "Un par un : %u cubes, drawCalls %u, %.1f images par seconde\n", seuilA, dcA, fpsA);
		std::fprintf(f, "Groupés : drawCalls %u, %.1f images par seconde pour les memes %u cubes, nouveau seuil %u cubes\n",
					 dcB, fpsB, seuilA, seuilB);
		std::fprintf(f, "Trouvé : NkDrawCallInstanced, declare dans NKRenderer/Core/NkRendererTypes.h, soumis par "
						"NkRender3D::SubmitInstanced (NKRenderer/Tools/Render3D/NkRender3D.h)\n");
		std::fclose(f);
	}
	logger.Info("cadence.txt ecrit. Echap pour quitter.");
	while (running && window.IsOpen())
	{
		events.PollEvents();
		if (renderer->BeginFrame())
		{
			const NkLightDesc sun = Soleil(false);
			r3d->BeginScene(Contexte((float)W / (float)H, {0.f, 1.7f, 1.5f}, {0.f, 1.4f, -2.f}, &sun));
			DessinerSalle(r3d, meshCube, meshPlan, decalage);
			renderer->Present();
			renderer->EndFrame();
		}
	}

	device->WaitIdle();
	NkRenderer::Destroy(renderer);
	NkDeviceFactory::Destroy(device);
	window.Close();
	return 0;
}
