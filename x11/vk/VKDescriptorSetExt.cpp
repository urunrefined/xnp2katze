#include "VKDescriptorSetExt.h"
#include "util/Core.h"
#include "util/Matrix4x4.h"

#include <string.h>
#include <vector>

namespace BR {

VulkanDescriptorLayoutExt::VulkanDescriptorLayoutExt(VkDevice device)
    : device(device) {
    VkDescriptorSetLayoutBinding samplerLayoutBinding[3]{};

    samplerLayoutBinding[0].binding = 0;
    samplerLayoutBinding[0].descriptorCount = 1;
    samplerLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    samplerLayoutBinding[0].pImmutableSamplers = nullptr;
    samplerLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    samplerLayoutBinding[1].binding = 1;
    samplerLayoutBinding[1].descriptorCount = 1;
    samplerLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    samplerLayoutBinding[1].pImmutableSamplers = nullptr;
    samplerLayoutBinding[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    samplerLayoutBinding[2].binding = 2;
    samplerLayoutBinding[2].descriptorCount = 1;
    samplerLayoutBinding[2].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding[2].pImmutableSamplers = nullptr;
    samplerLayoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = arraySize(samplerLayoutBinding);
    layoutInfo.pBindings = samplerLayoutBinding;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) !=
        VK_SUCCESS) {
        throw "failed to create descriptor set layout!";
    }
}

VulkanDescriptorLayoutExt::~VulkanDescriptorLayoutExt() {
    vkDestroyDescriptorSetLayout(device, layout, 0);
}

template <class T1>
static VkDeviceSize getUniformSize(const VulkanPhysicalDevice &physicalDevice) {
    VkDeviceSize alignment =
        physicalDevice.getMinUniformBufferOffsetAlignment();

    if (sizeof(T1) % alignment) {
        return sizeof(T1) + (alignment - (sizeof(T1) % alignment));
    }

    return sizeof(T1);
}

size_t
VulkanDescriptorSetExt::getChunkSize(VulkanPhysicalDevice &physicalDevice,
                                     uint32_t modelCount) {
    return getUniformSize<Matrix4x4f>(physicalDevice) +
           getUniformSize<Matrix4x4f>(physicalDevice) * modelCount;
}

VulkanDescriptorSetExt::VulkanDescriptorSetExt(
    VkDevice device, VulkanPhysicalDevice &physicalDevice,
    const UniformChunk &chunk, VkImageView imageView, VkSampler sampler,
    VkDescriptorPool descriptorPoolExt, VkDescriptorSetLayout layout,
    size_t modelCount)
    : device(device), physicalDevice(physicalDevice),
      descriptorPoolExt(descriptorPoolExt), chunk(chunk),
      modelCount(modelCount) {
    std::vector<VkDescriptorSetLayout> layouts(modelCount, layout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPoolExt;
    allocInfo.descriptorSetCount = modelCount;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(modelCount);

    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) !=
        VK_SUCCESS) {
        throw "failed to allocate descriptor sets!";
    }

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;

    VkDescriptorBufferInfo worldBufferInfo{};

    worldBufferInfo.buffer = chunk.buffer;
    worldBufferInfo.offset =
        getUniformSize<Matrix4x4f>(physicalDevice) * 0 + chunk.bufferOffset;
    worldBufferInfo.range = sizeof(Matrix4x4f);

    VkWriteDescriptorSet descriptorWrite[3]{};

    for (size_t i = 0; i < modelCount; i++) {
        VkDescriptorSet descriptorSet = descriptorSets[i];

        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].dstSet = descriptorSet;
        descriptorWrite[0].dstBinding = 0;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pImageInfo = nullptr;
        descriptorWrite[0].pBufferInfo = &worldBufferInfo;

        VkDescriptorBufferInfo modelBufferInfo{};

        modelBufferInfo.buffer = chunk.buffer;
        modelBufferInfo.offset =
            getUniformSize<Matrix4x4f>(physicalDevice) * (i + 1) +
            chunk.bufferOffset;
        modelBufferInfo.range = sizeof(Matrix4x4f);

        descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[1].dstSet = descriptorSet;
        descriptorWrite[1].dstBinding = 1;
        descriptorWrite[1].dstArrayElement = 0;
        descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[1].descriptorCount = 1;
        descriptorWrite[1].pImageInfo = nullptr;
        descriptorWrite[1].pBufferInfo = &modelBufferInfo;

        descriptorWrite[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[2].dstSet = descriptorSet;
        descriptorWrite[2].dstBinding = 2;
        descriptorWrite[2].dstArrayElement = 0;
        descriptorWrite[2].descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[2].descriptorCount = 1;
        descriptorWrite[2].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device, arraySize(descriptorWrite),
                               descriptorWrite, 0, nullptr);
    }
}

void VulkanDescriptorSetExt::updateWorldMatrix(const Matrix4x4f &world) {
    chunk.update((const char *)world.getData(), 0, sizeof(float) * 16);
}

void VulkanDescriptorSetExt::updateModelMatrix(const Matrix4x4f &model,
                                               size_t index) {
    chunk.update((const char *)model.getData(),
                 getUniformSize<Matrix4x4f>(physicalDevice) * (index + 1),
                 sizeof(float) * 16);
}

VulkanDescriptorSetExt::~VulkanDescriptorSetExt() {
    vkFreeDescriptorSets(device, descriptorPoolExt, modelCount,
                         descriptorSets.data());
}

} // namespace BR
