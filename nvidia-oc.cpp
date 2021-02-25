#include <exception>
#include <iostream>
#include <memory>
#include <regex>
#include <string>

#include <X11/Xlib.h>
#include <NVCtrl/NVCtrl.h>
#include <NVCtrl/NVCtrlLib.h>

static int getGpuMaxPerfMode(Display *dpy, int gpuId)
{
    char *perfModesBuf = nullptr;
    if (!XNVCTRLQueryTargetStringAttribute(dpy, NV_CTRL_TARGET_TYPE_GPU, gpuId, 0,
                                           NV_CTRL_STRING_PERFORMANCE_MODES, &perfModesBuf)) {
        throw std::runtime_error("Failed to query perf mode settings for GPU " + std::to_string(gpuId));
    }
    std::string perfModes(perfModesBuf);
    free(perfModesBuf);

    int maxPerfMode = -1;
    std::smatch m;
    while (std::regex_search(perfModes, m, std::regex("perf=(\\d+)"))) {
        int perf = std::stoi(m[1]);
        if (perf > maxPerfMode)
            maxPerfMode = perf;
        perfModes = m.suffix();
    }

    if (maxPerfMode < 0) {
        throw std::runtime_error("Failed to parse perf mode settings for GPU " + std::to_string(gpuId));
    }
    return maxPerfMode;
}

int nvidiaOC(int coreClkOffset, int memClkOffset)
{
    Display *dpy = XOpenDisplay(nullptr);
    if (!dpy) {
        throw std::runtime_error("Cannot open X display");
    }
    std::shared_ptr<Display> _(dpy, [](Display *dpy){ XCloseDisplay(dpy); });

    if (!XNVCTRLQueryExtension(dpy, nullptr, nullptr)) {
        throw std::runtime_error("NV-CONTROL X extension not found");
    }

    int gpus = -1;
    XNVCTRLQueryTargetCount(dpy, NV_CTRL_TARGET_TYPE_GPU, &gpus);
    std::cout << "Found " << gpus << " GPUs" << std::endl;

    for (int i = 0; i < gpus; ++i) {
        int maxPerfMode = getGpuMaxPerfMode(dpy, i);

#define SET_ATTRIBUTE(attr, val, dm) do {                                                                               \
            if (!XNVCTRLSetTargetAttributeAndGetStatus(dpy, NV_CTRL_TARGET_TYPE_GPU, i, (dm), (attr), (val))) {         \
                throw std::runtime_error("Failed to set attribute " #attr " to value " #val + std::to_string(i));       \
            }                                                                                                           \
        } while(0)

        // Force maximum performance mode
        SET_ATTRIBUTE(NV_CTRL_GPU_POWER_MIZER_MODE, NV_CTRL_GPU_POWER_MIZER_MODE_PREFER_MAXIMUM_PERFORMANCE, 0);
        // GPU core clock offset
        SET_ATTRIBUTE(NV_CTRL_GPU_NVCLOCK_OFFSET,           coreClkOffset,  maxPerfMode);
        // GPU memory clock offset
        SET_ATTRIBUTE(NV_CTRL_GPU_MEM_TRANSFER_RATE_OFFSET, memClkOffset,   maxPerfMode);

#undef SET_ATTRIBUTE
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << " CORE_CLOCK MEM_CLOCK" << std::endl;
        return 0;
    }

    int coreclk, memclk;
    try {
        coreclk = std::stoi(argv[1]);
        memclk = std::stoi(argv[2]);
    } catch (std::invalid_argument const &ex) {
        std::cerr << "Invalid argument" << std::endl;
        return 1;
    }

    std::cout << "Setting core and memory clocks offsets to " << coreclk << " and " << memclk << " MHz" << std::endl;
    try {
        nvidiaOC(coreclk, memclk);
    } catch (std::exception const &ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    std::cout << "Done!" << std::endl;
    return 0;
}
