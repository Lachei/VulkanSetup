#pragma once
#include <string>
#include <structures/data.hpp>
#include <util/memory_view.hpp>
#include <imgui/imgui.h>
#include <structures/brushes.hpp>
#include <structures/vk_context.hpp>
#include <structures/datasets.hpp>
#include <structures/enum_names.hpp>
#include <structures/dynamic_bitset.hpp>

namespace structures{

enum class median_type: uint32_t{
    none,
    arithmetic,
    synthetic,
    geometric,
    COUNT
};
static enum_names<median_type> median_type_names{
    "none",
    "arithmetic",
    "synthetic",
    "geometric"
};
using median_iteration = enum_iteration<median_type>;

struct drawlist{
    std::string             id{};
    std::string             name{};
    std::string_view        parent_dataset{};
    std::string_view        parent_templatelist{};
    struct appearance{
        ImVec4      color{1,1,1,1};
        bool        show{true};
        bool        show_histogram{};
    };
    change_tracker<appearance> appearance_drawlist{};
    std::vector<float>      brush_ratios_to_parent{};
    change_tracker<bool>    immune_to_global_brushes{};
    change_tracker<appearance> appearance_median{};
    change_tracker<median_type> median_typ{};  
    dynamic_bitset<>        active_indices_bitset{};                

    buffer_info             median_buffer{};                // linear array buffer containing the median values for all attributes
    buffer_info             active_indices_bitset_gpu{};
    buffer_info             priority_colors_gpu{};
    std::vector<buffer_info> derived_data_infos{};          // vulkan buffer need e.g. for large vis counting
    tracked_brush           local_brushes{};
    buffer_info             local_brushes_gpu{};

    //TODO: add cluster and line bundles

    bool                        any_change() const          {return appearance_drawlist.changed || immune_to_global_brushes.changed || appearance_median.changed || median_typ.changed || local_brushes.changed;}
    void                        clear_change()              {appearance_drawlist.changed = false; immune_to_global_brushes.changed = false; appearance_median.changed = false; median_typ.changed = false; local_brushes.changed = false;}
    dataset&                    dataset_write() const       {return globals::datasets().at(parent_dataset)();}
    const structures::dataset&  dataset_read() const        {return globals::datasets.read().at(parent_dataset).read();} 
    //templatelist&               templatelist_write() const  {return *globals::datasets().at(parent_dataset)().templatelist_index[parent_templatelist];}
    const structures::templatelist& const_templatelist()const{return *globals::datasets().at(parent_dataset)().templatelist_index[parent_templatelist];}
};
using tracked_drawlist = unique_tracker<drawlist>;
using drawlists_t = change_tracker<std::map<std::string_view, tracked_drawlist>>;
}

namespace globals{
extern structures::drawlists_t drawlists;
extern std::vector<std::string_view> selected_drawlists;
}