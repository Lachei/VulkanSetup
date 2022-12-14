#pragma once

#include <structures/data.hpp>
#include <structures/vk_context.hpp>
#include <util/memory_view.hpp>
#include <structures/vk_context.hpp>
#include <util/vk_util.hpp>
#include <util/ranges.hpp>
#include <structures/array_struct.hpp>
#include <vector>

namespace util{
namespace data{
struct gpu_header{
    uint32_t dimension_count;
    uint32_t column_count;
    uint32_t data_address_offset;
    uint32_t _;
};

template<typename T = float>
inline uint64_t header_size (const structures::data<T>& data) {
    uint64_t size = 4;                  // header fields
    size += data.dimension_sizes.size();     // dimension sizes
    size += data.column_dimensions.size();   // column dimension counts
    size += data.column_dimensions.size();   // colummn dimension offsets
    for(const auto& a: data.column_dimensions)
        size += a.size();               // column dimension
    size *= sizeof(uint32_t);
    size += data.columns.size() * sizeof(uint64_t); // column data addresses
    return size;
}

template<typename T>
inline structures::dynamic_struct<gpu_header, uint32_t> create_packed_header(const structures::data<T>& data, util::memory_view<const structures::buffer_info> buffers){
    assert(buffers.size() == data.columns.size());
    
    std::vector<VkDeviceAddress> device_addresses(buffers.size());
    for(int i: util::size_range(buffers))
        device_addresses[i] = util::vk::get_buffer_address(buffers[i]);
    
    uint32_t header_size = static_cast<uint32_t>(::util::data::header_size(data));
    structures::dynamic_struct<gpu_header, uint32_t> packed_header((header_size - sizeof(gpu_header)) / sizeof(uint32_t));
    packed_header->dimension_count = static_cast<uint32_t>(data.dimension_sizes.size());
    packed_header->column_count = static_cast<uint32_t>(data.columns.size());

    // dimension sizes
    uint32_t cur_offset{};
    for(const auto& dim_size: data.dimension_sizes)
        packed_header[cur_offset++] = dim_size;
    
    // column dimension counts
    for(const auto& column_dimension: data.column_dimensions)
        packed_header[cur_offset++] = static_cast<uint32_t>(column_dimension.size());
    
    // reserving space for column dimension offsets
    uint32_t column_dimension_offset_index = cur_offset;
    cur_offset += static_cast<uint32_t>(data.column_dimensions.size());

    // column dimensions and their offset
    for(int i: size_range(data.column_dimensions)){
        packed_header[column_dimension_offset_index++] = cur_offset;
        for(auto j: data.column_dimensions[i])
            packed_header[cur_offset++] = j;
    }

    // data addresses
    packed_header->data_address_offset = cur_offset;
    for(auto addr: device_addresses){
        packed_header.reinterpret_at<uint64_t>(cur_offset) = addr;
        cur_offset += sizeof(addr) / sizeof(uint32_t);
    }

    return packed_header;
}
}
}