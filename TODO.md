# TODO

Items left to deal with after adding the capability block `depends` condition (schema 0.8.3, #1090).

## Profiles

- [ ] **Upstream the Roadmap 2026 change.** `profiles/Khronos/VP_KHR_roadmap.json` now uses a `depends` block for `pipelineProtectedAccess`, but the build copies `VP_KHR_roadmap.json` from the Vulkan-Headers registry. The change must land in the Vulkan-Docs registry to reach the generated library, documentation and tests.
- [ ] **Convert `VP_LUNARG_minimum_requirements` to `depends` blocks**: `vulkan14dependent` (still in `optionals`), and the requirements removed in revision 4 because they depend on optional features (`lineWidthGranularity` on `wideLines`, `shaderSignedZeroInfNanPreserveFloat16` on `shaderFloat16`). Blocked by the pull transform item below, otherwise `VkProfiles_TestMinimumRequirementsDiff` fails.

## vkprofiles

- [ ] **Generate `depends` blocks when pulling requirements.** `transform --pull requirements` respects existing `depends` blocks, but it doesn't create them from vk.xml conditional requirements (e.g. `<require depends="VkPhysicalDeviceProtectedMemoryFeatures::protectedMemory">` for `pipelineProtectedAccess`, or property requirements depending on `wideLines` / `largePoints`). This changes the desktop and Android generated profiles, the library and the documentation.
- [ ] **Combine `depends` blocks.** `vkprofiles combine` skips the blocks with a `depends` condition with a warning. Union, intersection and difference could combine the blocks sharing the same condition.

## Profiles layer

- [ ] **Evaluate feature aliases in `depends`.** The layer reads the named feature structure of the simulated device. Before the end of the profile loading, a promoted structure and its Vulkan 1.x equivalent (e.g. `VkPhysicalDeviceProtectedMemoryFeatures` / `VkPhysicalDeviceVulkan11Features`) may not be synchronized yet, so a condition naming one may miss a value the profile set on the other.
