# Mini-project to debug the SVD routine in OpenBLAS
This project tries to reproduce a segmentation fault in OpenBLAS 0.3.20 and 0.3.21 implementations of `?gesvd` and `?gesdd`.


## Background
I have been getting segmentation faults randomly for certain matrices in the routines `?gesvd` and `?gesdd`.

This is the relevant part of the stack trace from my original project:

```
#9    Source "/home/cluster_users/x_aceituno/fLBIT/source/math/svd/svd_lapacke.cpp", line 273, in do_svd_lapacke<std::complex<double> >
        270:                     if(info != 0) break;
        271:                     lcwork = static_cast<int>(std::real(cwork[0]));
        272:                     cwork.resize(static_cast<size_t>(lcwork));
      > 273:                     info = LAPACKE_zgesvd_work(LAPACK_COL_MAJOR, 'S', 'S', rowsA, colsA, A.data(), lda, S.data(), U.data(), ldu, VT.data(), ldvt, cwork.data(),
        274:                                                lcwork, rwork.data());
        275:                     break;
        276:                 }
#8    Object "/opt/easybuild/software/OpenBLAS/0.3.21-GCC-12.2.0/lib/libopenblas_zenp-r0.3.21.so", at 0x7ff9925ee1de, in LAPACKE_zgesvd_work
#7    Object "/opt/easybuild/software/OpenBLAS/0.3.21-GCC-12.2.0/lib/libopenblas_zenp-r0.3.21.so", at 0x7ff99226ef36, in zgesvd_
#6    Object "/opt/easybuild/software/OpenBLAS/0.3.21-GCC-12.2.0/lib/libopenblas_zenp-r0.3.21.so", at 0x7ff9923a32f5, in zungbr_
#5    Object "/opt/easybuild/software/OpenBLAS/0.3.21-GCC-12.2.0/lib/libopenblas_zenp-r0.3.21.so", at 0x7ff9923a46ce, in zunglq_
#4    Object "/opt/easybuild/software/OpenBLAS/0.3.21-GCC-12.2.0/lib/libopenblas_zenp-r0.3.21.so", at 0x7ff9923a3f8c, in zungl2_
#3    Object "/opt/easybuild/software/OpenBLAS/0.3.21-GCC-12.2.0/lib/libopenblas_zenp-r0.3.21.so", at 0x7ff992326460, in zlarf_
#2    Object "/opt/easybuild/software/OpenBLAS/0.3.21-GCC-12.2.0/lib/libopenblas_zenp-r0.3.21.so", at 0x7ff991a4b5b2, in zgemv_
#1    Object "/opt/easybuild/software/OpenBLAS/0.3.21-GCC-12.2.0/lib/libopenblas_zenp-r0.3.21.so", at 0x7ff991d53484, in zgemv_n
#0    Object "/usr/lib/x86_64-linux-gnu/libc.so.6", at 0x7ff99144251f, in 
Exit SIGSEGV: 11
```

In all cases, the stack trace points to zgemv_n in the inner-most stack frame.


## Minimum requirements
- CMake 3.24
- BLAS: OpenBLAS (with lapack/lapacke) or Intel MKL
- conan package manager


## Build
Use the `CMakePresets.json`:

```shell
$ cmake --list-presets
Available configure presets:

  "release-native-mkl"       - Release|conan|march=native|mkl
  "release-native-openblas"  - Release|conan|march=native|openblas
  "release-haswell-mkl"      - Release|conan|march=haswell|mkl
  "release-haswell-openblas" - Release|conan|march=haswell|openblas
  "debug-mkl"                - Debug|conan|mkl|shared|asan
  "debug-openblas"           - Debug|conan|mkl|shared|asan

```
Choose one of the presets. Dependencies are automatically installed by conan:

```
cmake  --preset release-haswell-openblas

    < ... clipped a lot of CMake output ... >

cmake --build  --preset release-haswell-openblas

    < ... build process ... >
```
