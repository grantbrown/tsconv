#define __CL_ENABLE_EXCEPTIONS
#include <Rcpp.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <OCLProvider.hpp>
#include <CL/cl.hpp>

using namespace tsconv;


PlatformContainer::PlatformContainer(cl::Platform *inPlatform)
{
    //Store platform
    platform = new cl::Platform*; 
    *platform = inPlatform;

    //Store all platform devices to create context
    std::vector<cl::Device>* allDevices = new std::vector<cl::Device>();
    (*platform) -> getDevices(CL_DEVICE_TYPE_ALL, allDevices);
    context = new cl::Context(*allDevices);

    // Process and store devices by type
    devices = new std::vector<DeviceContainer*>();
    deviceTypes = new std::vector<std::string>();
    deviceNames = new std::vector<std::string>();
    doublePrecision = new std::vector<cl_uint>();

    std::vector<cl::Device> *cpuDevices = new std::vector<cl::Device>; 
    std::vector<cl::Device> *gpuDevices = new std::vector<cl::Device>; 

    try
    {
        (*platform) -> getDevices(CL_DEVICE_TYPE_CPU, cpuDevices);
    }
    catch(cl::Error e)
    {
       if (e.err() == -1) 
       {
           Rcpp::Rcout << "...no OpenCL CPU devices found\n";
       }
       else
       {
           Rcpp::Rcerr << "Error querying CPU devices.\n";
           Rcpp::Rcerr << e.what() << "\n";
           throw(e.err());
       }

    }

    try
    {
        (*platform) -> getDevices(CL_DEVICE_TYPE_GPU, gpuDevices);
    }
    catch(cl::Error e)
    {
       if (e.err() == -1) 
       {
           Rcpp::Rcout << "...no OpenCL GPU devices found\n";
       }
       else
       {
           Rcpp::Rcerr << "Error querying GPU devices.\n";
           Rcpp::Rcerr << e.what() << "\n";
           throw(e.err());
       }
    }


    std::string clExt;
    cl_uint i;
    DeviceContainer* newDevice;
    for (i = 0; i < cpuDevices -> size(); i++)
    {  
        Rcpp::Rcout << "Adding CPU Device: ";
        newDevice = new DeviceContainer(&(*cpuDevices)[i], context);
        devices -> push_back(newDevice);
        clExt = (*(newDevice -> device)) -> getInfo<CL_DEVICE_EXTENSIONS>();
        doublePrecision -> push_back((clExt.find("cl_khr_fp64") != std::string::npos));
        deviceTypes -> push_back("CPU");
        deviceNames -> push_back((*(newDevice -> device)) -> getInfo<CL_DEVICE_NAME>());
        Rcpp::Rcout << (*deviceNames)[deviceNames -> size() - 1] << "\n";

    }
    for (i = 0; i < gpuDevices -> size(); i++)
    {   
        Rcpp::Rcout << "Adding GPU Device: ";
        newDevice = new DeviceContainer(&((*gpuDevices)[i]), context);
        devices -> push_back(newDevice);
        clExt = (*(newDevice -> device)) -> getInfo<CL_DEVICE_EXTENSIONS>();
        doublePrecision -> push_back((clExt.find("cl_khr_fp64") != std::string::npos));
        deviceTypes -> push_back("GPU");
        deviceNames -> push_back((*(newDevice -> device)) -> getInfo<CL_DEVICE_NAME>());
        Rcpp::Rcout << (*deviceNames)[deviceNames -> size() - 1] << "\n";
    }
}

PlatformContainer::~PlatformContainer()
{
    while (devices -> size() != 0){delete (devices -> back()); devices -> pop_back();}
    delete devices;
    delete deviceTypes;
    while (deviceNames -> size() != 0){deviceNames -> pop_back();}
    delete deviceNames;
    while (doublePrecision -> size() != 0){doublePrecision -> pop_back();}
    delete doublePrecision;
    delete context;
    delete platform;
}


