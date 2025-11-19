#pragma once
#include "../include/Bin2D.h"

struct Histogram3D {
public:
	std::vector<std::vector<Bin2D>> trueBins;
	std::vector<std::vector<Bin2D>> falseBins;
	bool drawingReady = false;
	bool sectioningReady = false;
	std::vector<std::tuple<double, double, bool>> data;

	Histogram3D() {}

	Histogram3D(int binsNumberAlongColumn1, int binsNumberAlongColumn2, std::vector<std::tuple<double, double, bool>> &data);

	static std::vector<std::tuple<double, double, bool>> loadData(std::string const &csvPath, char delimiter,
            int column1Number, int column2Number, int boolColumnNumber, bool skipFirstRow);

	static std::vector<std::tuple<double, double, bool>> loadData(std::string const &csvPath, char delimiter,
            std::string const &column1Name, std::string const &column2Name, std::string const &boolColumnName);


	static std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str, char delimiter);

	static int getColumnNumberByName(std::vector<std::string>, std::string const& columnName);

	void sortDataAndUpdateHistogramAndBins();
};