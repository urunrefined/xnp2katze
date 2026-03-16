#include "ConsoleContext.h"
#include "np2.h"
#include "util/Deep.h"

#include "fdd/diskdrv.h"
#include "util/Codepage.h"
#include "util/FileListing.h"
#include "util/Number.h"
#include "util/StringView.h"

#include "gl/GLConsole.h"
#include "gl/GLGlyphCache.h"

#include "cpumem.h"

namespace BR {

static void list(GLConsole &console, const char *str, UINT32 val,
                 UINT32 offset = 0) {
    LineColor<132> lineColor;

    lineColor << FormatString{str, 0} << FormatPad{36 + offset, 0}
              << FormatSize{val, 2};

    console.addLine(lineColor);
}

static void listIdx(GLConsole &console, size_t idx, const char *str) {
    LineColor<132> lineColor;

    lineColor << FormatSize{idx, 0} << FormatString{": ", 2} << FormatPad{8, 0}
              << FormatString{str, 0};

    console.addLine(lineColor);
}

static void listSep(GLConsole &console, const char *sentinel, const char *str) {
    LineColor<132> lineColor;

    lineColor << FormatString{sentinel, 0} << FormatString{str, 1}
              << FormatString{sentinel, 0};

    console.addLine(lineColor);
}

static void list(GLConsole &console, const char *str, const char *val,
                 UINT32 offset = 0) {
    LineColor<132> lineColor;
    lineColor << FormatString{str, 0} << FormatPad{36 + offset, 0}
              << FormatString{val, 2};

    console.addLine(lineColor);
}

static void listComConfig(GLConsole &console, const COMCFG &cfg) {
    list(console, "type", cfg.type, 4);
    list(console, "ttyname", cfg.ttyname, 4);
    list(console, "alsaRawHWName", cfg.alsaRawHWName, 4);
}

static void printHex(GLConsole &console, size_t offset, size_t sz,
                     const uint8_t *data) {
    LineColor<132> lineColor;

    lineColor << FormatHex16{(uint16_t)offset, 1};
    lineColor << FormatString{":  ", 2};

    for (size_t i = offset; i < offset + sz; i++) {
        lineColor << FormatHex{data[i], 1};
        lineColor << FormatString{" ", 0};
    }
    
    lineColor << FormatString{"  ", 0};
    
    for (size_t i = offset; i < offset + sz; i++) {
        uint8_t ch = data[i];
    
        if (isprint(ch)) {
            lineColor << FormatPrintChar{ch, 0};
        }
        else {
            lineColor << FormatPrintChar{'.', 0};
        }
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

    listSep(console, "", "");
}

static void listConfig(GLConsole &console, const NP2OSCFG &oscfg) {
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

static void printHelp(GLConsole &console) {
    listSep(console, " --- ", "Help");

    list(console, "help", "Shows this help");
    list(console, "showosconfig", "Shows the current OS config");
    list(console, "dumpmem <offset> <size>",
         "Prints memory from offset <offset> for <size> bytes");
    list(console, "disk", "Shows currently available disks");
    list(console, "inserti <diskindex> <drivenumber>",
         "Insert disk with number <diskindex> into drive <drivenumber>");
    list(console, "insert <diskname> <drivenumber>",
         "Insert disk with name <diskname> into drive <drivenumber>");

    listSep(console, "", "");
}

static void processConsoleCommand(const std::string &line, GLConsole &console,
                                  const NP2OSCFG &oscfg,
                                  const std::string &diskDir,
                                  Mutex &globalMutex) {

    auto tokens = split(line.c_str());

    if (!tokens.count)
        return;

    if (tokens.views[0] == "help" || tokens.views[0] == "?") {
        printHelp(console);
    }

    if (tokens.views[0] == "showosconfig") {
        listConfig(console, oscfg);
    }

    if (tokens.views[0] == "dumpmem") {
        if (tokens.count == 3) {
            int offset = atol16(tokens.views[1].str);
            size_t sz = (size_t)atol16(tokens.views[2].str);

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

        listSep(console, "", "");
    }

    if (tokens.views[0] == "inserti") {
        try {
            FileListing listing(diskDir.c_str());

            if (tokens.count == 3) {
                size_t fidx = (size_t)atoi(tokens.views[1].str);

                if (fidx < listing.filenames.size()) {
                    std::string diskPath(diskDir + "/" +
                                         listing.filenames[fidx]);

                    list(console, "Load", listing.filenames[fidx].c_str(), 0);
                    int driveNumber = atoi(tokens.views[2].str);

                    LockGuard lg(globalMutex);
                    diskdrv_readyfdd(driveNumber, diskPath.c_str(), 0);
                }
            }

        } catch (...) {
            list(console, "Could not create directory listing", "");
        }

        listSep(console, "", "");
    }

    if (tokens.views[0] == "insert") {
        if (tokens.count == 3) {
            std::string diskName(tokens.views[1].str, tokens.views[1].sz);
            std::string diskPath = diskDir + "/" + diskName;

            list(console, "Load", diskName.c_str(), 0);

            int driveNumber = atoi(tokens.views[2].str);

            LockGuard lg(globalMutex);

            diskdrv_readyfdd(driveNumber, diskPath.c_str(), 0);
        }

        listSep(console, "", "");
    }
}

ConsoleContext::ConsoleContext(
    VulkanDevice &device, VulkanPhysicalDevice &physicalDevice,
    std::unique_ptr<VulkanWindowContext> &&windowContext_,
    VulkanSampler &sampler, VulkanDescriptorLayouts &layouts,
    const std::string &diskDir, const NP2OSCFG &oscfg, Mutex &globalMutex)
    : MainContext(device, physicalDevice, std::move(windowContext_)),
      uniformBuffer(device, physicalDevice, 2 * 1024 * 1024), ua(uniformBuffer),
      glyphCache(physicalDevice, device),
      vtx(device, physicalDevice, 1024 * 1024 * 20), alc(vtx),
      font(getFont().c_str(), 64),
      fontContext{glyphCache.textGlyphMappingCache, glyphCache.imageIndexed,
                  font.freetypeFace, font.hbfont},
      console(alc, fontContext, 132, device, physicalDevice, ua, sampler,
              layouts, glyphCache.alphaTexture.textureView),
      diskDir(diskDir), oscfg(oscfg), globalMutex(globalMutex)

{
    cmbBuffers.push_back(&vtx);
    cmbBuffers.push_back(&uniformBuffer);

    textures.push_back(&glyphCache.alphaTexture);
}

void ConsoleContext::createRender(VulkanRenderPass &renderPass,
                                  VkFormat renderDepthFormat,
                                  ShaderStore &shaderStore,
                                  VulkanDescriptorLayouts &layouts) {

    (void)shaderStore;
    (void)layouts;

    MainContext::createRender(renderPass, renderDepthFormat);

    RenderOptions renderOptions{
        VK_FALSE, VK_TRUE, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        VK_POLYGON_MODE_FILL,
        getAspectScissor(4.0f / 3.0f, renderContext->swapChain.extent.width,
                         renderContext->swapChain.extent.height)};

    pipeline = std::make_unique<PipelineTexExtIyColor>(
        device, shaderStore, renderOptions, renderPass,
        layouts.descriptorLayoutExt);
}

void ConsoleContext::work() {
    Input &input = getInput();

    if (!input.codepoints.empty()) {
        console.add(input.codepoints);
        needsUpdate = true;
    }

    for (auto &keyEvent : input.keyEvents) {
        if (keyEvent.key == KeyButtons::KEY_ENTER &&
            keyEvent.state == PRESSED) {
            auto utf8 = getUTF8FromUnicode(console.codePoints);

            if (utf8) {
                processConsoleCommand(*utf8, console, oscfg, diskDir,
                                      globalMutex);
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

void ConsoleContext::frame() {
    if (glyphCache.imageIndexed.dirty) {
        glyphCache.alphaTexture.dirty();
        glyphCache.imageIndexed.dirty = false;
    }
}

void ConsoleContext::record() {
    console.draw(*pipeline, renderBuffer->commandBuffers.data(),
                 renderBuffer->commandBuffers.size());
}

ConsoleContext::~ConsoleContext() {}

} // namespace BR
