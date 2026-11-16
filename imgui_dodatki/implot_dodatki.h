#pragma once

#include "implot.h"

namespace ImPlot {
	template <typename T>
	void PlotLineMoje(const char* label_id, const T* xs, const T* ys, int count, ImPlotLineFlags flags, int strideX, int strideY);
}
