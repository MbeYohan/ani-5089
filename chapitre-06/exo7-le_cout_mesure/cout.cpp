#include "NKPlatform/NkPlatformDetect.h"
#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventDispatcher.h"
#include "NKEvent/NkKeyboardEvent.h"
#include "NKEvent/NkMouseEvent.h"
#include "NKEvent/NkWindowEvent.h"
#include "NKLogger/NkLog.h"
#include "NKMath/NkQuat.h"
#include "NKRHI/Core/NkDeviceFactory.h"
#include "NKRenderer/NkRenderer.h"
#include "NKRenderer/Core/NkCamera.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKTime/NkChrono.h"
#include "NKMath/NkAngle.h"
#include "NKMath/NkMat.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d)
{
	d.appName = "S6Cout";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const int kIterations = 1000000;

static NkQuatf ParMatrices(float lacetDeg, float tangageDeg)
{
	const NkMat4f rotation = NkMat4f::RotationY(NkAngle(lacetDeg)) * NkMat4f::RotationX(NkAngle(tangageDeg));
	return NkQuatf(rotation).Normalized();
}

static NkQuatf ParQuaternions(float lacetDeg, float tangageDeg)
{
	return (NkQuatf::RotateY(NkAngle(lacetDeg)) * NkQuatf::RotateX(NkAngle(tangageDeg))).Normalized();
}

static float LacetI(int i)
{
	return (i % 3600) * 0.1f;
}

static float TangageI(int i)
{
	return (i % 1800) * 0.05f;
}

static double BoucleMatrices()
{
	double somme = 0.0;
	for (int i = 0; i < kIterations; i++)
	{
		const NkQuatf q = ParMatrices(LacetI(i), TangageI(i));
		somme += q.x + q.y + q.z + q.w;
	}
	return somme;
}

static double BoucleQuaternions()
{
	double somme = 0.0;
	for (int i = 0; i < kIterations; i++)
	{
		const NkQuatf q = ParQuaternions(LacetI(i), TangageI(i));
		somme += q.x + q.y + q.z + q.w;
	}
	return somme;
}

static void MesurerEtEcrire()
{
#ifdef NDEBUG
	const char *config = "Release";
	std::FILE *f = std::fopen("mesures.txt", "a");
#else
	const char *config = "Debug";
	std::FILE *f = std::fopen("mesures.txt", "w");
#endif
	std::printf("construction : %s\n", config);

	const NkQuatf a = ParMatrices(45.f, 30.f);
	NkQuatf b = ParQuaternions(45.f, 30.f);
	if (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w < 0.f)
		b = NkQuatf(-b.x, -b.y, -b.z, -b.w);
	std::printf("lacet 45, tangage 30 : matrices (%.6f, %.6f, %.6f, %.6f)\n", a.x, a.y, a.z, a.w);
	std::printf("lacet 45, tangage 30 : quaternions (%.6f, %.6f, %.6f, %.6f)\n", b.x, b.y, b.z, b.w);
	std::printf("ecart le plus grand : %.3e\n",
				std::fmax(std::fmax(std::fabs(a.x - b.x), std::fabs(a.y - b.y)),
						  std::fmax(std::fabs(a.z - b.z), std::fabs(a.w - b.w))));

	double sommeM = 0.0;
	double sommeQ = 0.0;
	for (int essai = 0; essai < 3; essai++)
	{
		NkChrono c1;
		sommeM = BoucleMatrices();
		const double tM = c1.Elapsed().ToMilliseconds();

		NkChrono c2;
		sommeQ = BoucleQuaternions();
		const double tQ = c2.Elapsed().ToMilliseconds();

		std::printf("%s, matrices : %.3f ms\n", config, tM);
		std::printf("%s, quaternions : %.3f ms\n", config, tQ);
		if (f)
		{
			std::fprintf(f, "%s, matrices : %.3f ms\n", config, tM);
			std::fprintf(f, "%s, quaternions : %.3f ms\n", config, tQ);
		}
	}
	if (f)
		std::fclose(f);
	std::printf("sommes lues : %.6f et %.6f\n", sommeM, sommeQ);
}

static NkIDevice *CreerPeripherique(NkDeviceInitInfo &info)
{
	const char *nom = std::getenv("API");
	if (!nom)
		nom = "dx11";
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

static void Pave(NkRender3D *r3d, NkMeshHandle cube, NkVec3f centre, NkVec3f taille, NkVec3f teinte, bool ombre = true)
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

static NkLightDesc Soleil()
{
	NkLightDesc sun;
	sun.type = NkLightType::NK_DIRECTIONAL;
	sun.direction = {-0.4f, -1.f, -0.3f};
	sun.color = {1.f, 0.95f, 0.85f};
	sun.intensity = 3.f;
	sun.castShadow = false;
	sun.shadowStatic = false;
	return sun;
}

static NkSceneContext Contexte(float aspect, NkVec3f position, NkVec3f cible, NkVec3f haut, const NkLightDesc *soleil)
{
	NkCamera3DData camData;
	camData.up = haut;
	camData.fovY = 70.f;
	camData.aspect = aspect;
	camData.nearPlane = 0.05f;
	camData.farPlane = 200.f;
	NkCamera3D cam(camData);
	cam.SetPosition(position);
	cam.SetTarget(cible);
	cam.SetUp(haut);
	NkSceneContext sctx;
	sctx.camera = cam;
	if (soleil)
		sctx.lights.PushBack(*soleil);
	return sctx;
}

static void DessinerSalle(NkRender3D *r3d, NkMeshHandle cube, NkMeshHandle plan)
{
	const float decalage = 0.002f;
	Sol(r3d, plan);
	const NkVec3f mur = {0.72f, 0.70f, 0.66f};
	Pave(r3d, cube, {0.f, 1.25f, -2.f}, {4.f, 2.5f, 0.1f}, mur);
	Pave(r3d, cube, {0.f, 1.25f, 2.f}, {4.f, 2.5f, 0.1f}, mur);
	Pave(r3d, cube, {-2.f, 1.25f, 0.f}, {0.1f, 2.5f, 4.f}, mur);
	Pave(r3d, cube, {2.f, 1.25f, 0.f}, {0.1f, 2.5f, 4.f}, mur);
	Pave(r3d, cube, {0.f, 2.55f, 0.f}, {4.1f, 0.1f, 4.1f}, {0.85f, 0.85f, 0.85f}, false);
	const NkVec3f porte = {0.40f, 0.25f, 0.12f};
	const NkVec3f vitre = {0.45f, 0.65f, 0.85f};
	const float face = 1.95f;
	const float e = 0.02f;
	Pave(r3d, cube, {-0.70f, 1.f, -face + decalage - e * 0.5f}, {0.9f, 2.f, e}, porte);
	Pave(r3d, cube, {-face + decalage - e * 0.5f, 1.4f, -0.9f}, {e, 1.f, 1.2f}, vitre);
	Pave(r3d, cube, {-face + decalage - e * 0.5f, 1.4f, 0.9f}, {e, 1.f, 1.2f}, vitre);
	const NkVec3f bois = {0.45f, 0.30f, 0.18f};
	const float longueur = 1.20f, profondeur = 0.80f;
	const float xTable = 0.60f, zTable = 0.70f;
	const float dessus = 0.75f, epPlateau = 0.05f, section = 0.06f;
	const float yPlateau = dessus - epPlateau * 0.5f;
	const float hPied = dessus - epPlateau;
	const float yPied = hPied * 0.5f;
	const float dx = longueur * 0.5f - section * 0.5f;
	const float dz = profondeur * 0.5f - section * 0.5f;
	Pave(r3d, cube, {xTable, yPlateau, zTable}, {longueur, epPlateau, profondeur}, bois);
	Pave(r3d, cube, {xTable - dx, yPied, zTable - dz}, {section, hPied, section}, bois);
	Pave(r3d, cube, {xTable + dx, yPied, zTable - dz}, {section, hPied, section}, bois);
	Pave(r3d, cube, {xTable - dx, yPied, zTable + dz}, {section, hPied, section}, bois);
	Pave(r3d, cube, {xTable + dx, yPied, zTable + dz}, {section, hPied, section}, bois);
}

int nkmain(const NkEntryState &state)
{
	(void)state;

	MesurerEtEcrire();
	const char *sans = std::getenv("MASALLE_SANS_FENETRE");
	if (sans && std::strcmp(sans, "1") == 0)
		return 0;

	NkWindowConfig winCfg;
	winCfg.title = "Exo 7 - le cout mesure";
	winCfg.width = 1280;
	winCfg.height = 720;
	winCfg.centered = true;
	winCfg.resizable = true;
	NkWindow window(winCfg);
	if (!window.IsValid())
	{
		logger.Error("[S6Cout] fenetre KO");
		return 1;
	}

	NkDeviceInitInfo devInfo{};
	devInfo.surface = window.GetSurfaceDesc();
	devInfo.width = (uint32)window.GetSize().width;
	devInfo.height = (uint32)window.GetSize().height;
	NkIDevice *device = CreerPeripherique(devInfo);
	if (!device || !device->IsValid())
	{
		logger.Error("[S6Cout] peripherique KO");
		window.Close();
		return 2;
	}

	NkRendererConfig cfg = NkRendererConfig::ForGame(device->GetApi(), devInfo.width, devInfo.height);
	cfg.subsystems = NK_SS_RENDER3D;
	cfg.vsync = true;
	NkRenderer *renderer = NkRenderer::Create(device, cfg);
	if (!renderer || !renderer->Initialize())
	{
		logger.Errorf("[S6Cout] renderer KO : %s", NkRGetLastErrorMessage());
		NkRenderer::Destroy(renderer);
		NkDeviceFactory::Destroy(device);
		window.Close();
		return 3;
	}

	NkRender3D *r3d = renderer->GetRender3D();
	NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
	NkMeshHandle meshPlan = renderer->GetMeshSystem()->GetPlane();

	bool running = true;
	uint32 W = devInfo.width;
	uint32 H = devInfo.height;
	NkEventSystem &events = NkEvents();
	events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *)
												{ running = false; });
	events.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e)
												 {
		const uint32 w = (uint32)e->GetWidth(), h = (uint32)e->GetHeight();
		if (w > 0 && h > 0 && (w != W || h != H)) {
			W = w;
			H = h;
			renderer->OnResize(w, h);
		} });
	events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e)
											 {
		if (e->GetKey() == NkKey::NK_ESCAPE)
			running = false; });

	const NkVec3f position = {0.f, 1.70f, 0.f};
	const NkVec3f cible = {0.f, 1.40f, -2.f};
	const NkLightDesc sun = Soleil();
	while (running && window.IsOpen())
	{
		events.PollEvents();
		if (!renderer->BeginFrame())
			continue;
		r3d->BeginScene(Contexte((float)W / (float)H, position, cible, NkVec3f(0.f, 1.f, 0.f), &sun));
		DessinerSalle(r3d, meshCube, meshPlan);
		renderer->Present();
		renderer->EndFrame();
	}

	device->WaitIdle();

	NkRenderer::Destroy(renderer);
	NkDeviceFactory::Destroy(device);
	window.Close();
	return 0;
}
