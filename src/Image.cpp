#include <Rcpp.h>
#include "Image.hpp"
using namespace Rcpp;
using namespace tsconv;

Image::Image(SEXP data)
{
    Rcpp::NumericMatrix inData(data);
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
}

RCPP_MODULE(mod_Image)
{
    using namespace Rcpp;
    using namespace tsconv;
    class_<Image>( "Image" )
    .constructor<SEXP>()
    .method("rotate", &Image::rotate)
    .property("data", &Image::getImage);
}
