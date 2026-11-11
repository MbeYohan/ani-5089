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
#include "NKRenderer/Core/NkRenderGraph.h"
#include "NKRenderer/Core/NkTextureLibrary.h"
#include "NKRenderer/Materials/NkMaterialCollection.h"
#include "NKRenderer/Mesh/NkMeshSystem.h"
#include "NKRenderer/Tools/Offscreen/NkOffscreenTarget.h"
#include "NKRenderer/Tools/Render3D/NkRender3D.h"
#include "NKRenderer/Tools/Render2D/NkRender2D.h"
#include "NKRenderer/Tools/Overlay/NkOverlayRenderer.h"

#include <cstdio>
#include <cstdlib>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d)
{
	d.appName = "AniExo9Cible";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

int nkmain(const NkEntryState &state)
{
	(void)state;

	const bool corrige = std::getenv("CORRIGE") != nullptr;
	const uint32 imagesVoulues = 12;
	std::FILE *journal = std::fopen(corrige ? "apres.txt" : "avant.txt", "w");
	if (!journal)
	{
		return 10;
	}

	NkWindowConfig winCfg;
	winCfg.title = "Exo 9 : cible hors ecran plus petite que la fenetre";
	winCfg.width = 1280;
	winCfg.height = 720;
	winCfg.centered = true;
	winCfg.resizable = false;
	NkWindow window(winCfg);
	if (!window.IsValid())
	{
		return 1;
	}

	NkDeviceInitInfo devInfo{};
	devInfo.surface = window.GetSurfaceDesc();
	devInfo.width = (uint32)window.GetSize().width;
	devInfo.height = (uint32)window.GetSize().height;
	NkIDevice *device = NkDeviceFactory::CreateAutoDetect(devInfo);
	if (!device || !device->IsValid())
	{
		return 2;
	}
	std::printf("backend : %s\n", NkGraphicsApiName(device->GetApi()));
	const uint32 W = devInfo.width;
	const uint32 H = devInfo.height;
	const uint32 cibleW = W / 2;
	const uint32 cibleH = H / 2;

	NkRendererConfig cfgMain = NkRendererConfig::For2D(devInfo.api, W, H);
	cfgMain.Enable(NK_SS_OFFSCREEN);
	NkRenderer *rMain = NkRenderer::Create(device, cfgMain);
	if (!rMain)
	{
		return 3;
	}

	NkOffscreenDesc od;
	od.width = cibleW;
	od.height = cibleH;
	od.hasDepth = true;
	od.hdr = false;
	od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
	od.readable = true;
	od.readback = false;
	od.name = "Exo9_Cible";
	NkOffscreenTarget *cible = rMain->CreateOffscreen(od);
	if (!cible)
	{
		return 4;
	}

	NkRendererConfig cfgEye = NkRendererConfig::ForGame(devInfo.api, cibleW, cibleH);
	cfgEye.Disable(NK_SS_RENDER2D);
	cfgEye.Disable(NK_SS_TEXT);
	cfgEye.Disable(NK_SS_OVERLAY);
	cfgEye.Enable(NK_SS_OFFSCREEN);
	NkRenderer *rEye = NkRenderer::Create(device, cfgEye);
	if (!rEye)
	{
		return 5;
	}

	rEye->SetFinalColorTarget(rMain->GetTextures()->GetRHIHandle(cible->GetColorResult()));
	if (corrige)
	{
		rEye->SetRenderSizeOverride(cibleW, cibleH);
	}

	bool running = true;
	NkEventSystem &events = NkEvents();
	events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *)
												{ running = false; });
	events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e)
											 {
		if (e->GetKey() == NkKey::NK_ESCAPE) {
			running = false;
		} });

	NkMeshHandle cube = rEye->GetMeshSystem()->GetCube();
	uint32 image = 0;
	while (running && window.IsOpen())
	{
		events.PollEvents();
		if (!running)
		{
			break;
		}
		if (!rMain->BeginFrame())
		{
			continue;
		}
		NkICommandBuffer *cmd = rMain->GetCmd();

		rEye->FlushGraphRebuilds();
		NkRender3D *r3d = rEye->GetRender3D();
		r3d->ResetFrame();
		if (auto *mc = rEye->GetMaterialCollection())
		{
			mc->Upload();
		}

		NkCamera3DData camData;
		camData.up = {0.f, 1.f, 0.f};
		camData.fovY = 60.f;
		camData.aspect = (float32)cibleW / (float32)cibleH;
		camData.nearPlane = 0.1f;
		camData.farPlane = 100.f;
		NkCamera3D cam(camData);
		cam.SetPosition({2.5f, 1.8f, 3.5f});
		cam.SetTarget({0.f, 0.5f, 0.f});

		NkSceneContext sctx;
		sctx.camera = cam;
		sctx.time = 0.f;
		NkLightDesc sun;
		sun.type = NkLightType::NK_DIRECTIONAL;
		sun.direction = {-0.4f, -1.f, -0.3f};
		sun.color = {1.f, 0.95f, 0.85f};
		sun.intensity = 3.f;
		sun.castShadow = false;
		sctx.lights.PushBack(sun);
		r3d->BeginScene(sctx);

		NkDrawCall3D dc;
		dc.mesh = cube;
		dc.transform = NkMat4f::Translate({0.f, 0.5f, 0.f});
		dc.aabb = {{-1.f, -0.5f, -1.f}, {1.f, 1.5f, 1.f}};
		dc.tint = {0.9f, 0.5f, 0.2f};
		dc.metallic = 0.f;
		dc.roughness = 0.5f;
		dc.castShadow = false;
		r3d->Submit(dc);

		if (auto *graph = rEye->GetRenderGraph())
		{
			graph->Execute(cmd);
		}

		NkOverlayRenderer *overlay = rMain->GetOverlay();
		NkRender2D *r2d = rMain->GetRender2D();
		if (overlay && r2d)
		{
			overlay->BeginOverlay(cmd, W, H);
			r2d->DrawImage(cible->GetColorResult(), {0.f, 0.f, (float32)cibleW, (float32)cibleH});
			overlay->EndOverlay();
		}

		rMain->Present();
		rMain->EndFrame();

		++image;
		std::fprintf(journal, "image : %u, etat : dessinee\n", image);
		std::fflush(journal);
		if (corrige && image >= imagesVoulues)
		{
			running = false;
		}
	}

	std::fclose(journal);
	device->WaitIdle();
	NkRenderer::Destroy(rEye);
	NkRenderer::Destroy(rMain);
	NkDeviceFactory::Destroy(device);
	window.Close();
	return 0;
}
