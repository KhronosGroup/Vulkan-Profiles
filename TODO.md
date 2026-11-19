# TODO

Items left to deal with after adding the capability block `depends` condition (schema 0.8.3, #1090).

## Profiles layer

- [ ] **Evaluate feature aliases in `depends`.** The layer reads the named feature structure of the simulated device. Before the end of the profile loading, a promoted structure and its Vulkan 1.x equivalent (e.g. `VkPhysicalDeviceProtectedMemoryFeatures` / `VkPhysicalDeviceVulkan11Features`) may not be synchronized yet, so a condition naming one may miss a value the profile set on the other.
