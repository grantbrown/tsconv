#ifndef TSCONV_IMAGEDEF
#define TSCONV_IMAGEDEF

#include<Rcpp.h>
#include<Eigen/Core>

using namespace Rcpp;
namespace tsconv
{

class Image
{
    public:
        Image(SEXP data);
        void rotate(double theta);
        Rcpp::NumericMatrix getImage();
        ~Image();
    private:
        Eigen::MatrixXi* imageData;

};

}
#endif

