#include "svd.h"
#include <complex>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#ifndef lapack_complex_float
    #define lapack_complex_float std::complex<float>
#endif
#ifndef lapack_complex_double
    #define lapack_complex_double std::complex<double>
#endif

// complex must be included before lapacke!
#if defined(MKL_AVAILABLE)
    #include <mkl_lapacke.h>
#elif defined(OPENBLAS_AVAILABLE)
    #include <openblas/lapacke.h>
#else
    #include <lapacke.h>
#endif

namespace svd {

    template<typename Scalar>
    std::tuple<MatrixType<Scalar>, VectorType<Scalar>, MatrixType<Scalar>> gesvd(const Scalar *A_ptr, long rows, long cols) {
        // Sanity checks
        if(rows <= 0) throw std::logic_error("SVD error: rows() <= 0");
        if(cols <= 0) throw std::logic_error("SVD error: cols() <= 0");

        // Setup useful sizes
        int rowsA = static_cast<int>(rows);
        int colsA = static_cast<int>(cols);
        int sizeS = std::min(rowsA, colsA);

        // Make a copy of A because it is destroyed in gesvd
        MatrixType<Scalar> A = Eigen::Map<const MatrixType<Scalar>>(A_ptr, rows, cols);

        // Initialize result containers
        MatrixType<Scalar> U;
        VectorType<double> S;
        MatrixType<Scalar> VT;

        int info   = 0;
        int rowsU  = rowsA;
        int colsU  = std::min(rowsA, colsA);
        int rowsVT = std::min(rowsA, colsA);
        int colsVT = colsA;
        int lda    = rowsA;
        int ldu    = rowsU;
        int ldvt   = rowsVT;

        int         mx = std::max(rowsA, colsA);
        int         mn = std::min(rowsA, colsA);
        std::string errmsg;
        if constexpr(std::is_same<Scalar, double>::value) {
            U.resize(rowsU, colsU);
            S.resize(sizeS);
            VT.resize(rowsVT, colsVT);
            std::vector<real> rwork(1ul);

            info = LAPACKE_dgesvd_work(LAPACK_COL_MAJOR, 'S', 'S', rowsA, colsA, A.data(), lda, S.data(), U.data(), ldu, VT.data(), ldvt, rwork.data(), -1);
            if(info != 0) std::runtime_error(fmt::format("Lapacke SVD dgesvd error: parameter {} is invalid", info));

            // Get the optimal value
            int lrwork = static_cast<int>(rwork[0]);
            rwork.resize(static_cast<size_t>(lrwork));

            info = LAPACKE_dgesvd_work(LAPACK_COL_MAJOR, 'S', 'S', rowsA, colsA, A.data(), lda, S.data(), U.data(), ldu, VT.data(), ldvt, rwork.data(), lrwork);
            if(info < 0) throw std::runtime_error(fmt::format("Lapacke SVD dgesvd error: parameter {} is invalid", info));
            if(info > 0) throw std::runtime_error(fmt::format("Lapacke SVD dgesvd error: could not converge: info {}", info));
        } else if constexpr(std::is_same<Scalar, std::complex<double>>::value) {
            U.resize(rowsU, colsU);
            S.resize(sizeS);
            VT.resize(rowsVT, colsVT);

            int               lcwork = std::max(1, 2 * mn + mx);
            int               lrwork = std::max(1, 5 * mn);
            std::vector<cplx> cwork(static_cast<size_t>(lcwork));
            std::vector<real> rwork(static_cast<size_t>(lrwork));

            info = LAPACKE_zgesvd_work(LAPACK_COL_MAJOR, 'S', 'S', rowsA, colsA, A.data(), lda, S.data(), U.data(), ldu, VT.data(), ldvt, cwork.data(), -1,
                                       rwork.data());
            if(info != 0) std::runtime_error(fmt::format("Lapacke SVD zgesvd error: parameter {} is invalid", info));

            lcwork = static_cast<int>(std::real(cwork[0]));
            cwork.resize(static_cast<size_t>(lcwork));
            info = LAPACKE_zgesvd_work(LAPACK_COL_MAJOR, 'S', 'S', rowsA, colsA, A.data(), lda, S.data(), U.data(), ldu, VT.data(), ldvt, cwork.data(), lcwork,
                                       rwork.data());
            if(info < 0) throw std::runtime_error(fmt::format("Lapacke SVD zgesvd error: parameter {} is invalid", info));
            if(info > 0) throw std::runtime_error(fmt::format("Lapacke SVD zgesvd error: could not converge: info {}", info));
        }

        auto rank = S.nonZeros();

        // Do the truncation
        U  = U.leftCols(rank).eval();
        S  = S.head(rank).eval(); // Not all calls to do_svd need normalized S, so we do not normalize here!
        VT = VT.topRows(rank).eval();

        // Sanity checks
        if(not U.allFinite()) throw std::runtime_error("U has inf's or nan's");
        if(not VT.allFinite()) throw std::runtime_error("VT has inf's or nan's");
        if(not S.allFinite()) throw std::runtime_error("S has inf's or nan's");
        if(not(S.array() >= 0).all()) throw std::runtime_error("S is not positive");
        return std::make_tuple(U, S, VT);
    }

    template std::tuple<MatrixType<real>, VectorType<real>, MatrixType<real>> gesvd(const real *, long, long);
    template std::tuple<MatrixType<cplx>, VectorType<cplx>, MatrixType<cplx>> gesvd(const cplx *, long, long);
}
