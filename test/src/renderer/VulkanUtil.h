#pragma once

#include "VulkanContext.h"

namespace renderer {

class VulkanUtil {
public:
  static void createBuffer(VulkanContext &context, VkDeviceSize size,
                           VkBufferUsageFlags usage,
                           VkMemoryPropertyFlags properties, VkBuffer &buffer,
                           VkDeviceMemory &bufferMemory);
  static void copyBuffer(VulkanContext &context, VkCommandPool commandPool,
                         VkBuffer srcBuffer, VkBuffer dstBuffer,
                         VkDeviceSize size);
  static void createImage(VulkanContext &context, uint32_t width,
                          uint32_t height, VkFormat format,
                          VkImageTiling tiling, VkImageUsageFlags usage,
                          VkMemoryPropertyFlags properties, VkImage &image,
                          VkDeviceMemory &imageMemory);
  static VkImageView createImageView(VulkanContext &context, VkImage image,
                                     VkFormat format,
                                     VkImageAspectFlags aspectFlags);
  static void transitionImageLayout(VulkanContext &context,
                                    VkCommandPool commandPool, VkImage image,
                                    VkFormat format, VkImageLayout oldLayout,
                                    VkImageLayout newLayout);
  static VkCommandBuffer beginSingleTimeCommands(VulkanContext &context,
                                                 VkCommandPool commandPool);
  static void endSingleTimeCommands(VulkanContext &context,
                                    VkCommandPool commandPool,
                                    VkCommandBuffer commandBuffer);
};

} // namespace renderer
