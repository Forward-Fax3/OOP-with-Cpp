#include <iostream>
#include <limits>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <SDL3/SDL_vulkan.h>

#include "VulkanContext.hpp"
#include "VulkanCore.hpp"
#include "Log.hpp"


PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pMessenger)
{
	return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT messenger,
	VkAllocationCallbacks const* pAllocator)
{
	return pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}

namespace OWC::Graphics
{
	VulkanContext::VulkanContext(const SDL_Window& windowHandle) // TODO: validation layers and debug messenger
	{
		const bool IsDebug = IsDebugMode();

		(void)windowHandle; // unused for now
		const auto runtimeVersion = vk::enumerateInstanceVersion();
		if (runtimeVersion < g_VulkanVersion)
			Log<LogLevel::Critical>("Vulkan runtime version {}.{}.{} is lower than the required version {}.{}.{}",
				VK_VERSION_MAJOR(runtimeVersion), VK_VERSION_MINOR(runtimeVersion), VK_VERSION_PATCH(runtimeVersion),
				VK_VERSION_MAJOR(g_VulkanVersion), VK_VERSION_MINOR(g_VulkanVersion), VK_VERSION_PATCH(g_VulkanVersion));

		VulkanCore::Init();
		auto& vkCore = VulkanCore::GetInstance();

		std::vector<const char*> extentions;
		if constexpr (!IsDistributionMode())
		{
			pfnVkCreateDebugUtilsMessengerEXT = std::bit_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkCore.GetVKInstance().getProcAddr("vkCreateDebugUtilsMessengerEXT"));
			pfnVkDestroyDebugUtilsMessengerEXT = std::bit_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkCore.GetVKInstance().getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
			if (!pfnVkCreateDebugUtilsMessengerEXT || !pfnVkDestroyDebugUtilsMessengerEXT)
				Log<LogLevel::Critical>("Failed to load Vulkan debug utils functions");

			vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
				vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
			);

			vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
				vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
				vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
				vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
			);

			m_DebugCallback = vkCore.GetVKInstance().createDebugUtilsMessengerEXT(
				vk::DebugUtilsMessengerCreateInfoEXT()
				.setMessageSeverity(severityFlags)
				.setMessageType(messageTypeFlags)
				.setPfnUserCallback( // TODO: do more with pCallbackData
					[](
						vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
						vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
						const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
						void* // unused userData
						) -> vk::Bool32
		{
						if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
							Log<LogLevel::Error>("Vulkan Validation Layer: {}: {}", vk::to_string(messageTypes), pCallbackData->pMessage);
						else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
							Log<LogLevel::Warn>("Vulkan Validation Layer: {}: {}", vk::to_string(messageTypes), pCallbackData->pMessage);
						else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
							Log<LogLevel::Trace>("Vulkan Validation Layer: {}: {}", vk::to_string(messageTypes), pCallbackData->pMessage);
						else 
							Log<LogLevel::Trace>("Vulkan Validation Layer: {}: {}", vk::to_string(messageTypes), pCallbackData->pMessage);
						return VK_FALSE;
		}
				)
			);
		}

		auto physcalDevices = vkCore.GetVKInstance().enumeratePhysicalDevices();

		vkCore.SetPhysicalDevice(physcalDevices.front()); // TODO: select proper physical device

		auto queueFamilies = vkCore.GetPhysicalDev().getQueueFamilyProperties();
		constexpr size_t sizeMax = std::numeric_limits<std::size_t>::max();
		[[maybe_unused]] size_t presentQueueFamilyIndex = sizeMax;
		size_t graphicsQueueFamilyIndex = sizeMax;
		size_t computeQueueFamilyIndex = sizeMax;
		size_t transferQueueFamilyIndex = sizeMax;

		for (size_t i = 0; i < queueFamilies.size(); i++) // try to find all queue families in one loop and have different queue families if possible
		{
			// TODO: add surface handle
//			if (presentQueueFamilyIndex == sizeMax)
//			{
//				const vk::Bool32 presentSupport = vkCore.GetPhysicalDev().getSurfaceSupportKHR(static_cast<uint32_t>(i), vk::SurfaceKHR()); // TODO: surface handle
//				if (presentSupport)
//					presentQueueFamilyIndex = i;
//			}

			if (graphicsQueueFamilyIndex == sizeMax && (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics)
				graphicsQueueFamilyIndex = i;

			else if (computeQueueFamilyIndex == sizeMax && (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
				computeQueueFamilyIndex = i;

			// prefer a dedicated transfer queue family
			else if (transferQueueFamilyIndex == sizeMax && (queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer
				&& (queueFamilies[i].queueFlags & vk::QueueFlags(vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute)) == vk::QueueFlags(0))
				transferQueueFamilyIndex = i;

			if ((graphicsQueueFamilyIndex /*| presentQueueFamilyIndex*/ | computeQueueFamilyIndex | transferQueueFamilyIndex) != sizeMax)
				break;
		}

		// check if compute queue family is found, otherwise find any even if it shares with others
		if (computeQueueFamilyIndex == sizeMax)
			for (size_t i = 0; i < queueFamilies.size(); i++)
				if ((queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)
				{
					computeQueueFamilyIndex = i;
					break;
				}

		// check if transfer queue family is found, otherwise find any even if it shares with others
		if (transferQueueFamilyIndex == sizeMax)
			for (size_t i = 0; i < queueFamilies.size(); i++)
				if ((queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)
				{
					transferQueueFamilyIndex = i;
					break;
				}

		// if no transfer queue found, use graphics queue
		if (transferQueueFamilyIndex != sizeMax)
			transferQueueFamilyIndex = graphicsQueueFamilyIndex;

		std::cout << "Selected Queue Families: \n"
			<< " Graphics: " << graphicsQueueFamilyIndex << "\n"
			<< " Compute: " << computeQueueFamilyIndex << "\n"
			<< " Transfer: " << transferQueueFamilyIndex << "\n";
		
		// TODO: check queue family index validity

		float queuePriority = 1.0f;
		vk::DeviceQueueCreateInfo deviceGraphicsQueueCreateInfo;
		deviceGraphicsQueueCreateInfo
			.setQueueFamilyIndex(static_cast<uint32_t>(graphicsQueueFamilyIndex))
			.setQueueCount(1)
			.setPQueuePriorities(&queuePriority);

		vk::DeviceQueueCreateInfo deviceComputeQueueCreateInfo;
		deviceComputeQueueCreateInfo
			.setQueueFamilyIndex(static_cast<uint32_t>(computeQueueFamilyIndex))
			.setQueueCount(1)
			.setPQueuePriorities(&queuePriority);

		vk::DeviceQueueCreateInfo deviceTransferQueueCreateInfo;
		deviceTransferQueueCreateInfo
			.setQueueFamilyIndex(static_cast<uint32_t>(transferQueueFamilyIndex))
			.setQueueCount(1)
			.setPQueuePriorities(&queuePriority);

		std::array<vk::DeviceQueueCreateInfo, 3> deviceQueueCreateInfos
		{
			deviceGraphicsQueueCreateInfo,
			deviceComputeQueueCreateInfo,
			deviceTransferQueueCreateInfo
		};

		vkCore.SetDevice(
			vkCore.GetPhysicalDev().createDevice(
				vk::DeviceCreateInfo()
					.setQueueCreateInfoCount(3)
					.setPQueueCreateInfos(deviceQueueCreateInfos.data())
			)
		);
	}

	VulkanContext::~VulkanContext()
	{
		auto& vkCore = VulkanCore::GetConstInstance();

		vkCore.GetDevice().destroy();
		if (m_DebugCallback)
			vkCore.GetVKInstance().destroyDebugUtilsMessengerEXT(m_DebugCallback);
		vkCore.GetVKInstance().destroy();
		VulkanCore::Shutdown();
	}

	void VulkanContext::SwapBuffers()
	{

	}

	void VulkanContext::WaitForIdle()
	{
		VulkanCore::GetConstInstance().GetDevice().waitIdle();
	}
}