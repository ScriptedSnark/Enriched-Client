if( PLATFORM_WINDOWS )
add_sources(
	CMakeLists.txt
	buffer.c
	buffer.h
	hde32.c
	hde32.h
	hde64.c
	hde64.h
	hook.c
	MinHook.h
	pstdint.h
	table32.h
	table64.h
	trampoline.c
	trampoline.h
)
endif()