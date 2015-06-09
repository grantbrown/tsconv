#include <Rcpp.h>
#include "Image.hpp"
#include "OCLProvider.hpp"
using namespace Rcpp;
using namespace tsconv;

Image::Image(SEXP data, SEXP rotKernel)
{
    Rcpp::NumericMatrix inData(data);
    std::string rotationKernel = Rcpp::as<std::string>(rotKernel);
    oclProvider = new OCLProvider(1);
    try
    {
        cl::Kernel compiledRotationKernel = oclProvider -> buildKernel(rotationKernel); 
    }
    catch (int e)
    {
        Rcpp::Rcerr << "Couldn't build kernel.\n";
    }
    imageData = new Eigen::MatrixXi(inData.nrow(), inData.ncol());
    int i, j;

    for (i = 0; i < inData.ncol(); i++)
    {
        for (j = 0; j < inData.nrow(); j++)
        {
            (*imageData)(j,i) = inData(j,i);
        }
    }
}

Rcpp::NumericMatrix Image::getImage()
{
    Rcpp::NumericMatrix outData(imageData -> rows(), imageData -> cols());
    int i, j;
    for (i = 0; i < outData.ncol(); i++)
    {
        for (j = 0; j < outData.nrow(); j++)
        {
            outData(j,i) = (*imageData)(j,i);
        }
    }
}

void Image::rotate(double theta)
{
    // Rotate Image
}

Image::~Image()
{
    delete imageData;
    delete oclProvider;
}

RCPP_MODULE(mod_Image)
{
    using namespace Rcpp;
    using namespace tsconv;
    class_<Image>( "Image" )
    .constructor<SEXP, SEXP>()
    .method("rotate", &Image::rotate)
    .property("data", &Image::getImage);
}
