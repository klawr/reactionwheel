add_executable(iot-proji
	main.cpp
)

target_link_libraries(iot-proji
	PRIVATE ${WIRING_LIBRARY}
)
target_include_directories(iot-proji
	PRIVATE ${WIRING_INCLUDE_DIR}
)
