#!lua

solution "Speccy"
	configurations { "Debug", "Release" }

	project "Speccy"
		kind "ConsoleApp"
		language "C++"
		files { "**.h", "**.cpp" }
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines {}
			flags { "Symbols", "Optimize" }


