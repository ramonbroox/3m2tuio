include_directories(.)

set(TUIO_SRCS
    ip/IpEndpointName.cpp
    TuioServer.cpp
    TuioClient.cpp
    osc/OscTypes.cpp
    osc/OscOutboundPacketStream.cpp
    osc/OscPrintReceivedElements.cpp
    osc/OscReceivedElements.cpp
    TuioTime.cpp
)
if(WIN32)
    set(TUIO_SRCS ${TUIO_SRCS}
        ip/win32/NetworkingUtils.cpp
        ip/win32/UdpSocket.cpp)
else(WIN32)
    set(TUIO_SRCS ${TUIO_SRCS}
        ip/posix/NetworkingUtils.cpp
        ip/posix/UdpSocket.cpp)
endif(WIN32)


add_library(TUIO STATIC ${TUIO_SRCS})

if(WIN32)
target_link_libraries(TUIO Ws2_32 Winmm)
endif(WIN32)

if(WIN32)
#install(TARGETS TUIO DESTINATION lib)
else(WIN32)
#install(TARGETS TUIO LIBRARY DESTINATION lib)
endif(WIN32)
