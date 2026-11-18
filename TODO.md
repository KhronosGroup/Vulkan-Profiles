# TODO

Items left to deal with after adding the capability block `depends` condition (schema 0.8.3, #1090).

## vkprofiles

- [ ] **Strip the capabilities of unsatisfied conditions.** Add `transform --strip depends`: set the capabilities whose condition isn't satisfied by the profile to the value of the devices not supporting the condition, or remove them when there is no such value, including the capability blocks with a `depends` condition not satisfied by the profile.
- [ ] **Combine `depends` blocks with union and intersection.** `vkprofiles combine --mode difference` compares the blocks sharing the same condition, but union and intersection skip the blocks with a `depends` condition with a warning.
- [ ] **Document `depends` in the whitepaper and in `vkprofiles doc`.** Explain how `--pull depends` extracts the conditional capabilities: a profile only lists a dependency when the profile doesn't satisfy it, e.g. `maxGeometryShaderInvocations` 0 unconditionally and 32 in a block with the `geometryShader` condition.

## Profiles layer

- [ ] **Evaluate feature aliases in `depends`.** The layer reads the named feature structure of the simulated device. Before the end of the profile loading, a promoted structure and its Vulkan 1.x equivalent (e.g. `VkPhysicalDeviceProtectedMemoryFeatures` / `VkPhysicalDeviceVulkan11Features`) may not be synchronized yet, so a condition naming one may miss a value the profile set on the other.
