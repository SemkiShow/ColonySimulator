# SPDX-FileCopyrightText: 2026 SemkiShow
#
# SPDX-License-Identifier: GPL-3.0-only

import subprocess
import clang.cindex
import os, re
from pathlib import Path

ROOT_PATH = Path(__file__).parent.parent

INCLUDE_PATH = ["include", "thirdparty/raylib/src", "thirdparty/RayUI/include"]

HEADERS = [
    "include/**/*.hpp",
    "thirdparty/raylib/src/raylib.h",
    "thirdparty/raylib/src/raymath.h",
    "thirdparty/RayUI/include/**/*.hpp",
]

HEADER_BLACKLIST = ["include/Mods.hpp", "include/Json.hpp"]

STRUCT_BLACKLIST = ["AudioStream", "ModelAnimation"]

VARIABLE_BLACKLIST = []

FUNCTION_BLACKLIST = ["Vector2ToJson", "JsonToVector2"]


def get_headers() -> list[str]:
    blacklist = {os.path.abspath(ROOT_PATH / b) for b in HEADER_BLACKLIST}
    files: list[str] = []
    for regex in HEADERS:
        for file in ROOT_PATH.glob(regex):
            abs_file = os.path.abspath(file)
            if abs_file not in blacklist:
                files.append(str(file))
    return files


def get_include_path(header: str) -> str:
    file_path = Path(header)
    abs_file = file_path.resolve()
    best_path = None

    for include_dir in INCLUDE_PATH:
        abs_include = (ROOT_PATH / include_dir).resolve()
        try:
            rel = abs_file.relative_to(abs_include)

            if best_path is None or len(str(rel)) < len(str(best_path)):
                best_path = rel
        except ValueError:
            continue

    if best_path:
        return str(best_path).replace("\\", "/")

    # Fallback to project root relative
    return str(file_path.relative_to(ROOT_PATH)).replace("\\", "/")


def translate_type(cpp_type: str) -> str:
    cpp_type = cpp_type.lower()

    if "vector" in cpp_type or "list" in cpp_type:
        return "any[]"
    if re.search(r'\b(int|float|double|long|size_t)\b', cpp_type):
        return "number"
    if "bool" in cpp_type:
        return "boolean"
    if "string" in cpp_type:
        return "string"
    if "void" in cpp_type:
        return "nil"

    return "any"


def clean_type(type_str: str) -> str:
    type_str = type_str.replace("_Bool", "bool")

    type_str = type_str.replace("std::__cxx11::basic_string<char>", "std::string")
    type_str = type_str.replace("std::basic_string<char>", "std::string")

    return type_str


def sanitize_name(name: str) -> str:
    LUA_KEYWORDS = [
        "and",
        "break",
        "do",
        "else",
        "elseif",
        "end",
        "false",
        "for",
        "function",
        "goto",
        "if",
        "in",
        "local",
        "nil",
        "not",
        "or",
        "repeat",
        "return",
        "then",
        "true",
        "until",
        "while",
    ]
    if name in LUA_KEYWORDS:
        return f"{name}_"
    return name


def parse_struct(node):
    if not node.is_definition():
        return

    struct_name = sanitize_name(node.spelling)
    if not struct_name or struct_name in STRUCT_BLACKLIST:
        return

    cpp_entries = []
    lua_data = []

    explicit_ctors = []
    ctor_lua_sigs = []
    member_data = []

    def get_base_classes(node):
        bases = set()
        for child in node.get_children():
            if child.kind == clang.cindex.CursorKind.CXX_BASE_SPECIFIER:
                base_declaration = child.type.get_declaration()
                base_name = base_declaration.spelling
                if base_name:
                    bases.add(base_name)
                    bases.update(get_base_classes(base_declaration))
        return bases

    base_classes = list(get_base_classes(node))

    def handle_method(child):
        name = sanitize_name(child.spelling)
        ret = clean_type(child.result_type.get_canonical().spelling)
        args = [clean_type(p.type.get_canonical().spelling) for p in child.get_arguments()]
        lua_args = [f"a{i}: {translate_type(t)}" for i, t in enumerate(args)]

        const_q = " const" if child.is_const_method() else ""
        ptr_type = f"{ret} ({struct_name}::*)({', '.join(args)}){const_q}"
        if child.is_static_method():
            ptr_type = f"{ret} (*)({', '.join(args)})"

        cpp_entries.append(f'"{name}", static_cast<{ptr_type}>(&{struct_name}::{name})')
        lua_data.append(
            f"---@method {name}({', '.join(lua_args)}): {translate_type(child.result_type.spelling)}"
        )

    def handle_field(child):
        name = sanitize_name(child.spelling)
        raw_type = child.type.get_canonical().spelling

        cpp_entries.append(f'"{name}", &{struct_name}::{name}')
        lua_data.append(f"---@field {name} {translate_type(child.type.spelling)}")

        if "[" not in raw_type:
            member_data.append((clean_type(raw_type), name))

    def handle_constructor(child):
        if child.is_copy_constructor() or child.is_move_constructor():
            return

        args_cpp = [clean_type(p.type.get_canonical().spelling) for p in child.get_arguments()]
        args_lua = [
            f"{sanitize_name(p.spelling)}: {translate_type(p.type.spelling)}"
            for p in child.get_arguments()
        ]

        explicit_ctors.append(f"{struct_name}({', '.join(args_cpp)})")
        ctor_lua_sigs.append(f"---@overload fun({', '.join(args_lua)}): {struct_name}")

    for child in node.get_children():
        if child.access_specifier != clang.cindex.AccessSpecifier.PUBLIC:
            continue

        if child.kind == clang.cindex.CursorKind.CONSTRUCTOR:
            handle_constructor(child)
        elif child.kind == clang.cindex.CursorKind.FIELD_DECL:
            handle_field(child)
        elif child.kind == clang.cindex.CursorKind.CXX_METHOD:
            if not child.spelling.startswith("operator"):
                handle_method(child)

    # Inheritance in Lua
    lua_inherit_str = f" : {', '.join(base_classes)}" if base_classes else ""
    lua_out.append(f"---@class {struct_name}{lua_inherit_str}")

    lua_out.extend(ctor_lua_sigs)

    # Constructors
    constructor_code = ""
    if explicit_ctors:
        constructor_code = f'sol::constructors<{", ".join(explicit_ctors)}>()'
    else:
        lambdas = [f"[]() {{ return {struct_name}{{}}; }}"]
        if member_data:
            m_args = ", ".join([f"{t} _{n}" for t, n in member_data])
            m_inits = ", ".join([f"_{n}" for _, n in member_data])
            lambdas.append(f"[]({m_args}) {{ return {struct_name}{{{m_inits}}}; }}")

            lua_args = [f"arg{i}: {translate_type(t)}" for i, (t, _) in enumerate(member_data)]
            lua_out.append(f"---@overload fun({', '.join(lua_args)}): {struct_name}")

        constructor_code = f"sol::call_constructor, sol::factories({', '.join(lambdas)})"

    # Inheritance in C++
    if base_classes:
        bases_str = f"sol::base_classes, sol::bases<{', '.join(base_classes)}>()"
        cpp_entries.insert(0, bases_str)

    lua_out.extend(lua_data)
    lua_out.append(f"{struct_name} = {{}}\n")

    out_file.append(f'    lua.new_usertype<{struct_name}>("{struct_name}",')
    out_file.append(f'        {constructor_code},')
    if cpp_entries:
        out_file.append(",\n".join([f'        {e}' for e in cpp_entries]))

    out_file[-1] = out_file[-1].rstrip().rstrip(',')
    out_file.append("    );")


def parse_global_variable(node):
    var_name = sanitize_name(node.spelling)
    if not var_name or var_name in VARIABLE_BLACKLIST:
        return

    lua_type = translate_type(node.type.spelling)
    raw_cpp_type = node.type.spelling

    if node.type.is_const_qualified():
        out_file.append(f'    lua["{var_name}"] = {var_name};')
    else:
        out_file.append(f'    lua["{var_name}"] = sol::property(')
        out_file.append(f'        []() {{ return {var_name}; }},')
        out_file.append(f'        []({raw_cpp_type} v) {{ {var_name} = v; }}')
        out_file.append(f'    );')

    lua_out.append(f"---@type {lua_type}")
    lua_out.append(f"{var_name} = nil\n")


def parse_function(node):
    func_name = sanitize_name(node.spelling)
    if not func_name or func_name.startswith("operator") or func_name in FUNCTION_BLACKLIST:
        return

    # Check if the function is variadic (has '...')
    is_variadic = node.type.is_function_variadic()

    args = list(node.get_arguments())
    for i, p in enumerate(args):
        t_str = p.type.spelling
        if "&" in t_str and "const" not in t_str:
            return

    params_lua = []
    params_names = []
    for p in args:
        p_name = sanitize_name(p.spelling or f"arg{len(params_names)}")
        p_type = translate_type(p.type.spelling)
        params_lua.append(f"---@param {p_name} {p_type}")
        params_names.append(p_name)

    ret_type = clean_type(node.result_type.spelling)

    if is_variadic:
        arg_defs = ", ".join(
            [f"{clean_type(p.type.spelling)} {sanitize_name(p.spelling)}" for p in args]
        )
        arg_pass = ", ".join([sanitize_name(p.spelling) for p in args])

        # Generic fallback: just pass the known arguments, drop the '...'
        binding_target = f'[]({arg_defs}) {{ return {func_name}({arg_pass}); }}'
    else:
        arg_types = ", ".join([clean_type(p.type.spelling) for p in args])
        binding_target = f"static_cast<{ret_type}(*)({arg_types})>({func_name})"

    out_file.append(f'    lua.set_function("{func_name}", {binding_target});')

    if params_lua:
        lua_out.append("\n".join(params_lua))
    lua_out.append(f"---@return {translate_type(ret_type)}")
    lua_out.append(f"function {func_name}({', '.join(params_names)}) end\n")


def parse_node(node):
    print(f"Parsing node {node.spelling}")
    if (
        node.kind == clang.cindex.CursorKind.STRUCT_DECL
        or node.kind == clang.cindex.CursorKind.CLASS_DECL
    ):
        parse_struct(node)
    elif node.kind == clang.cindex.CursorKind.VAR_DECL:
        parse_global_variable(node)
    elif node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
        parse_function(node)
    elif node.kind == clang.cindex.CursorKind.LINKAGE_SPEC:
        for child in node.get_children():
            parse_node(child)


# def generate_bindings():
#     global out_file

#     all_headers = get_headers()
#     abs_all_headers = {os.path.abspath(f) for f in all_headers}

#     master_header_content = "\n".join([f'#include "{h}"' for h in all_headers])

#     args = [f"-I{dir}" for dir in INCLUDE_PATH]

#     # Add system headers to the clang include path
#     try:
#         resource_dir = subprocess.check_output(
#             ["clang", "-print-resource-dir"], encoding='utf-8'
#         ).strip()
#         args.append(f"-I{os.path.join(resource_dir, 'include')}")

#     except Exception as e:
#         print(f"Warning: Could not auto-detect system headers: {e}")

#     print("Parsing headers...")
#     index = clang.cindex.Index.create()
#     tu = index.parse(
#         "master_header.hpp", args, unsaved_files=[("master_header.hpp", master_header_content)]
#     )

#     if tu.cursor is None:
#         print("Error: Failed to parse header files for Lua bindings")
#         return

#     for diag in tu.diagnostics:
#         if diag.severity >= clang.cindex.Diagnostic.Error:
#             print(f"WARNING: {diag.spelling}")
#             print(f"  Location: {diag.location}")

#     node_counter = 0
#     for node in tu.cursor.get_children():
#         if not node.location.file or not node.location.file.name in abs_all_headers:
#             continue

#         header_include_path = get_include_path(str(node.location.file))
#         if not header_include_path in hpp_outs:
#             file_prefix = "Bindings/" + header_include_path
#             Path("tools/" + file_prefix).parent.mkdir(parents=True, exist_ok=True)
#             hpp_path = file_prefix + ".hpp"

#             function_name = f"CreateBinding{node_counter}"
#             node_counter += 1

#             hpp_outs[header_include_path] = ["#include <sol/forward.hpp>", "", f"void {function_name}(sol::state& lua);"]

#             cpp_outs[header_include_path] = [
#                 "#include \"Includes.hpp\"",
#                 f"#include \"{hpp_path}\"",
#                 f"#include \"{header_include_path}\"",
#                 "",
#                 f"void {function_name}(sol::state& lua)",
#                 "{",
#             ]

#             bindings_out.insert(0, f"#include \"{hpp_path}\"")
#             bindings_out.append(f"    {function_name}(lua);")

#         out_file = cpp_outs[header_include_path]

#         parse_node(node)


def generate_bindings(header_path: str):
    args = [f"-I{dir}" for dir in INCLUDE_PATH]

    # Add system headers to the clang include path
    try:
        resource_dir = subprocess.check_output(
            ["clang", "-print-resource-dir"], encoding='utf-8'
        ).strip()
        args.append(f"-I{os.path.join(resource_dir, 'include')}")

    except Exception as e:
        print(f"Warning: Could not auto-detect system headers: {e}")

    index = clang.cindex.Index.create()
    tu = index.parse(header_path, args)

    if tu.cursor is None:
        print("Error: Failed to parse header files for Lua bindings")
        return

    for diag in tu.diagnostics:
        if diag.severity >= clang.cindex.Diagnostic.Error:
            print(f"WARNING: {diag.spelling}")
            print(f"  Location: {diag.location}")

    for node in tu.cursor.get_children():
        if not node.location.file or not os.path.samefile(node.location.file.name, header_path):
            continue

        parse_node(node)


if __name__ == "__main__":
    hpp_out: list[str] = []
    cpp_out: list[str] = []
    hpp_outs: dict[str, list[str]] = {}
    cpp_outs: dict[str, list[str]] = {}
    bindings_out: list[str] = [
        "#include <sol/state.hpp>",
        "",
        "inline void CreateLuaBindings(sol::state& lua)",
        "{",
    ]
    lua_out: list[str] = ["---@meta\n"]
    out_file: list[str] = []

    for i, header in enumerate(get_headers()):
        print("Parsing file ", get_include_path(header))

        header_include_path = get_include_path(header)
        file_prefix = "Bindings/" + header_include_path
        Path("tools/" + file_prefix).parent.mkdir(parents=True, exist_ok=True)
        hpp_path = file_prefix + ".hpp"

        function_name = f"CreateBinding{i}"

        hpp_out = ["#include <sol/forward.hpp>", "", f"void {function_name}(sol::state& lua);"]

        cpp_out = [
            "#include \"Includes.hpp\"",
            f"#include \"{hpp_path}\"",
            f"#include \"{header_include_path}\"",
            "",
            f"void {function_name}(sol::state& lua)",
            "{",
        ]
        out_file = cpp_out

        bindings_out.insert(0, f"#include \"{hpp_path}\"")
        bindings_out.append(f"    {function_name}(lua);")

        generate_bindings(header)

        cpp_out.append("}")

        with open(ROOT_PATH / f"tools/{hpp_path}", "w") as f:
            f.write("\n".join(hpp_out))
        with open(ROOT_PATH / f"tools/{file_prefix}.cpp", "w") as f:
            f.write("\n".join(cpp_out))

    # generate_bindings()

    # for key, value in hpp_outs.items():
    #     with open(ROOT_PATH / f"tools/Bindings/{key}.hpp", "w") as f:
    #         f.write("\n".join(value))
    # for key, value in cpp_outs.items():
    #     value.append("}")
    #     with open(ROOT_PATH / f"tools/Bindings/{key}.cpp", "w") as f:
    #         f.write("\n".join(value))

    bindings_out.append("}")

    with open(ROOT_PATH / "tools/Bindings.hpp", "w") as f:
        f.write("\n".join(bindings_out))
    with open(ROOT_PATH / "tools/colsimapi.lua", "w") as f:
        f.write("\n".join(lua_out))
