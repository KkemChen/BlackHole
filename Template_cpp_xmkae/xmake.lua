set_project("demo", "this is a demo")       -- 设置项目名称和描述
set_xmakever("2.8.3")                       -- 设置xmake版本
set_version("2.0.0", {build = "%Y-%m-%d %H:%M"})
--add_rules("mode.debug", "mode.release","clean")     -- xmake f --verbose -m debug/release

-- 添加C++编译选项
set_symbols("debug")                      -- 默认构建模式 debug release
set_optimize("none")                      -- 优化级别  
set_languages("cxx14")                    -- 语言标准
--set_toolset("cxx", "g++")


if is_plat("windows") then
        add_cxxflags("/Zc:__cplusplus","/utf-8")
end

if is_plat("macosx","linux") then
        add_cxxflags("-ggdb","-g","-fPIC","-Wall","-Wextra",
                    "-Wno-unused-function","-Wno-unused-parameter","-Wno-unused-variable",
                    "-Wno-error=extra","-Wno-error=missing-field-initializers","-Wno-error=type-limits")
        add_syslinks("pthread","stdc++fs")
    end


--includes("3rdpart")

add_requires("spdlog",{configs = {header_only = false},system = false})

--add_includedirs("3rdpart")
set_targetdir("build")
set_installdir("dist-app")

target("demo")
    set_kind("binary")
    add_files("src/**.cpp")
    add_packages("spdlog")


    set_configdir("$(projectdir)/src")
    add_configfiles("src/Ver.h.in")

    -----install-----
    add_installfiles("$(buildir)/target:name()", {prefixdir = "bin"})
    --add_installfiles("src/**.h", {prefixdir = "include"})
    add_installfiles("*.so", {prefixdir = "lib"})


-------------------rule-----------------
rule("clean")
    on_clean(function (target)
        os.rm("build")
        os.rm(".xmake")
    end)
rule_end()


