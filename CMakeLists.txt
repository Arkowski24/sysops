cmake_minimum_required(VERSION 3.8)
project(sysops)

set(CMAKE_C_STANDARD 99)

add_executable(msgServer cw06/systemV/msg_service.h cw06/systemV/server/msg_server.c cw06/systemV/server/msg_server_services.c cw06/systemV/server/msg_server.h)
add_executable(msgClient cw06/systemV/msg_service.h cw06/systemV/client/msg_client.c cw06/systemV/client/msg_client.h)