#ifndef OCL_PROVIDER_INC
#define OCL_PROVIDER_INC
#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <clBLAS.h>

namespace tsconv
{
    class DeviceContainer
    {
        public:
            DeviceContainer(cl::Device *inDevice, cl::Context *inContext);
            ~DeviceContainer();
            cl::Device** device;
            cl::CommandQueue * commandQueue;
    };

    class PlatformContainer
    {
        public:
            PlatformContainer(cl::Platform *inPlatform);
            ~PlatformContainer();
            cl::Platform **platform;
            cl::Context *context;
            std::vector<DeviceContainer*> *devices;
            std::vector<std::string> *deviceTypes; // CPU or GPU
            std::vector<std::string> *deviceNames;
            std::vector<cl_uint> *doublePrecision;
    };

    class OCLProvider
    {
        public:  
            //Methods 
            OCLProvider(int verbose);
            void setDevice(int platformId, int deviceId);
            void printSummary();
            ~OCLProvider();

            // Kernels
            /*
            cl::Kernel* test_kernel;
            cl::Kernel* R_Star_kernel;
            cl::Kernel* p_se_kernel1;
            cl::Kernel* p_se_kernel2;
            */

            // Queues
            cl::CommandQueue* cpuQueue;
            cl::CommandQueue* gpuQueue;
        private:
            int verbose;
            cl::Platform **currentPlatform;
            cl::Context **currentContext;
            DeviceContainer **currentDevice;
            int *isSetup;

            std::vector<PlatformContainer*> *platforms;
            cl::Kernel buildProgramForKernel(std::string kernelFile, 
                    DeviceContainer *device);
            std::vector<cl::Program> *programs;
    };
}

#endif

