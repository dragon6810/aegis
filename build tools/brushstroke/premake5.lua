workspace "AegisBuildTools"
   configurations { "Debug", "Release" }

project "Brushstroke"
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

   debugargs { "brushstroketest.map" }
   debugdir (_MAIN_SCRIPT_DIR)