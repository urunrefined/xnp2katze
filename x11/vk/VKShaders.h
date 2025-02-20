#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace BR {

class ShaderStage {
  public:
    VkDevice device;

    VkShaderModule shaderModule;

    ShaderStage(VkDevice device, const std::vector<char> &code);
    ~ShaderStage();

    operator VkShaderModule &();

    ShaderStage(const ShaderStage &that) = delete;
};

class ShaderStore {
  public:
    ShaderStage vertTri;
    ShaderStage fragTri;
    ShaderStage vertTexExt;
    ShaderStage vertTexExtColor;

    ShaderStage vertTex;
    ShaderStage fragTex;
    ShaderStage fragTexIy;
    ShaderStage fragTexIyColor;

    ShaderStore(VkDevice device);

    ~ShaderStore();
};

} // namespace BR
