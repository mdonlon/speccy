#!lua

z80_files = { "basic_opcodes.h", "cb_opcodes.h", "opcodes.h", "opcodes.cpp", "z80.h", "z80.cpp" }

solution "Speccy"
	configurations { "Debug", "Release" }

	project "Speccy"
		kind "ConsoleApp"
		language "C++"
		files { z80_files, "speccy.h", "speccy.cpp", "screen.h", "screen.cpp" }
		includedirs { "/usr/local/include/SDL2/" }
		libdirs { "/usr/local/lib/" }
		links { "SDL2" }

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }
			targetdir "debug/"

		configuration "Release"
			defines {}
			flags { "Symbols", "Optimize" }
			targetdir "release/"


	project "Zexall"
		kind "ConsoleApp"
		language "C++"
		files { z80_files, "zexall.cpp" }

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }
			targetdir "debug/"

		configuration "Release"
			defines {}
			flags { "Symbols", "Optimize" }
			targetdir "release/"



