/*
    Generates the final sokol_gfx.h compatible C header with
    embedded shader source/byte code, uniform block structs
    and sg_shader_desc structs.
*/
#include "shdc.h"
#include "fmt/format.h"
#include "pystring.h"
#include <stdio.h>

namespace shdc {

static const spirvcross_source_t* find_spirvcross_source(const spirvcross_t& spirvcross, slang_t::type_t slang, int snippet_index) {
    for (const auto& src: spirvcross.sources[(int)slang]) {
        if (src.snippet_index == snippet_index) {
            return &src;
        }
    }
    return nullptr;
}

static error_t write_program(FILE* f, const input_t& inp, const spirvcross_t& spirvcross, slang_t::type_t slang) {
    for (const auto& item: inp.programs) {
        const program_t& prog = item.second;
        int vs_snippet_index = inp.snippet_map.at(prog.vs_name);
        int fs_snippet_index = inp.snippet_map.at(prog.fs_name);
        const spirvcross_source_t* vs_src = find_spirvcross_source(spirvcross, slang, vs_snippet_index);
        const spirvcross_source_t* fs_src = find_spirvcross_source(spirvcross, slang, fs_snippet_index);
        if (!vs_src) {
            return error_t(inp.path, inp.snippets[vs_snippet_index].lines[0],
                fmt::format("no generated '{}' source for vertex shader '{}' in program '{}'",
                    slang_t::to_str(slang), prog.vs_name, prog.name));
        }
        if (!fs_src) {
            return error_t(inp.path, inp.snippets[vs_snippet_index].lines[0],
                fmt::format("no generated '{}' source for fragment shader '{}' in program '{}'",
                    slang_t::to_str(slang), prog.fs_name, prog.name));
        }
        std::vector<std::string> vs_lines, fs_lines;
        pystring::splitlines(vs_src->source_code, vs_lines);
        pystring::splitlines(fs_src->source_code, fs_lines);
        fmt::print(f, "static const char* {}_{}_{}_src =\n", prog.name, prog.vs_name, slang_t::to_str(slang));
        for (const auto& line: vs_lines) {
            fmt::print(f, "\"{}\\n\"\n", line);
        }
        fmt::print(f, ";\n");
        fmt::print(f, "static const char* {}_{}_{}_src =\n", prog.name, prog.fs_name, slang_t::to_str(slang));
        for (const auto& line: fs_lines) {
            fmt::print(f, "\"{}\\n\"\n", line);
        }
        fmt::print(f, ";\n");
    }
    return error_t();
}

error_t sokol_t::gen(const args_t& args, const input_t& inp, const spirvcross_t& spirvcross, const bytecode_t& bytecode) {
    FILE* f = fopen(args.output.c_str(), "w");
    if (!f) {
        return error_t(inp.path, 0, fmt::format("failed to open output file '{}'", args.output));
    }

    fmt::print(f, "#pragma once\n");
    fmt::print(f, "/* #version:{}# machine generated, don't edit */\n", SOKOL_SHDC_VERSION);
    fmt::print(f, "#include <stdint.h>\n");
    fmt::print(f, "#if !defined(SOKOL_GFX_INCLUDED)\n");
    fmt::print(f, "#error \"Please include sokol_gfx.h before {}\"\n", pystring::os::path::basename(args.output));
    fmt::print(f, "#endif\n");
    if (args.slang & slang_t::bit(slang_t::GLSL330)) {
        fmt::print(f, "#if defined(SOKOL_GLCORE33)\n");
        write_program(f, inp, spirvcross, slang_t::GLSL330);
        fmt::print(f, "#endif /* SOKOL_GLCORE33 */\n");
    }
    if (args.slang & slang_t::bit(slang_t::GLSL300ES)) {
        fmt::print(f, "#if defined(SOKOL_GLES3)\n");
        write_program(f, inp, spirvcross, slang_t::GLSL300ES);
        fmt::print(f, "#endif /* SOKOL_GLES3 */\n");
    }
    if (args.slang & slang_t::bit(slang_t::GLSL100)) {
        fmt::print(f, "#if defined(SOKOL_GLES2)\n");
        write_program(f, inp, spirvcross, slang_t::GLSL100);
        fmt::print(f, "#endif /* SOKOL_GLES2 */\n");
    }
    if (args.slang & slang_t::bit(slang_t::HLSL5)) {
        fmt::print(f, "#if defined(SOKOL_D3D11)\n");
        write_program(f, inp, spirvcross, slang_t::HLSL5);
        fmt::print(f, "#endif /* SOKOL_D3D11 */\n");
    }
    if (args.slang & slang_t::bit(slang_t::METAL_MACOS)) {
        fmt::print(f, "#if defined(SOKOL_METAL)\n");
        write_program(f, inp, spirvcross, slang_t::METAL_MACOS);
        fmt::print(f, "#endif /* SOKOL_METAL */\n");
    }
    if (args.slang & slang_t::bit(slang_t::METAL_IOS)) {
        fmt::print(f, "#if defined(SOKOL_METAL)\n");
        write_program(f, inp, spirvcross, slang_t::METAL_IOS);
        fmt::print(f, "#endif /* SOKOL_METAL */\n");
    }
    fclose(f);
    return error_t();
}

} // namespace shdc