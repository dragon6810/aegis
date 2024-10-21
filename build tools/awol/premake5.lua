workspace "AegisBuildTools"
   configurations { "Debug", "Release" }

project "Awol"
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   files { "**.h", "**.c", "../common/**.h", "../common/**.c" }
   includedirs { "../common" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   debugdir (_MAIN_SCRIPT_DIR)