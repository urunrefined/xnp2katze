#pragma once

#include <vulkan/vulkan.h>

namespace BR {

class VulkanSemaphore {
  public:
    VkDevice device;
    VkSemaphore semaphore;

    VulkanSemaphore(VkDevice device);

    ~VulkanSemaphore();

    operator VkSemaphore &() { return semaphore; }

    operator VkSemaphore *() { return &semaphore; }
};

class VulkanFence {
  public:
    VkDevice device;
    VkFence fence;

    VulkanFence(VkDevice device);
    ~VulkanFence();

    operator VkFence &() { return fence; }

    VkResult wait();
};

enum class SitterState { INIT, INACTIVE = 0, ACTIVE };

class Sitter {
  public:
    VkDevice device;
    VkFence fence;
    SitterState state = SitterState::INACTIVE;

    Sitter(VkDevice device);
    ~Sitter();

    void activate();
    void cancel();

    bool done();

    void block(uint64_t nanoseconds);
    void block();
    void wait();

    operator VkFence &() { return fence; }
};

} // namespace BR
