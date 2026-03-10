#include "VKShaders.h"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <ios>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace BR {

static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::string("failed to open file!") + filename;
    }

    const std::streamsize fileSize = file.tellg();

    if (fileSize == -1) {
        throw std::string("failed tell filesize!") + filename;
    }

    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

static std::vector<char> readShaderFile(const char *filename) {
    try {
        const std::string shaderFilename(std::string("x11/shader/") + filename);
        printf("Try relative shaderfilename: %s\n", shaderFilename.c_str());
        return readFile(shaderFilename);
    } catch (...) {
        const std::string shaderFilename(std::string("/usr/share/xnp2katze/") +
                                   filename);
        printf("Try global shaderfilename: %s\n", shaderFilename.c_str());
        return readFile(shaderFilename);
    }
}

ShaderStage::ShaderStage(VkDevice device, const std::vector<char> &code)
    : device(device) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = (uint32_t *)code.data();

    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
        VK_SUCCESS) {
        throw "failed to create shader module!";
    }
}

ShaderStage::~ShaderStage() {
    vkDestroyShaderModule(device, shaderModule, nullptr);
}

ShaderStage::operator VkShaderModule &() { return shaderModule; }

ShaderStore::ShaderStore(VkDevice device)
    : vertTri(device, readShaderFile("vert/tri.spv")),
      fragTri(device, readShaderFile("frag/tri.spv")),
      vertTexExt(device, readShaderFile("vert/texExt.spv")),
      vertTexExtColor(device, readShaderFile("vert/texExtColor.spv")),
      vertTex(device, readShaderFile("vert/tex.spv")),
      fragTex(device, readShaderFile("frag/tex.spv")),
      fragTexIy(device, readShaderFile("frag/texIy.spv")),
      fragTexIyColor(device, readShaderFile("frag/texIyColor.spv"))

{}

ShaderStore::~ShaderStore() {}

} // namespace BR
