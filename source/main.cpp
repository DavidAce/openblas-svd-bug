#include "stacktrace.h"
#include "svd.h"
#include <csignal>
#include <h5pp/h5pp.h>
#include <spdlog/spdlog.h>
int main() {
    debug::register_callbacks();
    auto log   = spdlog::stdout_color_mt("svd-fail", spdlog::color_mode::always);
    auto h5src = h5pp::File(fmt::format("{}/svd-fail.h5", OUTDIR), h5pp::FileAccess::READONLY);
    for(const auto &h5grp : h5src.findGroups("svd-fail", "/", -1, 1)) {
        auto dset = fmt::format("{}/A_cplx", h5grp);
        log->info("dset: {} | dims {}", dset, h5src.getDatasetDimensions(dset));
        auto routine = h5src.readAttribute<std::string>(h5grp, "svd_rtn");
        auto A       = h5src.readDataset<svd::MatrixType<svd::cplx>>(dset);
        if(routine == "gesvd") { auto [U, S, VT] = svd::gesvd(A.data(), A.rows(), A.cols()); }
        if(routine == "gesdd") { auto [U, S, VT] = svd::gesdd(A.data(), A.rows(), A.cols()); }
    }
}