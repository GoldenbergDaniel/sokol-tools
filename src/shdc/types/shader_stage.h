#pragma once

namespace shdc {

// a shader stage
struct ShaderStage {
    enum Enum {
        INVALID,
        VS,
        FS,
    };
    static const char* to_str(Enum e);
    static bool is_vs(Enum e);
    static bool is_fs(Enum e);
};

inline const char* ShaderStage::to_str(ShaderStage::Enum e) {
    switch (e) {
        case VS: return "VS";
        case FS: return "FS";
        default: return "INVALID";
    }
}

inline bool ShaderStage::is_vs(ShaderStage::Enum e) {
    return VS == e;
}

inline bool ShaderStage::is_fs(ShaderStage::Enum e) {
    return FS == e;
}

} // namespace shdc