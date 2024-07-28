workspace "AegisSDK"
   configurations { "Debug", "Release" }

   project "ACSG"
      kind "ConsoleApp"
      language "C"
      targetdir "bin/%{cfg.buildcfg}"

      includedirs { ".", "../qcommon" }
      files { "**.h", "**.c", "../qcommon/**.h", "../qcommon/**.c" }

      filter "configurations:Debug"
         defines { "DEBUG" }
         symbols "On"

      filter "configurations:Release"
         defines { "NDEBUG" }
         optimize "On"

         debugargs { "-test" }

print("Files included in project:")
for _, f in ipairs(os.matchfiles("**.c")) do print(f) end
for _, f in ipairs(os.matchfiles("**.h")) do print(f) end