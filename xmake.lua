add_requires("vulkan-loader", "glfw", "glm")
set_languages("c++17")

target("vulkan-learn")
	set_kind("binary")
	add_rules("utils.glsl2spv", {bin2c = true})
	add_files("shader/*")
	add_files("test.cpp")
	add_packages("vulkan-loader", "glfw", "glm")