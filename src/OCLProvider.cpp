#define __CL_ENABLE_EXCEPTIONS
#include <Rcpp.h>
#include <math.h>
#include <cstring>
#include <vector>
#include <OCLProvider.hpp>
#include <CL/cl.hpp>

using namespace tsconv;


OCLProvider::OCLProvider(int _verbose)
{
    verbose = _verbose;
    if (verbose) Rcpp::Rcout << "Setting up OpenCL Interface\n";
    try
    {
        // Allocate space for platforms and current config
        platforms = new std::vector<PlatformContainer*>();
        currentPlatform = new cl::Platform*;
        currentContext = new cl::Context*;
        currentDevice = new DeviceContainer*;


        // Allocate space for kernels
        /*
        p_se_kernel2 = new cl::Kernel();
        */


        // Build platforms, devices, contexts
        cl_uint i;
        std::vector<cl::Platform> *pformVec = new std::vector<cl::Platform>;
        cl::Platform::get(pformVec);

        PlatformContainer* newPlatform;
        for (i = 0; i < pformVec -> size(); i++) 
        {
            newPlatform = new PlatformContainer((&(*pformVec)[i]));
            platforms -> push_back(newPlatform);
        }


        // Initialize clBLAS library
        
        clblasStatus err = clblasSetup();
        if (err != CL_SUCCESS)
        {
            Rcpp::Rcout << "Error setting up clBLAS library: " << err << "\n";
            throw(-1);
        }

        // Flag for existence of current<item>s
        isSetup = new int; *isSetup = 0;
        // Dummy code to pick device 0,0
        setDevice(0,0);
    }
    catch(cl::Error e)
    {
        Rcpp::Rcout << "Problem getting platforms:\n";
        Rcpp::Rcout << e.what() << ": Error Code " << e.err() << "\n";
        throw(e);
    }
}

void OCLProvider::printSummary()
{
    unsigned int i,j;
    if (platforms -> size() == 0)
    {
        Rcpp::Rcout << "No OpenCL Platforms Detected\n";
        return;
    }
    for (i = 0; i < (platforms -> size()); i++)
    {
        Rcpp::Rcout << "Platform " << i << ": "  << (*(((*platforms)[i]) -> platform)) -> getInfo<CL_PLATFORM_NAME>() << "\n";
        Rcpp::Rcout << "  Devices: \n";
        for (j = 0; j < (((*(platforms))[i]) -> devices) -> size(); j++)
        {
            Rcpp::Rcout << "  " << j << ". " <<  (*(((*(((*(platforms))[i]) -> devices))[j]) -> device)) -> getInfo<CL_DEVICE_NAME>() << "\n";
            Rcpp::Rcout << (j < 10 ? " " : (j < 100 ? "  " : (j < 1000 ? "   " : "   "))) << "    " << "Type: " << 
                ((*(((*(platforms))[i]) -> deviceTypes))[j])  << "\n";
            Rcpp::Rcout << (j < 10 ? " " : (j < 100 ? "  " : (j < 1000 ? "   " : "   "))) << "    " << "Supports Double Precision: " << 
                ((*(((*(platforms))[i]) -> doublePrecision))[j])  << "\n";
            Rcpp::Rcout << (j < 10 ? " " : (j < 100 ? "  " : (j < 1000 ? "   " : "   "))) << "    " << 
                "Preferred double vector width: " << (*(((*(((*(platforms))[i]) -> devices))[j]) -> device)) -> 
                getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE>() << "\n";
            Rcpp::Rcout << (j < 10 ? " " : (j < 100 ? "  " : (j < 1000 ? "   " : "   "))) << "    " << 
                "Local Memory: " << (*(((*(((*(platforms))[i]) -> devices))[j]) -> device)) -> getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << "\n";
            Rcpp::Rcout << (j < 10 ? " " : (j < 100 ? "  " : (j < 1000 ? "   " : "   "))) << "    " << 
                "Max Compute Units: " << (*(((*(((*(platforms))[i]) -> devices))[j]) -> device)) -> getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << "\n"; 
        }
    }
}

OCLProvider::~OCLProvider()
{
    clblasTeardown();
    delete currentPlatform;
    delete currentContext;
    delete currentDevice;
    delete platforms;
    delete isSetup;
}

void OCLProvider::setDevice(int platformId, int deviceId)
{

    try{
        unsigned int pID, dID;
        if (platformId < 0 || deviceId < 0)
        {
            Rcpp::Rcerr << "Invalid Arguments\n";
        }
        pID = platformId;
        dID = deviceId;
        if ((*platforms).size() < pID)
        {
            Rcpp::Rcerr << "Invalid Platform.\n";
             throw(-1);      
        }
        if ((*((*platforms)[pID] -> devices)).size() <= dID)
        {
            Rcpp::Rcerr << "Invalid Device.\n";
            throw(-1);
        }

        *currentPlatform = (*((*platforms)[pID] -> platform));
        *currentContext = (*platforms)[pID] -> context;
        *currentDevice = ((*((*platforms)[pID] -> devices))[dID]);
        *isSetup = 1;
    }
    catch (int e)
    {
        Rcpp::Rcout << "Error\n";
    }
}

// TODO: accept std::string kernel directly, store as R data
cl::Kernel OCLProvider::buildProgramForKernel(std::string kernelString, DeviceContainer* device)
{
    int err = 1;
    std::vector<cl::Device> devices; devices.push_back(**(device -> device));
    std::string log;

    cl::Program::Sources source(1, std::make_pair(kernelString.c_str(), 
                                kernelString.length() + 1));

    cl::Program program(**currentContext, source);
    std::vector<cl::Kernel> kernels;

    try
    {
        err = program.build(devices);
        log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
        if (log.find("warning") != std::string::npos)
        {
            Rcpp::Rcout << "Warnings generated while building kernel.\n";
            Rcpp::Rcout << "CL_PROGRAM_BUILD_LOG: \n" << log << "\n";
        }
        program.createKernels(&kernels);
    }
    catch(cl::Error e)
    {
        Rcpp::Rcout << "CL Error in: " << e.what()<< "\n";
        Rcpp::Rcout << "CL Error: " << e.err()<< "\n";
        log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
        err = e.err();
    }

    if (err != 0)
    {
        Rcpp::Rcerr << "Error building OpenCL Kernel, code: " << err << "\n"; 
        Rcpp::Rcerr << "Build Log: \n" << log << "\n";
        Rcpp::Rcerr << "Kernel Source: \n" << kernelString.c_str() << "\n";
        throw(err);
    }
    return(kernels[0]);
}



