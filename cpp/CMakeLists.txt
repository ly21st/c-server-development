cmake_minimum_required(VERSION 3.13)
project(cp)

set(CMAKE_CXX_STANDARD 14)

add_executable(merge merge.cpp)
add_executable(MyCircularDeque MyCircularDeque.cpp)
add_executable(trap trap.cpp)
add_executable(twoSum twoSum.cpp)
add_executable(longestCommonPrefix longestCommonPrefix.cpp)
add_executable(isPalindrome isPalindrome.cpp)
add_executable(gridIllumination gridIllumination.cpp)
add_executable(gridIllumination2 gridIllumination2.cpp)


add_executable(reactor lingshengxueyuan/chap5_3/reactor.c)
add_executable(mul_port_client_epoll lingshengxueyuan/chap5_3/mul_port_client_epoll.c)

add_executable(sync_http lingshengxueyuan/chap2_4/sync_http.c)
add_executable(async_http lingshengxueyuan/chap2_4/async_http.c)

add_executable(cfg lingshengxueyuan/HuLianWangYunPan/0voice-cloud-disk/common/cfg.c)

