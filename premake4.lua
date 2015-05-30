#!lua

solution "Speccy"
	configurations { "Debug", "Release" }

	project "Speccy"
		kind "ConsoleApp"
		language "C++"
		files { "**.h", "**.cpp" }
		includedirs { "/usr/local/include/SDL2/" }
		libdirs { "/usr/local/lib/" }
		links { "SDL2" }

		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines {}
			flags { "Symbols", "Optimize" }


