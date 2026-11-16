#pragma once

#include "implot3d.h"

namespace ImPlot3D{
    IMPLOT3D_TMP
    void PlotLine(const char* label_id, const T* xs, const T* ys, const T* zs, int count, int stride_x, int stride_y, int stride_z, const ImPlot3DSpec& spec = ImPlot3DSpec());
}