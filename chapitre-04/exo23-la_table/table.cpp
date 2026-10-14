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
	d.appName = "Exo23";
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

int nkmain(const NkEntryState &state)
{
	(void)state;
	const float decalage = 0.002f;

	NkWindowConfig winCfg;
	winCfg.title = "Exo 23 - la table";
	winCfg.width = 1280;
	winCfg.height = 720;
	winCfg.centered = true;
	winCfg.resizable = true;
	NkWindow window(winCfg);
	if (!window.IsValid())
	{
		logger.Error("[Exo23] fenetre KO");
		return 1;
	}

	NkDeviceInitInfo devInfo{};
	devInfo.surface = window.GetSurfaceDesc();
	devInfo.width = (uint32)window.GetSize().width;
	devInfo.height = (uint32)window.GetSize().height;
	NkIDevice *device = CreerPeripherique(devInfo);
	if (!device || !device->IsValid())
	{
		logger.Error("[Exo23] peripherique KO");
		window.Close();
		return 2;
	}
	logger.Infof("Interface graphique : %s", NkGraphicsApiName(device->GetApi()));

	NkRendererConfig cfg = NkRendererConfig::ForGame(device->GetApi(), devInfo.width, devInfo.height);
	cfg.subsystems = NK_SS_RENDER3D | NK_SS_SHADOW;
	cfg.vsync = true;
	NkRenderer *renderer = NkRenderer::Create(device, cfg);
	if (!renderer || !renderer->Initialize())
	{
		logger.Errorf("[Exo23] renderer KO : %s", NkRGetLastErrorMessage());
		NkRenderer::Destroy(renderer);
		NkDeviceFactory::Destroy(device);
		window.Close();
		return 3;
	}

	if (auto *shadow = renderer->GetShadow())
		shadow->GetConfig().autoFitDirectional = true;

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

	while (running && window.IsOpen())
	{
		events.PollEvents();
		if (!running)
			break;
		if (!renderer->BeginFrame())
			continue;
		const NkLightDesc sun = Soleil(true);
		r3d->BeginScene(Contexte((float)W / (float)H, {-1.4f, 1.7f, 1.6f}, {0.5f, 0.6f, 0.1f}, &sun));
		DessinerSalle(r3d, meshCube, meshPlan, decalage);
		renderer->Present();
		renderer->EndFrame();
	}

	device->WaitIdle();
	NkRenderer::Destroy(renderer);
	NkDeviceFactory::Destroy(device);
	window.Close();
	return 0;
}
