include_directories("tests")
LINK_LIBRARIES(CppUTest)
if (CUDA_FOUND)
    message("ADDING CUDA EXECUTABLE")
    set( CUDA_ARCHITECTURE_MINIMUM "5.0" CACHE STRING "Minimum required CUDA compute capability" )
    SET(CUDA_VERBOSE_BUILD ON)
    set(CUDA_COMPUTE_CAPABILITY "50")
    list(APPEND CUDA_NVCC_FLAGS "-DVERBOSE;-lCppUTest;-G;-g;-gencode arch=compute_50,code=sm_50")
    CUDA_COMPILE(MAIN_O  main.cu)
    cuda_add_executable(libnd4jtests ${MAIN_O})

else()
    LINK_LIBRARIES(CppUTest)
    message("CUDA not found")
    set(SOURCE_FILES
            src/array/shape.cu
            main.cu src/array/templatemath.cu)
    add_executable(nativeoperations ${SOURCE_FILES} src/array/templatemath.cu src/array/templatemath.h)
endif()