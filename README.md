# VulkanSetup

This cmake project holds a basic setup for Vulkan as the graphics/compute API, SDL2 as the OS-independent window manager and DearImgui for UI creation.
For Memory allocations the VulkanMemoryManager is used.

Imgui is included in this project, as multiview support, vertical layout support and node support needed handmerging and were not readily available on the original imgui github page.

The framework supports:
- a memory_view struct which gives a view to sequential data and allows for unified array input to functions, as well as multiple convenience functions being performed on the data it holds (eg. hash computation, search...)
- a c-file abstraction for easier c-file loading
- a template change_tracker to support tracking changes made to the structure it holds with const and non-const (read and write) access and a change bool for status
- a data structure to hold xArray like data (multidimensional data with different attributes defined on different dimensions) including dimensional and index based addressing
- a data abstraction holding cpu and gpu data
- a command line parser
- a global descriptor set storage for globally available gpu data that can be bound to pipeline exectuions (eg. global gpu data and global textures)
- a copy of robin hood hash maps to replace unordered_map from the standard
- a global vulkan context that holds VkInstance, VkPhysicalDevice, VkDevice, VkQueues (for each type a queue), ... plus a registry for vulkan resources that have to be destructed
- a vulkan initializer list for most vulkan structs for fast and versatile vulkan object creation
- a vma initializer list for fast and versatile vma memory allocation usage
- a vk util for easier object creation including VkResult checks and automatic registration in the global vk_context as well as easy object destruction with deregistration
- a rudimentary logger
- a netcdf and csv loader
- a global sampler registry
- a settings manager for writing out settings (written out binary to safe space) (a small json loader can be found in imgui_nodes)
- a gpu stager running on an extra thread which waits for buffer/image transfer tasks and executes them on an extra thread. Synchronization is provided on the cpu via wait_for_completion and via semaphores for the gpu (have to be inserterd into the task info)
- a file loader running on an extra thread which waits for file loading tasks and executes them in the background.
- a workbench based setup to create multiple visualization windows with automatic update calls when a change to data occurs.
- automatic recompilation of shader when shaders were updated in combination with shipping of precompiled shaders in the repo to avoid problems with recompilation.