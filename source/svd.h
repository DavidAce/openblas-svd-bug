#pragma once
#include <complex>
#include <Eigen/Core>

namespace svd {
    using real = double;
    using cplx = std::complex<double>;
    template<typename Scalar>
    using MatrixType = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
    template<typename Scalar>
    using VectorType = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;

    template<typename Scalar>
    std::tuple<MatrixType<Scalar>, VectorType<Scalar>, MatrixType<Scalar>> gesvd(const Scalar *A_ptr, long rows, long cols);
        template<typename Scalar>
    std::tuple<MatrixType<Scalar>, VectorType<Scalar>, MatrixType<Scalar>> gesdd(const Scalar *A_ptr, long rows, long cols);
}