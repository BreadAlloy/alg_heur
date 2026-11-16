#pragma once

namespace ImPlot3D{
IMPLOT3D_TMP void PlotLine(const char* label_id, const T* xs, const T* ys, const T* zs, int count, int stride_x, int stride_y, int stride_z, const ImPlot3DSpec& spec) {
    if (count < 2)
        return;
    GetterXYZ<IndexerIdx<T>, IndexerIdx<T>, IndexerIdx<T>> getter(IndexerIdx<T>(xs, count, spec.Offset, stride_x),
                                                                  IndexerIdx<T>(ys, count, spec.Offset, stride_y),
                                                                  IndexerIdx<T>(zs, count, spec.Offset, stride_z), count);
    return PlotLineEx(label_id, getter, spec);
}

#define INSTANTIATE_MACRO(T)                                                                                                                         \
    template IMPLOT3D_API void PlotLine<T>(const char* label_id, const T* xs, const T* ys, const T* zs, int count, int stride_x, int stride_y, int stride_z, const ImPlot3DSpec& spec);
CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
#undef INSTANTIATE_MACRO
}