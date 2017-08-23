
protobuf_generate_cpp(FILEPROTO_SRCS FILEPROTO_HDRS
	file.proto
)

add_executable(iot-proji
	main.cpp
	frontend.cpp
	thread_bus.cpp

	i2c.cpp
	mpu6050.cpp
	#stepper.cpp

	${FILEPROTO_SRCS}
	${FILEPROTO_HDRS}
)

target_link_libraries(iot-proji
	PRIVATE
		#${WIRING_LIBRARY}
		${CURSES_LIBRARIES}
		${Protobuf_LIBRARIES}

		Boost::boost
		cursespp
)
target_include_directories(iot-proji
	PRIVATE
		#${WIRING_INCLUDE_DIR}
		${CURSES_INCLUDE_DIRS}
		${Protobuf_INCLUDE_DIRS}
		${CMAKE_CURRENT_BINARY_DIR}
)
