#pragma once
#include <structures/vk_context.hpp>
#include <util/memory_view.hpp>
#include <thread>
#include <array>
#include <structures/semaphore.hpp>
#include <atomic>

namespace structures{

// stager uses a single staging buffer which is used in fifo mode for uploading data larger than the staging buffer
class stager{
public:
    enum class transfer_direction{
        upload,
        download,
        COUNT
    };
    struct task_base{virtual ~task_base(){}};
    struct task_common{     // common attributes in separate to be able to order the attributes in the structs
        util::memory_view<const uint8_t>    data_upload{};
        util::memory_view<uint8_t>          data_download{};
        util::memory_view<VkSemaphore>      wait_semaphores{};
        util::memory_view<uint32_t>         wait_flags{};
        util::memory_view<VkSemaphore>      signal_semaphores{};
        semaphore*                          cpu_semaphore{};
    };   
    struct staging_buffer_info: task_base{
        transfer_direction                  transfer_dir{transfer_direction::upload};
        VkBuffer                            dst_buffer{};
        size_t                              dst_buffer_offset{};
        task_common                         common{};

        staging_buffer_info() {}
        staging_buffer_info(transfer_direction transfer_dir, VkBuffer buffer, size_t dst_buffer_offset, const task_common& common): transfer_dir(transfer_dir), dst_buffer(buffer), dst_buffer_offset(dst_buffer_offset), common(common) {}
    };
    struct staging_image_info: task_base{
        transfer_direction                  transfer_dir{transfer_direction::upload};
        VkImage                             dst_image{};
        VkImageLayout                       start_layout{};
        VkImageLayout                       end_layout{};   // after staging the image is in this image layout
        VkImageSubresourceLayers            subresource_layers{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        uint32_t                            bytes_per_pixel{4};
        VkOffset3D                          image_offset{};
        VkExtent3D                          image_extent{1, 1, 1};
        task_common                         common{};
    };

    // needed for task_thread dispatch
    void init();
    // interrupts current transfers and joins with the task_thread
    void cleanup();
    // add an upload task
    void add_staging_task(const staging_buffer_info& stage_info);
    // add an upload task
    void add_staging_task(const staging_image_info& stage_info);
    // set staging buffer size (defaults to 128 MB)
    void set_staging_buffer_size(size_t size);
    // waiting for completion of all upload tasks
    void wait_for_completion();

    ~stager() {assert(!_task_thread.joinable() && "Call to cleanup() missing before destruction");}

private:
    using unique_task = std::unique_ptr<task_base>;
    semaphore               _task_semaphore{};
    std::vector<unique_task> _staging_tasks{};
    std::mutex              _task_add_mutex{};
    std::array<VkFence, 2>  _task_fences{};
    int                     _fence_index{};
    std::thread             _task_thread{};
    std::atomic_bool        _thread_finish{};
    std::atomic_uint32_t    _wait_completion{};
    structures::semaphore   _completion_sempahore{};
    VkCommandPool           _command_pool{};
    std::array<VkCommandBuffer, 2>_command_buffers{};
    std::atomic_size_t      _staging_buffer_size{1u<<27}; // default size is 128 MB
    buffer_info             _staging_buffer{};
    uint8_t*                _staging_buffer_mapped{};

    void _task_thread_function();
};
}

namespace globals{
extern structures::stager stager;
}