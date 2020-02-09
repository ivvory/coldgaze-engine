#include "vulkan\vulkan_core.h"
#include <vector>
#include <string>

namespace CG
{
	namespace Vk
	{
		class Device
		{
		public:
			/** @brief Physical device representation */
			VkPhysicalDevice physicalDevice = {};
			/** @brief Logical device representation (application's view of the device) */
			VkDevice logicalDevice = {};
			/** @brief Properties of the physical device including limits that the application can check against */
			VkPhysicalDeviceProperties properties = {};
			/** @brief Features of the physical device that an application can use to check if a feature is supported */
			VkPhysicalDeviceFeatures features = {};
			/** @brief Features that have been enabled for use on the physical device */
			VkPhysicalDeviceFeatures enabledFeatures = {};
			/** @brief Memory types and heaps of the physical device */
			VkPhysicalDeviceMemoryProperties memoryProperties = {};
			/** @brief Queue family properties of the physical device */
			std::vector<VkQueueFamilyProperties> queueFamilyProperties = {};
			/** @brief List of extensions supported by the device */
			std::vector<std::string> supportedExtensions = {};
			/** @brief Depth buffer format (selected during Vulkan initialization) */
			VkFormat depthFormat;

			/** @brief Default command pool for the graphics queue family index */
			VkCommandPool commandPool = VK_NULL_HANDLE;

			/** @brief Set to true when the debug marker extension is detected */
			bool enableDebugMarkers = false;

			/** @brief Contains queue family indices */
			struct
			{
				uint32_t graphics;
				uint32_t compute;
				uint32_t transfer;
			} queueFamilyIndices;

			/**  @brief Typecast to VkDevice */
			operator VkDevice() { return logicalDevice; };

			Device(VkPhysicalDevice physicalDevice);
			~Device();

			/**
			* Get the index of a memory type that has all the requested property bits set
			*
			* @param typeBits Bitmask with bits set for each memory type supported by the resource to request for (from VkMemoryRequirements)
			* @param properties Bitmask of properties for the memory type to request
			* @param (Optional) memTypeFound Pointer to a bool that is set to true if a matching memory type has been found
			*
			* @return Index of the requested memory type
			*
			* @throw Throws an exception if memTypeFound is null and no memory type could be found that supports the requested properties
			*/
			uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr);

			/**
			* Get the index of a queue family that supports the requested queue flags
			*
			* @param queueFlags Queue flags to find a queue family index for
			*
			* @return Index of the queue family index that matches the flags
			*
			* @throw Throws an exception if no queue family index could be found that supports the requested flags
			*/
			uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags);

			// TODO: incapsulate in device
			/**
            * Selected a suitable supported depth format starting with 32 bit down to 16 bit
            *
            * @param physicalDevicePhysical device to be used for format search
            *
			* @param depthFormat format that will be filled after successful function execution
			*
            * @return False if none of the depth formats in the list is supported by the device
            */
			VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat);

			/**
			* Create the logical device based on the assigned physical device, also gets default queue family indices
			*
			* @param enabledFeatures Can be used to enable certain features upon device creation
			* @param pNextChain Optional chain of pointer to extension structures
			* @param useSwapChain Set to false for headless rendering to omit the swapchain device extensions
			* @param requestedQueueTypes Bit flags specifying the queue types to be requested from the device
			*
			* @return VkResult of the device creation call
			*/
			VkResult CreateLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures, std::vector<const char*> enabledExtensions, void* pNextChain, bool useSwapChain = true, VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

			/**
			* Create a buffer on the device
			*
			* @param usageFlags Usage flag bitmask for the buffer (i.e. index, vertex, uniform buffer)
			* @param memoryPropertyFlags Memory properties for this buffer (i.e. device local, host visible, coherent)
			* @param buffer Pointer to a vk::Vulkan buffer object
			* @param size Size of the buffer in byes
			* @param data Pointer to the data that should be copied to the buffer after creation (optional, if not set, no data is copied over)
			*
			* @return VK_SUCCESS if buffer handle and memory have been created and (optionally passed) data has been copied
			*/
			// VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, vks::Buffer* buffer, VkDeviceSize size, void* data = nullptr);

			/**
			* Copy buffer data from src to dst using VkCmdCopyBuffer
			*
			* @param src Pointer to the source buffer to copy from
			* @param dst Pointer to the destination buffer to copy tp
			* @param queue Pointer
			* @param copyRegion (Optional) Pointer to a copy region, if NULL, the whole buffer is copied
			*
			* @note Source and destionation pointers must have the approriate transfer usage flags set (TRANSFER_SRC / TRANSFER_DST)
			*/
			// void copyBuffer(vks::Buffer* src, vks::Buffer* dst, VkQueue queue, VkBufferCopy* copyRegion = nullptr);

			/**
			* Create a command pool for allocation command buffers from
			*
			* @param queueFamilyIndex Family index of the queue to create the command pool for
			* @param createFlags (Optional) Command pool creation flags (Defaults to VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
			*
			* @note Command buffers allocated from the created pool can only be submitted to a queue with the same family index
			*
			* @return A handle to the created command buffer
			*/
			VkCommandPool CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

			/**
			* Check if an extension is supported by the (physical device)
			*
			* @param extension Name of the extension to check
			*
			* @return True if the extension is supported (present in the list read at device creation time)
			*/
			bool ExtensionSupported(std::string extension);

		};
	}
}