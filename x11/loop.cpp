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

static const unsigned int pc98Width = 640;
static const unsigned int pc98Height = 400;

static void list(GLConsole &console, const char *str, UINT32 val,
                 UINT32 offset = 0) {
    LineColor<80> lineColor;

    lineColor << FormatString{str, 0} << FormatPad{24 + offset, 0}
              << FormatSize{val, 2};

    console.addLine(lineColor);
}

static void listIdx(GLConsole &console, size_t idx, const char *str) {
    LineColor<80> lineColor;

    lineColor << FormatSize{idx, 0} << FormatString{": ", 2} << FormatPad{8, 0}
              << FormatString{str, 0};

    console.addLine(lineColor);
}

static void listSep(GLConsole &console, const char *sentinel, const char *str) {
    LineColor<80> lineColor;

    lineColor << FormatString{sentinel, 0} << FormatString{str, 1}
              << FormatString{sentinel, 0};

    console.addLine(lineColor);
}

static void list(GLConsole &console, const char *str, const char *val,
                 UINT32 offset = 0) {
    LineColor<80> lineColor;
    lineColor << FormatString{str, 0} << FormatPad{24 + offset, 0}
              << FormatString{val, 2};

    console.addLine(lineColor);
}

static void listComConfig(GLConsole &console, COMCFG &cfg) {
    list(console, "type", cfg.type, 4);
    list(console, "ttyname", cfg.ttyname, 4);
    list(console, "alsaRawHWName", cfg.alsaRawHWName, 4);
}

static void printHex(GLConsole &console, size_t offset, size_t sz,
                     const uint8_t *data) {
    LineColor<80> lineColor;

    lineColor << FormatHex16{(uint16_t)offset, 1};
    lineColor << FormatString{":  ", 2};

    for (size_t i = offset; i < offset + sz; i++) {
        lineColor << FormatHex{data[i], 1};
        lineColor << FormatString{" ", 0};
    }

    console.addLine(lineColor);
}

static void cHexdumpMainmem(GLConsole &console, size_t offset, size_t size) {
    if (offset + size > 0x200000) {
        list(console, "Dump would go above address 0x200000", "");
        return;
    }

    for (size_t i = offset; i < offset + size; i += 16) {
        size_t prln = std::min((size_t)16, offset + size - i);

        printHex(console, i, prln, (uint8_t *)mem);
    }
}

static void listConfig(GLConsole &console, NP2OSCFG &oscfg) {
    list(console, "Display Clock", oscfg.DISPCLK);

    listSep(console, " --- ", "Joypads");

    list(console, "JoyPad 1", oscfg.JOYPAD1);
    list(console, "JoyPad 2", oscfg.JOYPAD2);

    list(console, "Joy Device 1", oscfg.JOYDEV[0]);
    list(console, "Joy Device 2", oscfg.JOYDEV[1]);
    list(console, "", "");

    listSep(console, " --- ", "mpu");
    listComConfig(console, oscfg.mpu);
    list(console, "", "");

    listSep(console, " --- ", "com 0");
    listComConfig(console, oscfg.com[0]);
    list(console, "", "");

    listSep(console, " --- ", "com 1");
    listComConfig(console, oscfg.com[1]);
    list(console, "", "");

    listSep(console, " --- ", "com 2");
    listComConfig(console, oscfg.com[2]);
    list(console, "", "");

    list(console, "Confirm", oscfg.confirm);
    list(console, "StatSave", oscfg.statsave);
    list(console, "hostdrv_write", oscfg.hostdrv_write);
    list(console, "jastsnd", oscfg.jastsnd);
    list(console, "", "");

    listSep(console, " --- ", "midi");

    list(console, "Midiwait", oscfg.MIDIWAIT);
    list(console, "Mididev 0", oscfg.MIDIDEV[0]);
    list(console, "Mididev 1", oscfg.MIDIDEV[1]);

    list(console, "", "");
}

static void processConsoleCommand(const std::string &line, GLConsole &console,
                                  NP2OSCFG &oscfg, const std::string &diskDir) {

    auto tokens = split(line.c_str());

    if (!tokens.count)
        return;

    if (tokens.views[0] == "showosconfig") {
        listConfig(console, oscfg);
    }

    if (tokens.views[0] == "dumpmem") {
        if (tokens.count == 3) {
            int offset = atoi(tokens.views[1].str);
            size_t sz = (size_t)atoi(tokens.views[2].str);

            cHexdumpMainmem(console, offset, sz);
        }
    }

    if (tokens.views[0] == "disk") {
        try {
            FileListing listing(diskDir.c_str());

            for (size_t i = 0; i < listing.filenames.size(); i++) {
                listIdx(console, i, listing.filenames[i].c_str());
            }
        } catch (...) {
            list(console, "Disk Directory does not exist", diskDir.c_str());
        }
    }

    if (tokens.views[0] == "inserti") {
        try {
            FileListing listing(diskDir.c_str());
            size_t fidx = (size_t)atoi(tokens.views[2].str);

            if (fidx < listing.filenames.size()) {
                int diskno = atoi(tokens.views[2].str);
                std::string diskPath(diskDir + "/" + listing.filenames[diskno]);

                list(console, "Load", listing.filenames[diskno].c_str(), 0);
                diskdrv_readyfdd(diskno, diskPath.c_str(), 0);
            }

        } catch (...) {
            list(console, "No", "");
        }
    }

    if (tokens.views[0] == "insert") {
        if (tokens.count == 3) {
            int diskno = atoi(tokens.views[2].str);
            std::string diskName(tokens.views[1].str, tokens.views[1].sz);
            std::string diskPath = diskDir + "/" + diskName;

            diskdrv_readyfdd(diskno, diskPath.c_str(), 0);
        }
    }
}

void loop(SignalFD &sfd, InputMapper &inputMapper, NP2CFG &cfg, NP2OSCFG &oscfg,
          Sfx::PulseSoundEngine &soundEngine, const std::string &diskDir) {

    (void)cfg;

    VisualScreen visualScreen = VisualScreen::MAIN;
    DoubleLines doubleLines = DoubleLines::NO;
    ViewPortMode mode = ViewPortMode::INTEGER;

    vkdebug = 1;

    GLFWContext glfwContext(640, 400);

    auto extensions = getRequiredGLFWExtensions();
    std::vector<const char *> layers;

    if (vkdebug) {
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        layers.push_back(validationLayerString);
    }

    VulkanInstance instance(extensions, layers);
    VulkanDebugCallback callback(instance, vkdebug);
    VulkanSurface surface(glfwContext, instance);

    VulkanPhysicalDevice physicalDevice =
        glPhysicalDeviceSelection(instance, surface);

    VulkanDevice device(vkdebug, physicalDevice, surface,
                        physicalDevice.queueFamilies);

    ShaderStore shaderStore(device);

    VulkanDescriptorLayouts layouts(device);

    VulkanDescriptorPoolExt descriptorPoolExt(device, 32);
    VulkanSampler sampler(physicalDevice, device, VK_FILTER_NEAREST);
    VkFormat renderDepthFormat = findDepthFormat(physicalDevice);

    // TODO: UniformBuffer needs to be newly calculated
    VulkanUniformBuffer uniformBuffer(device, physicalDevice, 2 * 1024 * 1024);
    UniformAllocator ua(uniformBuffer);

    GlyphCache glyphCache(physicalDevice, device);

    VulkanVtxBuffer vtx(device, physicalDevice, 1024 * 1024 * 20);
    DataAllocator alc(vtx);

    std::string fontfile = getFont();
    printf("using font %s\n", fontfile.c_str());
    Font font(fontfile.c_str(), 64);

    FontContext fontContext{glyphCache.textGlyphMappingCache,
                            glyphCache.imageIndexed, font.freetypeFace,
                            font.hbfont};

    GLConsole console(alc, fontContext, 80, device, physicalDevice, ua, sampler,
                      layouts, glyphCache.alphaTexture.textureView);

    std::vector<VulkanCmbBuffer *> cmbBuffers;
    cmbBuffers.push_back(&vtx);
    cmbBuffers.push_back(&uniformBuffer);

    std::vector<VulkanTextureGeneric *> textures;
    textures.push_back(&glyphCache.alphaTexture);

    auto dims = glfwContext.getCurrentSize();

    std::vector<uint32_t> codePoints;

    std::unique_ptr<VulkanSwapChain> swapChain(
        std::make_unique<VulkanSwapChain>(
            device, physicalDevice, device.graphicsFamily, device.presentFamily,
            surface, glfwContext.currentWidth, glfwContext.currentHeight,
            nullptr));

    std::unique_ptr<VulkanRenderPass> renderPass(
        std::make_unique<VulkanRenderPass>(
            device, swapChain->format, renderDepthFormat, ShouldPresent::YES));

    std::unique_ptr<VulkanSwapChainFramebuffers> swapChainFramebuffers(

        std::make_unique<VulkanSwapChainFramebuffers>(device, physicalDevice,
                                                      *swapChain, *renderPass,
                                                      renderDepthFormat));

    RenderOptions renderOptions = {
        VK_FALSE, VK_FALSE, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_POLYGON_MODE_FILL,
        getIntegerScissor(pc98Width, pc98Height, swapChain->extent.width,
                          swapChain->extent.height)};

    RenderOptions renderOptionsBlend = {
        VK_FALSE, VK_TRUE, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_POLYGON_MODE_FILL,
        getAspectScissor(4.0f / 3.0f, swapChain->extent.width,
                         swapChain->extent.height)};

    std::unique_ptr<PipelineTex> pipeline =
        std::make_unique<PipelineTex>(device, shaderStore, renderOptions,
                                      *renderPass, layouts.descriptorLayout);

    std::unique_ptr<PipelineTexExtIyColor> pipelineConsole =
        std::make_unique<PipelineTexExtIyColor>(device, shaderStore,
                                                renderOptionsBlend, *renderPass,
                                                layouts.descriptorLayoutExt);

    std::unique_ptr<VulkanScaler> scaler =
        std::make_unique<VulkanScaler>(device);

    std::unique_ptr<VulkanRenderBuffer> renderBuffer;

    console.ready();

    bool needsUpdate = true;
    bool needsSwapchainUpdate = false;

    std::vector<char> img(pc98Width * pc98Height * 4, 255);

    VulkanDescriptorPool descriptorPool(scaler->device, 1);

    VulkanTextureBGRA mainTexture(device, physicalDevice, pc98Width,
                                  pc98Height);

    textures.push_back(&mainTexture);

    VulkanDescriptorSet descriptorSetMain(
        scaler->device, mainTexture.textureView, sampler, descriptorPool,
        layouts.descriptorLayout);

    CallbackContext ctx{
        {pc98Width, pc98Height, img.data()}, &glfwContext.input, false};

    while (glfwContext.getWindowState() != WindowState::SHOULDCLOSE &&
           !sfd.isTriggered()) {

        mainloop(&ctx, &soundEngine);

        if (!needsSwapchainUpdate && !needsUpdate) {
            // glfwContext.wait(1);
        }

        auto nDims = glfwContext.getCurrentSize();

        glfwContext.pollWindowEvents();

        if (nDims != dims) {
            needsSwapchainUpdate = true;
            dims = nDims;
        }

        GLFWInput &input = glfwContext.input;

        if (input.getButton(KeyButtons::KEY_SUPER)) {
            inputMapper.handleInput(input, mode, visualScreen, doubleLines,
                                    soundEngine);
        } else {

            if (visualScreen == VisualScreen::MAIN) {
                inputMapper.handleInputKeys(input);
            } else {

                if (!input.codepoints.empty()) {
                    console.add(input.codepoints);
                    needsUpdate = true;
                }

                for (auto &keyEvent : input.keyEvents) {
                    if (keyEvent.key == KeyButtons::KEY_ENTER &&
                        keyEvent.state == PRESSED) {
                        auto utf8 = getUTF8FromUnicode(console.codePoints);

                        if (utf8) {
                            processConsoleCommand(*utf8, console, oscfg,
                                                  diskDir);
                        }

                        console.clear();
                        console.ready();

                        needsUpdate = true;
                    }

                    if (keyEvent.key == KeyButtons::KEY_BACKSPACE &&
                        keyEvent.state == PRESSED) {
                        console.del();
                        needsUpdate = true;
                    }

                    if (keyEvent.key == KeyButtons::KEY_ARROW_UP &&
                        keyEvent.state == PRESSED) {
                        console.up();
                        console.ready();
                        needsUpdate = true;
                    }

                    if (keyEvent.key == KeyButtons::KEY_ARROW_DOWN &&
                        keyEvent.state == PRESSED) {
                        console.down();
                        console.ready();
                        needsUpdate = true;
                    }

                    if (keyEvent.key == KeyButtons::KEY_PAGE_UP &&
                        keyEvent.state == PRESSED) {
                        console.pageUp();
                        console.ready();
                        needsUpdate = true;
                    }

                    if (keyEvent.key == KeyButtons::KEY_PAGE_DOWN &&
                        keyEvent.state == PRESSED) {
                        console.pageDown();
                        console.ready();
                        needsUpdate = true;
                    }
                }
            }
        }

        input.reset();

        if (glyphCache.imageIndexed.dirty) {
            glyphCache.alphaTexture.dirty();
            glyphCache.imageIndexed.dirty = false;
        }

        if (scaler->renderingComplete()) {
            if (ctx.dirty) {

                ctx.dirty = false;

                if (doubleLines == DoubleLines::SIMPLE) {
                    doubleBlankLines(pc98Width, pc98Height,
                                     (const unsigned char *)img.data(),
                                     (unsigned char *)mainTexture.data);
                } else if (doubleLines == DoubleLines::ADAPTIVE) {
                    doubleBlankLinesAdaptive(pc98Width, pc98Height,
                                             (const unsigned char *)img.data(),
                                             (unsigned char *)mainTexture.data);
                } else {
                    memcpy(mainTexture.data, img.data(), img.size());
                }

                mainTexture.dirty();
                needsUpdate = true;
            }

            if (needsSwapchainUpdate) {
                if (glfwContext.currentWidth == 0 ||
                    glfwContext.currentHeight == 0) {
                    continue;
                }

                swapChain = std::make_unique<VulkanSwapChain>(
                    device, physicalDevice, device.graphicsFamily,
                    device.presentFamily, surface, glfwContext.currentWidth,
                    glfwContext.currentHeight, swapChain.get());

                renderPass = std::make_unique<VulkanRenderPass>(
                    device, swapChain->format, renderDepthFormat,
                    ShouldPresent::YES);

                swapChainFramebuffers =
                    std::make_unique<VulkanSwapChainFramebuffers>(
                        device, physicalDevice, *swapChain, *renderPass,
                        renderDepthFormat);

                renderOptions.scissor = getIntegerScissor(
                    pc98Width, pc98Height, swapChain->extent.width,
                    swapChain->extent.height);

                renderOptionsBlend.scissor =
                    getAspectScissor(4.0f / 3.0f, swapChain->extent.width,
                                     swapChain->extent.height);

                pipeline = std::make_unique<PipelineTex>(
                    device, shaderStore, renderOptions, *renderPass,
                    layouts.descriptorLayout);

                pipelineConsole = std::make_unique<PipelineTexExtIyColor>(
                    device, shaderStore, renderOptionsBlend, *renderPass,
                    layouts.descriptorLayoutExt);

                scaler = std::make_unique<VulkanScaler>(device);

                needsSwapchainUpdate = false;
                needsUpdate = true;
            }

            if (needsUpdate) {
                renderBuffer = std::make_unique<VulkanRenderBuffer>(
                    device, device.graphicsFamily);

                renderBuffer->begin(*renderPass,
                                    swapChainFramebuffers->framebuffers,
                                    swapChain->extent);

                if (visualScreen == VisualScreen::MAIN) {
                    pipeline->record(renderBuffer->commandBuffers.data(),
                                     renderBuffer->commandBuffers.size(),
                                     descriptorSetMain, 6);
                } else if (visualScreen == VisualScreen::CONSOLEMAIN) {
                    pipeline->record(renderBuffer->commandBuffers.data(),
                                     renderBuffer->commandBuffers.size(),
                                     descriptorSetMain, 6);

                    console.draw(*pipelineConsole,
                                 renderBuffer->commandBuffers.data(),
                                 renderBuffer->commandBuffers.size());
                } else if (visualScreen == VisualScreen::CONSOLE) {
                    console.draw(*pipelineConsole,
                                 renderBuffer->commandBuffers.data(),
                                 renderBuffer->commandBuffers.size());
                }

                renderBuffer->end();

                auto drawRet = scaler->draw(*renderBuffer, cmbBuffers, textures,
                                            *swapChain);

                if (drawRet.state == RenderState::NEEDSSWAPCHAINUPDATE) {
                    needsSwapchainUpdate = true;
                } else if (drawRet.state == RenderState::OK) {
                    scaler->present(drawRet.index, *swapChain);
                    needsUpdate = false;
                }
            }
        }
    }

    while (!scaler->renderingComplete()) {
        usleep(1000);
    }

    vkDeviceWaitIdle(device);
}

} // namespace BR
