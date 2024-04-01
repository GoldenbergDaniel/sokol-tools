#pragma once
#include <string>
#include "fmt/format.h"
#include "shader_stage.h"
#include "type.h"

namespace shdc::refl {

struct StorageBuffer {
    static const int Num = 4;   // must be identical with SG_MAX_SHADERSTAGE_STORAGE_BUFFERS
    ShaderStage::Enum stage = ShaderStage::Invalid;
    int slot = -1;
    std::string inst_name;
    Type struct_refl;

    bool equals(const StorageBuffer& other) const;
    void dump_debug(const std::string& indent) const;
};

inline bool StorageBuffer::equals(const StorageBuffer& other) const {
    return (stage == other.stage)
        && (slot == other.slot)
        && (inst_name == other.inst_name)
        && (struct_refl.equals(other.struct_refl));
}

inline void StorageBuffer::dump_debug(const std::string& indent) const {
    const std::string indent2 = indent + "  ";
    fmt::print(stderr, "{}-\n", indent);
    fmt::print(stderr, "{}stage: {}\n", indent2, ShaderStage::to_str(stage));
    fmt::print(stderr, "{}slot: {}\n", indent2, slot);
    fmt::print(stderr, "{}inst_name: {}\n", indent2, inst_name);
    fmt::print(stderr, "{}struct:\n", indent2);
    struct_refl.dump_debug(indent2);
}

} // namespace
