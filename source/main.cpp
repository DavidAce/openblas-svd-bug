#include "stacktrace.h"
#include "svd.h"
#include <csignal>
#include <h5pp/h5pp.h>
#include <spdlog/spdlog.h>

void merge() {
    auto log = spdlog::stdout_color_mt("svd-merge", spdlog::color_mode::always);
    auto h5tgt = h5pp::File(fmt::format("{}/svd-fail.h5", OUTDIR), h5pp::FileAccess::REPLACE);
    h5tgt.setCompressionLevel(6);
    size_t counter = 0;
    for (const auto &path: h5pp::fs::recursive_directory_iterator(
            "/mnt/WDB-AN1500/mbl_transition/lbit77-debug/output")) {
        if (path.is_regular_file() and path.path().filename().string().find("svd-save") != std::string::npos) {
            log->info("path: {}", path.path().string());
            auto h5src = h5pp::File(path.path().string(), h5pp::FileAccess::READONLY);
            for (const auto &group: h5src.findGroups("svd-fail", "/", -1, 1)) {
                auto seed = h5src.readAttribute<long>(group, "seed");
                auto A = h5src.readDataset<svd::MatrixType<svd::cplx>>(fmt::format("{}/A_cplx", group));
                auto routine = h5src.readAttribute<std::string>(group, "svd_rtn");
                h5tgt.writeDataset(A, fmt::format("{}-{}/A_cplx", group, seed), H5D_layout_t::H5D_CHUNKED);
                h5tgt.writeAttribute(routine, fmt::format("{}-{}", group, seed), "svd_rtn");
//                h5tgt.copyLinkFromLocation(fmt::format("{}-{}", group, seed), h5src.openFileHandle(), group);
                counter++;
            }
        }
        if (counter >= 20) break;
    }
    exit(0);
}


int main() {
//    merge();
    debug::register_callbacks();
    auto log = spdlog::stdout_color_mt("svd-fail", spdlog::color_mode::always);
    auto h5src = h5pp::File(fmt::format("{}/svd-fail.h5", OUTDIR), h5pp::FileAccess::READONLY);
    for (const auto &h5grp: h5src.findGroups("svd-fail", "/", -1, 1)) {
        auto dset = fmt::format("{}/A_cplx", h5grp);
        log->info("dset: {} | dims {}", dset, h5src.getDatasetDimensions(dset));
        auto routine = h5src.readAttribute<std::string>(h5grp, "svd_rtn");
        auto A = h5src.readDataset<svd::MatrixType<svd::cplx>>(dset);
        if (routine == "gesvd") { auto [U, S, VT] = svd::gesvd(A.data(), A.rows(), A.cols()); }
        if (routine == "gesdd") { auto [U, S, VT] = svd::gesdd(A.data(), A.rows(), A.cols()); }
    }
}