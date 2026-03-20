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

    lineColor << FormatString{str, ConsoleColor::WHITE}
              << FormatPad{36 + offset} << FormatSize{val, ConsoleColor::RED};

    console.addLine(lineColor);
}

static void listIdx(GLConsole &console, size_t idx, const char *str) {
    LineColor<132> lineColor;

    lineColor << FormatSize{idx} << FormatString{": ", ConsoleColor::RED}
              << FormatPad{8} << FormatString{str};

    console.addLine(lineColor);
}

static void listSep(GLConsole &console, const char *sentinel, const char *str) {
    LineColor<132> lineColor;

    lineColor << FormatString{sentinel} << FormatString{str, ConsoleColor::BLUE}
              << FormatString{sentinel};

    console.addLine(lineColor);
}

static void list(GLConsole &console, const char *str, const char *val,
                 UINT32 offset = 0) {
    LineColor<132> lineColor;
    lineColor << FormatString{str} << FormatPad{36 + offset}
              << FormatString{val, ConsoleColor::RED};

    console.addLine(lineColor);
}

static void listComConfig(GLConsole &console, const COMCFG &cfg) {
    list(console, "type", cfg.type, 4);
    list(console, "ttyname", cfg.ttyname, 4);
    list(console, "alsaRawHWName", cfg.alsaRawHWName, 4);
}

static void printHex(GLConsole &console, size_t offset, size_t sz,
                     const uint8_t *dataOffset0) {
    LineColor<132> lineColor;

    lineColor << FormatHex32{(uint32_t)offset, ConsoleColor::BLUE};
    lineColor << FormatString{":  ", ConsoleColor::RED};

    for (size_t i = offset; i < offset + sz; i++) {
        lineColor << FormatHex{dataOffset0[i]};
        lineColor << FormatString{" "};
    }

    lineColor << FormatString{"  "};

    for (size_t i = offset; i < offset + sz; i++) {
        uint8_t ch = dataOffset0[i];

        if (isprint(ch)) {
            lineColor << FormatPrintChar{ch};
        } else {
            lineColor << FormatPrintChar{'.'};
        }
    }

    console.addLine(lineColor);
}

static void printSpecialLine(GLConsole &console, size_t offset,
                             ConsoleColor (&colors)[16],
                             const uint8_t *dataAdjusted) {
    LineColor<132> lineColor;

    lineColor << FormatHex32{(uint32_t)offset, ConsoleColor::BLUE};
    lineColor << FormatString{":  ", ConsoleColor::RED};

    for (size_t i = 0; i < 16; i++) {
        lineColor << FormatHex{dataAdjusted[i], colors[i]};
        lineColor << FormatString{" "};
    }

    lineColor << FormatString{"  "};

    for (size_t i = 0; i < 16; i++) {
        uint8_t ch = dataAdjusted[i];

        if (isprint(ch)) {
            lineColor << FormatPrintChar{ch, colors[i]};
        } else {
            lineColor << FormatPrintChar{'.', colors[i]};
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
    list(console, "ref",
         "Save a reference snapshot of the current system memory");
    list(console, "cmp", "Compare saved reference with current system memory");
    list(console, "pokeb <offset> <byte>",
         "Put <byte> into <offset> system memory");
    list(console, "pokew <offset> <word>",
         "Put <word> into <offset> system memory");
    list(console, "pokedw <offset> <dword>",
         "Put <dword> into <offset> system memory");
    list(console, "pokecs <offset> <string>",
         "Put <string> with a 0 byte after each character into <offset> system "
         "memory");
    list(console, "pokes <offset> <string>",
         "Put <string> into <offset> system memory");

    listSep(console, "", "");
}

static void processConsoleCommand(const std::string &line, GLConsole &console,
                                  const NP2OSCFG &oscfg,
                                  const std::string &diskDir,
                                  std::vector<uint8_t> &ref,
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

    if (tokens.views[0] == "ref") {
        ref.clear();
        ref.insert(ref.begin(), mem, mem + 0x200000);
        list(console, "OK", "");
    }

    if (tokens.views[0] == "cmp") {
        if (ref.size() != 0x200000) {
            list(console, "No reference", "");
            return;
        }

        std::vector<uint8_t> cur;
        cur.insert(cur.begin(), mem, mem + 0x200000);

        // printf("%zu %zu\n", cur.size(), ref.size());

        listSep(console, "--", "Begin compare");

        for (size_t i = 0; i < 0x200000; i += 16) {
            ConsoleColor colors[16]{};
            bool hit = false;

            for (size_t j = 0; j < 16; j++) {
                if (cur[i + j] != ref[i + j]) {
                    colors[j] = ConsoleColor::RED;
                    hit = true;
                }
            }

            if (hit) {
                printSpecialLine(console, i, colors, ref.data() + i);
                printSpecialLine(console, i, colors, cur.data() + i);
                list(console, "", "");
            }
        }

        listSep(console, "--", "End compare");
    }

    if (tokens.views[0] == "pokeb") {
        if (tokens.count == 3) {
            size_t offset = (size_t)atol16(tokens.views[1].str);
            uint8_t byte = (uint8_t)atol16(tokens.views[2].str);

            if (offset >= 0x200000) {
                return;
            }

            mem[offset] = byte;

            LineColor<132> lineColor;
            lineColor << FormatString{"Set offset "}
                      << FormatHex32{(uint32_t)offset, ConsoleColor::BLUE}
                      << FormatString{" to "}
                      << FormatHex{byte, ConsoleColor::BLUE};

            console.addLine(lineColor);
        }
    }

    if (tokens.views[0] == "pokew") {
        if (tokens.count == 3) {
            size_t offset = (size_t)atol16(tokens.views[1].str);
            uint16_t word = (uint16_t)atol16(tokens.views[2].str);

            if (offset + 1 >= 0x200000) {
                return;
            }

            mem[offset] = word & 0xff;
            mem[offset + 1] = (word >> 8) & 0xff;

            LineColor<132> lineColor;
            lineColor << FormatString{"Set offset "}
                      << FormatHex32{(uint32_t)offset, ConsoleColor::BLUE}
                      << FormatString{" to "}
                      << FormatHex16{word, ConsoleColor::BLUE};

            console.addLine(lineColor);
        }
    }

    if (tokens.views[0] == "pokedw") {
        if (tokens.count == 3) {
            size_t offset = (size_t)atol16(tokens.views[1].str);
            uint32_t dword = (uint32_t)atol16(tokens.views[2].str);

            if (offset + 3 >= 0x200000) {
                return;
            }

            mem[offset] = dword & 0xff;
            mem[offset + 1] = (dword >> 8) & 0xff;
            mem[offset + 2] = (dword >> 16) & 0xff;
            mem[offset + 3] = (dword >> 24) & 0xff;

            LineColor<132> lineColor;
            lineColor << FormatString{"Set offset "}
                      << FormatHex32{(uint32_t)offset, ConsoleColor::BLUE}
                      << FormatString{" to "}
                      << FormatHex32{dword, ConsoleColor::BLUE};

            console.addLine(lineColor);
        }
    }

    if (tokens.views[0] == "pokes") {
        if (tokens.count == 3) {
            size_t offset = (size_t)atol16(tokens.views[1].str);

            StringView &strView = tokens.views[2];

            if (offset + strView.sz >= 0x200000) {
                return;
            }

            for (size_t i = 0; i < strView.sz; i++) {
                mem[offset + i] = strView.str[i];
            }

            list(console, "OK", "");
        }
    }

    if (tokens.views[0] == "pokecs") {
        if (tokens.count == 3) {
            size_t offset = (size_t)atol16(tokens.views[1].str);

            StringView &strView = tokens.views[2];

            size_t dosConsoleStrSize = strView.sz * 2;

            if (offset + dosConsoleStrSize >= 0x200000) {
                return;
            }

            uint8_t dosConsoleStr[dosConsoleStrSize];

            for (size_t i = 0; i < strView.sz; i++) {
                dosConsoleStr[i * 2] = strView.str[i];
                dosConsoleStr[i * 2 + 1] = 0;
            }

            for (size_t i = 0; i < dosConsoleStrSize; i++) {
                mem[offset + i] = dosConsoleStr[i];
            }

            list(console, "OK", "");
        }
    }

    if (tokens.views[0] == "drop") {
        ref.clear();
        list(console, "OK", "");
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
      diskDir(diskDir), oscfg(oscfg), globalMutex(globalMutex) {
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
                processConsoleCommand(*utf8, console, oscfg, diskDir, ref,
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
