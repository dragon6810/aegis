workspace "AegisBuildTools"
   configurations { "Debug", "Release" }

project "Awol"
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   files { "**.h", "**.c" }
   files { "../common/**.h", "../common/**.c" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   debugdir (_MAIN_SCRIPT_DIR)