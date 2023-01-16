<!-- markdownlint-disable MD041 -->
<p align="left"><img src="https://vulkan.lunarg.com/img/NewLunarGLogoBlack.png" alt="LunarG" width=263 height=113 /></p>
<p align="left">Copyright © 2021-2023 LunarG, Inc.</p>

<p align="center"><img src="./images/logo.png" width=400 /></p>

[![Creative Commons][3]][4]

[3]: https://i.creativecommons.org/l/by-nd/4.0/88x31.png "Creative Commons License"
[4]: https://creativecommons.org/licenses/by-nd/4.0/

# Vulkan Profiles Toolset

The *Vulkan Profiles Toolset* is a collection of tools delivered with the [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) for Vulkan application developers to leverage *Vulkan Profiles* while developing a Vulkan application.

* **[Vulkan Profiles Tools Overview](./OVERVIEW.md)**: Discover the components and how they work together.
* **[Vulkan Profiles Documentation](https://vulkan.lunarg.com/doc/sdk/latest/windows/profiles_definitions.html)**: *Tables to compare profiles capabilities*.
* **[Change Log](./CHANGELOG.md)**: The history of *Vulkan Profiles Toolset* releases.
* **[Contributing](./CONTRIBUTING.md)**: How to contribute to the *Vulkan Profiles* repository.
* **[Bug reports](https://github.com/KhronosGroup/Vulkan-Profiles)**: Open a GitHub issue when you encounter a bug.

## Default branch changed to 'main' 2023-01-16

As discussed in #338, the default branch of this repository is now 'main'. This change should be largely transparent to repository users, since github rewrites many references to the old 'master' branch to 'main'. However, if you have a checked-out local clone, you may wish to take the following steps as recommended by github:

```sh
git branch -m master main
git fetch origin
git branch -u origin/main main
git remote set-head origin -a
```

--------------
## Contact Information
* [Christophe Riccio](mailto:christophe@lunarg.com)
* [Žiga Markuš](mailto:ziga@lunarg.com)

--------------
## Information for Developing or Contributing:
Please see the [CONTRIBUTING.md](./CONTRIBUTING.md) file in this repository for more details. 

--------------
## How to Build and Run
[BUILD.md](./BUILD.md) provides instructions to build the repository, which allows regenerating the *Vulkan Profiles Toolset* against new Vulkan Header API version and with supports of new profiles or updated profiles.

--------------
## Version Tagging Scheme
Updates to the *Vulkan Profiles* repository which correspond to a new Vulkan specification release are tagged using the following format: v<version> (e.g., v1.1.96).

Note: Marked version releases have undergone thorough testing but do not imply the same quality level as SDK tags. SDK tags follow the sdk-<version>.<patch> format (e.g., sdk-1.1.92.0).

This scheme was adopted following the 1.1.96 Vulkan specification release.

--------------
## License

This work is released as open source under a [Apache-style license](./LICENSE.md) from Khronos including a Khronos copyright.

--------------
## Downloads

*Vulkan Profiles Tools* is delivered with the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).


