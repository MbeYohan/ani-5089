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
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterial.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKFileSystem/NkPath.h"
#include "NKFont/NkFont.h"
#include "NKFont/Embedded/NkFontEmbedded.h"
#include "NKImage/Core/NkImage.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d)
{
	d.appName = "S5Panneau";
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

static void PaveMateriau(NkRender3D *r3d, NkMeshHandle cube, NkVec3f centre, NkVec3f taille, NkMatInstHandle materiau,
						 bool ombre = true)
{
	NkDrawCall3D dc;
	dc.mesh = cube;
	dc.transform = NkMat4f::Translate(centre) * NkMat4f::Scale(taille);
	dc.aabb = Boite(centre, taille);
	dc.tint = {1.f, 1.f, 1.f};
	dc.roughness = 0.9f;
	dc.castShadow = ombre;
	dc.material = materiau;
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

static void ChoisirVue(NkVec3f &position, NkVec3f &cible)
{
	position = {0.f, 1.7f, 1.5f};
	cible = {0.f, 1.4f, -2.f};
	const char *vue = std::getenv("VUE");
	if (!vue)
		return;
	if (std::strcmp(vue, "3m") == 0)
	{
		position = {-0.70f, 1.70f, 1.00f};
		cible = {-0.70f, 2.20f, -1.95f};
	}
	else if (std::strcmp(vue, "50cm") == 0)
	{
		position = {-0.70f, 2.20f, -1.50f};
		cible = {-0.70f, 2.20f, -1.95f};
	}
	else if (std::strcmp(vue, "fenetre") == 0)
	{
		position = {-1.50f, 1.40f, -0.90f};
		cible = {-1.50f, 1.40f, -2.00f};
	}
	else if (std::strcmp(vue, "centre") == 0)
	{
		position = {0.f, 1.40f, 0.f};
		cible = {0.f, 1.40f, -2.f};
	}
	else if (std::strcmp(vue, "coin") == 0)
	{
		position = {1.80f, 1.40f, 1.80f};
		cible = {1.80f, 1.40f, 0.f};
	}
}

static void DessinerSalle(NkRender3D *r3d, NkMeshHandle cube, NkMeshHandle plan, bool murTexture, NkMatInstHandle murMateriau)
{
	const float decalage = 0.002f;
	Sol(r3d, plan);
	const NkVec3f mur = {0.72f, 0.70f, 0.66f};
	if (murTexture)
		PaveMateriau(r3d, cube, {0.f, 1.25f, -2.f}, {4.f, 2.5f, 0.1f}, murMateriau);
	else
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

static bool FabriquerPanneau(NkImage &image, int &atlasL, int &atlasH)
{
	const char *mot = "Sortie";
	NkFontAtlas atlas;
	NkFont *police = NkFontEmbedded::AddToAtlas(atlas, NkEmbeddedFontId::Inter, 32.f);
	if (!police || !atlas.Build())
		return false;
	nkft_uint8 *pixels = nullptr;
	atlas.GetTexDataAsAlpha8(&pixels, &atlasL, &atlasH);
	if (!pixels)
		return false;

	float largeurF = 0.f;
	for (const char *c = mot; *c; ++c)
	{
		const NkFontGlyph *g = police->FindGlyph((NkFontCodepoint)(unsigned char)*c);
		if (!g)
			return false;
		largeurF += g->advanceX;
	}
	const int largeur = (int)std::ceil(largeurF);
	const int hauteur = (int)std::ceil(police->ascent - police->descent);
	const int base = (int)std::ceil(police->ascent);

	const int fondR = 0x1B, fondG = 0x5E, fondB = 0x20;
	image = NkImage::Create((uint32)largeur, (uint32)hauteur, 4, 0x1B5E20FFu);
	if (!image.IsValid())
		return false;

	float curseur = 0.f;
	for (const char *c = mot; *c; ++c)
	{
		const NkFontGlyph *g = police->FindGlyph((NkFontCodepoint)(unsigned char)*c);
		const int xDebut = (int)std::floor(curseur + g->x0);
		const int xFin = (int)std::ceil(curseur + g->x1);
		const int yDebut = (int)std::floor(base + g->y0);
		const int yFin = (int)std::ceil(base + g->y1);
		const float rectL = g->x1 - g->x0;
		const float rectH = g->y1 - g->y0;
		for (int y = yDebut; y < yFin; ++y)
		{
			for (int x = xDebut; x < xFin; ++x)
			{
				if (x < 0 || y < 0 || x >= largeur || y >= hauteur)
					continue;
				const float fx = ((x + 0.5f) - (curseur + g->x0)) / rectL;
				const float fy = ((y + 0.5f) - (base + g->y0)) / rectH;
				if (fx < 0.f || fx > 1.f || fy < 0.f || fy > 1.f)
					continue;
				int ax = (int)((g->u0 + fx * (g->u1 - g->u0)) * atlasL);
				int ay = (int)((g->v0 + fy * (g->v1 - g->v0)) * atlasH);
				if (ax < 0)
					ax = 0;
				if (ay < 0)
					ay = 0;
				if (ax >= atlasL)
					ax = atlasL - 1;
				if (ay >= atlasH)
					ay = atlasH - 1;
				const float couverture = pixels[ay * atlasL + ax] / 255.0f;
				const int r = (int)std::lround(255 * couverture + fondR * (1.f - couverture));
				const int gr = (int)std::lround(255 * couverture + fondG * (1.f - couverture));
				const int b = (int)std::lround(255 * couverture + fondB * (1.f - couverture));
				image.SetPixel(x, y, math::NkColor((uint8)r, (uint8)gr, (uint8)b, 255));
			}
		}
		curseur += g->advanceX;
	}
	return true;
}

int nkmain(const NkEntryState &state)
{
	(void)state;

	NkWindowConfig winCfg;
	winCfg.title = "Exo 7 - un panneau au-dessus de la porte";
	winCfg.width = 1280;
	winCfg.height = 720;
	winCfg.centered = true;
	winCfg.resizable = true;
	NkWindow window(winCfg);
	if (!window.IsValid())
	{
		logger.Error("[S5Panneau] fenetre KO");
		return 1;
	}

	NkDeviceInitInfo devInfo{};
	devInfo.surface = window.GetSurfaceDesc();
	devInfo.width = (uint32)window.GetSize().width;
	devInfo.height = (uint32)window.GetSize().height;
	NkIDevice *device = CreerPeripherique(devInfo);
	if (!device || !device->IsValid())
	{
		logger.Error("[S5Panneau] peripherique KO");
		window.Close();
		return 2;
	}

	logger.Infof("Interface graphique : %s", NkGraphicsApiName(device->GetApi()));
	NkRendererConfig cfg = NkRendererConfig::ForGame(device->GetApi(), devInfo.width, devInfo.height);
	cfg.subsystems = NK_SS_RENDER3D;
	cfg.vsync = true;
	NkRenderer *renderer = NkRenderer::Create(device, cfg);
	if (!renderer || !renderer->Initialize())
	{
		logger.Errorf("[S5Panneau] renderer KO : %s", NkRGetLastErrorMessage());
		NkRenderer::Destroy(renderer);
		NkDeviceFactory::Destroy(device);
		window.Close();
		return 3;
	}

	NkRender3D *r3d = renderer->GetRender3D();
	NkMeshHandle meshCube = renderer->GetMeshSystem()->GetCube();
	NkMeshHandle meshPlan = renderer->GetMeshSystem()->GetPlane();

	NkImage panneau;
	int atlasL = 0, atlasH = 0;
	if (!FabriquerPanneau(panneau, atlasL, atlasH) || !panneau.SavePNG("panneau.png"))
	{
		logger.Error("[S5Panneau] panneau impossible a fabriquer");
		NkRenderer::Destroy(renderer);
		NkDeviceFactory::Destroy(device);
		window.Close();
		return 4;
	}
	logger.Infof("atlas : %d x %d", atlasL, atlasH);
	logger.Infof("image : %d x %d", panneau.Width(), panneau.Height());

	NkImage affiche = panneau.Copy();
	affiche.FlipHorizontal();
	affiche.SavePNG("panneau-affiche.png");
	NkTexHandle texture = renderer->GetTextures()->Load("panneau-affiche.png");
	NkMaterial *materiau = NkMaterial::Create(renderer->GetMaterials(), NkMaterialType::NK_PBR_METALLIC);
	materiau->SetAlbedoMap(texture);
	const NkMatInstHandle panneauMateriau = materiau->GetInstHandle();
	const float hautPanneau = 0.20f;
	const float largPanneau = hautPanneau * (float)panneau.Width() / (float)panneau.Height();

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

	NkVec3f positionCamera, cibleCamera;
	ChoisirVue(positionCamera, cibleCamera);
	const NkLightDesc sun = Soleil();

	while (running && window.IsOpen())
	{
		events.PollEvents();
		if (!renderer->BeginFrame())
			continue;
		r3d->BeginScene(Contexte((float)W / (float)H, positionCamera, cibleCamera, &sun));
		DessinerSalle(r3d, meshCube, meshPlan, false, NkMatInstHandle());
		PaveMateriau(r3d, meshCube, {-0.70f, 2.20f, -1.945f}, {largPanneau, hautPanneau, 0.01f}, panneauMateriau, false);
		renderer->Present();
		renderer->EndFrame();
	}

	device->WaitIdle();

	NkRenderer::Destroy(renderer);
	NkDeviceFactory::Destroy(device);
	window.Close();
	return 0;
}
