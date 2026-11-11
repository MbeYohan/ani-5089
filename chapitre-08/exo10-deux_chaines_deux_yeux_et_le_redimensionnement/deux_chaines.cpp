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
#include "NKXR/NkXrSwapchain.h"

#include <cstdio>

using namespace nkentseu;
using namespace nkentseu::renderer;

static void ConfigureAppData(NkAppData &d)
{
	d.appName = "AniExo10DeuxChaines";
}
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

static const uint32 kOeil = 2;
static const char *kNom[kOeil] = {"gauche", "droit"};

static const uint32 kImages[kOeil] = {3, 2};
static const uint32 kMaxImages = 3;

int nkmain(const NkEntryState &state)
{
	(void)state;

	std::FILE *trace = std::fopen("trace.txt", "w");
	std::FILE *resize = std::fopen("resize.txt", "w");
	if (!trace || !resize)
	{
		return 10;
	}

	NkWindowConfig winCfg;
	winCfg.title = "Exo 10 : deux chaines, deux yeux (redimensionne la fenetre)";
	winCfg.width = 1280;
	winCfg.height = 720;
	winCfg.centered = true;
	winCfg.resizable = true;
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
	uint32 W = devInfo.width;
	uint32 H = devInfo.height;
	uint32 eyeW = W / 2;
	uint32 eyeH = H;

	NkRendererConfig cfgMain = NkRendererConfig::For2D(devInfo.api, W, H);
	cfgMain.Enable(NK_SS_OFFSCREEN);
	NkRenderer *rMain = NkRenderer::Create(device, cfgMain);
	if (!rMain)
	{
		return 3;
	}

	NkOffscreenTarget *cible[kOeil][kMaxImages] = {};
	NkRenderer *rEye[kOeil] = {};
	xr::NkXrSwapchain *chaine[kOeil] = {};

	for (uint32 e = 0; e < kOeil; ++e)
	{
		for (uint32 i = 0; i < kImages[e]; ++i)
		{
			NkOffscreenDesc od;
			od.width = eyeW;
			od.height = eyeH;
			od.hasDepth = true;
			od.hdr = false;
			od.colorFmt = NkGPUFormat::NK_RGBA8_UNORM;
			od.readable = true;
			od.readback = false;
			od.name = (e == 0) ? "Exo10_Gauche" : "Exo10_Droit";
			cible[e][i] = rMain->CreateOffscreen(od);
			if (!cible[e][i])
			{
				return 4;
			}
		}
		NkRendererConfig cfgEye = NkRendererConfig::ForGame(devInfo.api, eyeW, eyeH);
		cfgEye.Disable(NK_SS_RENDER2D);
		cfgEye.Disable(NK_SS_TEXT);
		cfgEye.Disable(NK_SS_OVERLAY);
		cfgEye.Enable(NK_SS_OFFSCREEN);
		rEye[e] = NkRenderer::Create(device, cfgEye);
		if (!rEye[e])
		{
			return 5;
		}
		rEye[e]->SetRenderSizeOverride(eyeW, eyeH);
	}

	auto handle = [&](uint32 e, uint32 i) -> uint64
	{
		return (uint64)rMain->GetTextures()->GetRHIHandle(cible[e][i]->GetColorResult()).id;
	};

	auto creerChaine = [&](uint32 e, bool journalise)
	{
		chaine[e] = new xr::NkXrSwapchain();
		xr::NkXrSwapchainDesc sd;
		sd.width = eyeW;
		sd.height = eyeH;
		sd.imageCount = kImages[e];
		sd.eye = (e == 0) ? xr::NkXrEye::NK_XR_EYE_LEFT : xr::NkXrEye::NK_XR_EYE_RIGHT;
		sd.name = (e == 0) ? "Exo10_Gauche" : "Exo10_Droit";
		chaine[e]->Init(sd);
		if (journalise)
		{
			std::fprintf(resize, "recreation de la chaine %s : %u images, %ux%u\n", kNom[e], kImages[e], eyeW, eyeH);
		}
		for (uint32 i = 0; i < kImages[e]; ++i)
		{
			chaine[e]->SetImageNative(i, handle(e, i));
			if (journalise)
			{
				std::fprintf(resize, "reenregistrement chaine %s, image %u : poignee %llu\n", kNom[e], i,
							 (unsigned long long)handle(e, i));
			}
		}
	};

	creerChaine(0, false);
	creerChaine(1, false);

	bool running = true;
	bool pending = false;
	uint32 pendingW = 0;
	uint32 pendingH = 0;
	NkEventSystem &events = NkEvents();
	events.AddEventCallback<NkWindowCloseEvent>([&](NkWindowCloseEvent *)
												{ running = false; });
	events.AddEventCallback<NkKeyPressEvent>([&](NkKeyPressEvent *e)
											 {
		if (e->GetKey() == NkKey::NK_ESCAPE) {
			running = false;
		} });
	events.AddEventCallback<NkWindowResizeEvent>([&](NkWindowResizeEvent *e)
												 {
		const uint32 w = (uint32)e->GetWidth();
		const uint32 h = (uint32)e->GetHeight();
		if (w > 1 && h > 0 && (w != W || h != H)) {
			pending = true;
			pendingW = w;
			pendingH = h;
		} });

	NkMeshHandle cube = rEye[0]->GetMeshSystem()->GetCube();
	uint32 image = 0;
	uint32 apresResize = 0;
	while (running && window.IsOpen())
	{
		events.PollEvents();
		if (!running)
		{
			break;
		}

		if (pending)
		{
			pending = false;
			W = pendingW;
			H = pendingH;
			eyeW = W / 2;
			eyeH = H;
			std::fprintf(resize, "fenetre redimensionnee : %ux%u, yeux %ux%u\n", W, H, eyeW, eyeH);
			rMain->OnResize(W, H);
			for (uint32 e = 0; e < kOeil; ++e)
			{
				std::fprintf(resize, "destruction de la chaine %s : %u images\n", kNom[e], kImages[e]);
				delete chaine[e];
				chaine[e] = nullptr;
			}
			for (uint32 e = 0; e < kOeil; ++e)
			{
				for (uint32 i = 0; i < kImages[e]; ++i)
				{
					cible[e][i]->Resize(eyeW, eyeH);
					std::fprintf(resize, "redimensionnement de la cible %s, image %u : %ux%u\n", kNom[e], i, eyeW, eyeH);
				}
				rEye[e]->SetRenderSizeOverride(eyeW, eyeH);
				std::fprintf(resize, "taille de rendu forcee, oeil %s : %ux%u\n", kNom[e], eyeW, eyeH);
			}
			creerChaine(0, true);
			creerChaine(1, true);
			std::fflush(resize);
			apresResize = 5;
		}

		if (!rMain->BeginFrame())
		{
			continue;
		}
		NkICommandBuffer *cmd = rMain->GetCmd();

		uint32 prete[kOeil] = {};
		for (uint32 e = 0; e < kOeil; ++e)
		{
			uint32 index = 0;
			chaine[e]->AcquireImage(index);
			prete[e] = index;

			rEye[e]->SetFinalColorTarget(rMain->GetTextures()->GetRHIHandle(cible[e][index]->GetColorResult()));
			rEye[e]->FlushGraphRebuilds();
			NkRender3D *r3d = rEye[e]->GetRender3D();
			r3d->ResetFrame();
			if (auto *mc = rEye[e]->GetMaterialCollection())
			{
				mc->Upload();
			}

			NkCamera3DData camData;
			camData.up = {0.f, 1.f, 0.f};
			camData.fovY = 60.f;
			camData.aspect = (float32)eyeW / (float32)eyeH;
			camData.nearPlane = 0.1f;
			camData.farPlane = 100.f;
			NkCamera3D cam(camData);
			const float32 decalage = (e == 0) ? -0.06f : 0.06f;
			cam.SetPosition({2.5f + decalage, 1.8f, 3.5f});
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
			dc.tint = (e == 0) ? NkVec3f{0.9f, 0.4f, 0.2f} : NkVec3f{0.2f, 0.6f, 0.9f};
			dc.metallic = 0.f;
			dc.roughness = 0.5f;
			dc.castShadow = false;
			r3d->Submit(dc);

			if (auto *graph = rEye[e]->GetRenderGraph())
			{
				graph->Execute(cmd);
			}
			chaine[e]->ReleaseImage();
		}

		NkOverlayRenderer *overlay = rMain->GetOverlay();
		NkRender2D *r2d = rMain->GetRender2D();
		if (overlay && r2d)
		{
			overlay->BeginOverlay(cmd, W, H);
			const uint32 g = chaine[0]->GetLastReleasedIndex();
			const uint32 d = chaine[1]->GetLastReleasedIndex();
			r2d->DrawImage(cible[0][g]->GetColorResult(), {0.f, 0.f, (float32)W * 0.5f, (float32)H});
			r2d->DrawImage(cible[1][d]->GetColorResult(), {(float32)W * 0.5f, 0.f, (float32)W * 0.5f, (float32)H});
			overlay->EndOverlay();
		}

		rMain->Present();
		rMain->EndFrame();

		++image;
		if (image <= 30)
		{
			std::fprintf(trace, "image : %u, gauche : %u, droit : %u\n", image, prete[0], prete[1]);
			std::fflush(trace);
		}
		if (apresResize > 0)
		{
			std::printf("apres redimensionnement : image %u, gauche : %u, droit : %u\n", image, prete[0], prete[1]);
			--apresResize;
		}
	}

	std::fclose(trace);
	std::fclose(resize);
	device->WaitIdle();
	for (uint32 e = 0; e < kOeil; ++e)
	{
		delete chaine[e];
		NkRenderer::Destroy(rEye[e]);
	}
	NkRenderer::Destroy(rMain);
	NkDeviceFactory::Destroy(device);
	window.Close();
	return 0;
}
