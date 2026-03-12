#include "vk/VKContext.h"
#include "vk/VKDebug.h"
#include "vk/VKDescriptorLayouts.h"
#include "vk/VKDescriptorPool.h"
#include "vk/VKDescriptorPoolExt.h"
#include "vk/VKDevice.h"
#include "vk/VKInstance.h"
#include "vk/VKPhysicalDeviceEnumerations.h"
#include "vk/VKPipelineTex.h"
#include "vk/VKSampler.h"
#include "vk/VKScaler.h"
#include "vk/VKShaders.h"
#include "vk/VKSwapChain.h"
#include "vk/VKSwapChainFramebuffers.h"
#include "vk/VKUtil.h"

#include "glfw/VKGLFW.h"
#include "glfw/VKSurface.h"

#include "gl/GLConsole.h"
#include "gl/GLGlyphCache.h"

#include "pulse/PulseSoundEngine.h"

#include "util/Codepage.h"
#include "util/FileListing.h"
#include "util/StringView.h"
#include "util/Thread.h"

#include "ConsoleContext.h"
#include "PicContext.h"
#include "exception.h"
#include "fdd/diskdrv.h"
#include "inputhandling.h"
#include "loop.h"
#include "np2.h"
#include "pccore.h"
#include "pccorecallbacks.h"
#include "util/Matrix4x4.h"
#include "util/Vertex.h"

#include "cpumem.h"

namespace BR {

#ifdef VKDEBUG
static int vkdebug = 1;
#else
static int vkdebug = 0;
#endif

static const char *validationLayerString = "VK_LAYER_KHRONOS_validation";

static bool
shouldWindowsClose(std::vector<std::unique_ptr<MainContext>> &contexts) {
    for (auto &context : contexts) {
        if (context->windowContext->glfwSurface.getWindowState() ==
            WindowState::SHOULDCLOSE) {
            return true;
        }
    }

    return false;
}

void loop(SignalFD &sfd, InputMapper &inputMapper, NP2CFG &cfg, NP2OSCFG &oscfg,
          Sfx::PulseSoundEngine &soundEngine, const std::string &diskDir) {

    (void)cfg;
    (void)oscfg;
    (void)diskDir;

    vkdebug = 1;

    GLFWContext glfwContext;

    auto extensions = getRequiredGLFWExtensions();
    std::vector<const char *> layers;

    if (vkdebug) {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        layers.push_back(validationLayerString);
    }

    VulkanInstance instance(extensions, layers);
    VulkanDebugCallback callback(instance, vkdebug);

    auto firstWindow =
        std::make_unique<VulkanWindowContext>(instance, 640, 480, "XNP2Katze-Main");

    VulkanPhysicalDevice physicalDevice =
        glPhysicalDeviceSelection(instance, firstWindow->vulkanSurface);

    VulkanDevice device(vkdebug, physicalDevice, firstWindow->vulkanSurface,
                        physicalDevice.queueFamilies);

    ShaderStore shaderStore(device);
    VulkanDescriptorLayouts layouts(device);

    VulkanSampler sampler(physicalDevice, device, VK_FILTER_NEAREST);
    VkFormat renderDepthFormat = findDepthFormat(physicalDevice);

    std::vector<std::unique_ptr<MainContext>> contexts;

    Mutex globalMutex;

    contexts.emplace_back(std::make_unique<PicContext>(
        device, physicalDevice, std::move(firstWindow), sampler, layouts,
        soundEngine, inputMapper, globalMutex));

    MainContext &context = *contexts.back();

    VulkanWindowContext &windowContext = *(context.windowContext);

    SwapChainSupportDetails swapChainSupport(physicalDevice,
                                             windowContext.vulkanSurface);

    VkSurfaceFormatKHR surfaceFormat =
        chooseSwapSurfaceFormat(swapChainSupport.formats);

    VulkanRenderPass renderPass(device, surfaceFormat.format, renderDepthFormat,
                                ShouldPresent::YES);

    auto secondWindow =
        std::make_unique<VulkanWindowContext>(instance, 640, 480, "XNP2Katze-Console");

    contexts.emplace_back(std::make_unique<ConsoleContext>(
        device, physicalDevice, std::move(secondWindow), sampler, layouts,
        diskDir, oscfg, globalMutex));

    while (!shouldWindowsClose(contexts) && !sfd.isTriggered()) {
        glfwContext.pollWindowEvents();

        for (auto &context : contexts) {
            context->work();
            context->reset();
        }

        for (auto &context : contexts) {
            if (context->isOutdated()) {
                context->createRender(renderPass, renderDepthFormat,
                                      shaderStore, layouts);
            }
        }

        for (auto &context : contexts) {
            if (!context->canCurrentlyRender()) {
                continue;
            }

            context->frame();
            context->render(device, renderPass);
        }
    }

    for (auto &context : contexts) {
        while (!context->canCurrentlyRender()) {
            usleep(1000);
        }
    }

    vkDeviceWaitIdle(device);
}

} // namespace BR
