//===- OpenCL.h --------------------------------------------------*- C++ -*-===//
//
//                             The LLVM Linker
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "OpenCL.h"

using namespace llvm;
using namespace lld;
using namespace lld::elf;

OpenCLContext::OpenCLContext() {
    printf("Now we're cooking with gas!\n");
}

OpenCLContext::~OpenCLContext() {
    printf("Turning off the gas...\n");
    if (t != nullptr) {
      delete t;
    }
}

void OpenCLContext::initKernel() {
  printf("Initializing kernel...\n");
  cl_int err;
  cl_uint nDevices;
  cl_device_id device_ids[2]; // My MBP has two GPUs - an Intel and an AMD Radeon.
  uint device = 1; // 0: Intel on-board, 1: Radeon

  err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 2, device_ids, &nDevices);
  assert(err == CL_SUCCESS);


  this->deviceID = device_ids[device];
  this->context = clCreateContext(0, 1, &this->deviceID, NULL, NULL, &err);
  assert(this->context && err == CL_SUCCESS);
}

void OpenCLContext::init() {
    t = new std::thread(&OpenCLContext::initKernel, this);
}

void OpenCLContext::ensureInit() {
    t->join();
}