//===- OpenCL.h --------------------------------------------------*- C++ -*-===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLD_OPENCL_H
#define LLD_OPENCL_H

#include "lld/Common/LLVM.h"

#include <OpenCL/cl.h>

namespace lld {
namespace elf {

class OpenCLContext {
private:
    cl_context context;
    cl_mem device_buffer;

public:
    OpenCLContext() {
        cl_int err;
        cl_device_id device_id[2];
        cl_uint nDevices;
        cl_ulong max_mem_allocation;

        err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 2, device_id, &nDevices);
        assert (err == CL_SUCCESS && "Could not obtain OpenCL device.");

        this->context = clCreateContext(0, 1, &device_id[1], NULL, NULL, &err);
        assert (context && err == CL_SUCCESS && "Could not create OpenCL context");

        err = clGetDeviceInfo(device_id[1], CL_DEVICE_MAX_MEM_ALLOC_SIZE,
                        sizeof(cl_ulong), &max_mem_allocation, NULL);
        assert (err == CL_SUCCESS && "Could not get max kernel memory allocation size");

        device_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                                max_mem_allocation, nullptr, &err);
        assert (err == CL_SUCCESS && "Could not allocate kernel buffer");

        printf("Now we're cooking with gas!\n");
    }

    ~OpenCLContext() {
        clReleaseMemObject(this->device_buffer);
        clReleaseContext(this->context);
        printf("Turning off the gas...\n");
    }
};

extern OpenCLContext *CLContext;

} // namespace elf
} // namespace lld

#endif
