#pragma once
#include "../include/Bin2D.h"

struct Histogram3D {
	static const sf::Vector3f blue;
	static const sf::Vector3f red;
	static constexpr float maxHeight = 0.6f;
	static constexpr float minHeight = -0.4f;
	static constexpr float xLow = -0.75f;
	static constexpr float xHigh = -xLow;
	static constexpr float zLow = -0.75f;
	static constexpr float zHigh = -zLow;

	std::vector<std::vector<Bin2D>> trueBins;
	std::vector<std::vector<Bin2D>> falseBins;
	int highestValuesCount = 0;
	float gridBinsStep[2];
	float gridHeightStep = (maxHeight - minHeight) / 10.f;
	bool drawingReady = false;
	bool sectioningReady = false;
	bool isGridWanted = true;
	std::vector<std::tuple<double, double, bool>> data;

	Histogram3D() {}

	Histogram3D(int binsNumberAlongColumn1, int binsNumberAlongColumn2, std::vector<std::tuple<double, double, bool>>& data);

	static std::vector<std::tuple<double, double, bool>> loadData(std::string const& csvPath, char delimiter,
		int column1Number, int column2Number, int boolColumnNumber, bool skipFirstRow);

	static std::vector<std::tuple<double, double, bool>> loadData(std::string const& csvPath, char delimiter,
		std::string const& column1Name, std::string const& column2Name, std::string const& boolColumnName);

	static std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str, char delimiter);

	static int getColumnNumberByName(std::vector<std::string>, std::string const& columnName);

	void sortDataAndUpdateHistogramAndBins();

	void prepareForDrawing();

	void rebin(int newBinsNumberAlongColumn1, int newBinsNumberAlongColumn2);

	void updateGridSteps(int* gridSquaresNumbers);
};