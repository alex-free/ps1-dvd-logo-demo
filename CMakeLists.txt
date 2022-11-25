# PSn00bSDK PS1 DVD Logo Demo CMake script
# (C) 2022 Alex Free 3-BSD (see license.txt)

cmake_minimum_required(VERSION 3.21)

project(
	ps1-dvd-logo-demo-demo
	LANGUAGES    C ASM
	VERSION      1.0.0
	DESCRIPTION  "Bounces a DVD logo around with some nice visual effects and gimmicks."
	HOMEPAGE_URL "https://alex-free.github.io/ps1-dvd-logo-demo"
)

file(GLOB _sources *.c)
psn00bsdk_add_executable(dvd-logo-demo GPREL ${_sources})
psn00bsdk_add_cd_image(dvd-logo-demo_iso dvd-logo-demo iso.xml DEPENDS dvd-logo-demo)
psn00bsdk_target_incbin(dvd-logo-demo PRIVATE dvd_logo dvd.tim)

install(FILES ${PROJECT_BINARY_DIR}/dvd-logo-demo.exe TYPE BIN)
