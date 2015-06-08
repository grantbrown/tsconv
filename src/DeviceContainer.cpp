#define __CL_ENABLE_EXCEPTIONS
#include <Rcpp.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <OCLProvider.hpp>
#include <CL/cl.hpp>

using namespace tsconv;

DeviceContainer::DeviceContainer(cl::Device *inDevice, cl::Context *inContext)
{
    device = new cl::Device*;
    *device = inDevice; 
    commandQueue = new cl::CommandQueue(*inContext, *inDevice); 
}

DeviceContainer::~DeviceContainer()
{
    Rcpp::Rcout << "Deleting DeviceContainer.\n";
    delete commandQueue;
    delete device;
}


