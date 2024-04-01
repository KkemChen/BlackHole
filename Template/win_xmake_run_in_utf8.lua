add_rules("mode.debug", "mode.release")

add_requires("vcpkg::opencv4",{alias="opencv"},{configs = {shared = true}})

set_encodings("utf-8") 
target("opencv_test")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("vcpkg::opencv4")
    before_run(function (target)
        print("Setting code page to UTF-8")
        os.exec("cmd /c chcp 65001")
    end)

