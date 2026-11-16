#pragma once

namespace ImPlot {
    template <typename T>
    void PlotLineMoje(const char* label_id, const T* xs, const T* ys, int count, ImPlotLineFlags flags, int strideX, int strideY) {
        GetterXY<IndexerIdx<T>,IndexerIdx<T>> getter(IndexerIdx<T>(xs,count,0,strideX),IndexerIdx<T>(ys,count,0,strideY),count);
        PlotLineEx(label_id, getter, flags);
    }

    #define INSTANTIATE_MACRO(T) \
    template IMPLOT_API void PlotLineMoje<T>(const char* label_id, const T* xs, const T* ys, int count, ImPlotLineFlags flags, int strideX, int strideY);
    CALL_INSTANTIATE_FOR_NUMERIC_TYPES()
    #undef INSTANTIATE_MACRO
}




