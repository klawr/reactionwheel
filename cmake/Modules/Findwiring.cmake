
find_path(WIRING_INCLUDE_DIR wiringPi.h)
find_library(WIRING_LIBRARY wiringPi)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wiring
    REQUIRED_VARS
        WIRING_LIBRARY
	WIRING_INCLUDE_DIR
)

