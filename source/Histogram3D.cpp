#include "../include/pch.h"
#include "../include/Histogram3D.h"

const sf::Vector3f Histogram3D::blue{ 3 / 255.f, 190 / 255.f, 252 / 255.f };
const sf::Vector3f Histogram3D::red{ 207 / 255.f, 48 / 255.f, 93 / 255.f };

std::vector<std::string> Histogram3D::getNextLineAndSplitIntoTokens(std::istream& istream, char delimiter) {
     std::vector<std::string> result;
     std::string line;
     std::getline(istream, line);
     std::stringstream lineStream(line);
     std::string cell;
     std::string trimmed_cell;
     while (std::getline(lineStream, cell, delimiter)) {
         trimmed_cell = cell.erase(0, cell.find_first_not_of(" \n\r\t"));
         trimmed_cell = trimmed_cell.erase(cell.find_last_not_of(" \n\r\t") + 1);
         result.push_back(trimmed_cell);
     }
     return result;
 }

int Histogram3D::getColumnNumberByName(std::vector<std::string> tokens, std::string const& columnName) {
    for (int columnNumber = 0; columnNumber < tokens.size(); ++columnNumber) {
        if (tokens[columnNumber] == columnName) {
            return columnNumber;
        }
    }
    throw std::runtime_error("Column name not found: " + columnName);
}

std::vector<std::tuple<double, double, bool>> Histogram3D::loadData(std::string const& csvPath, char delimiter,
    int column1Number, int column2Number, int boolColumnNumber, bool skipFirstRow) {
    std::vector<std::tuple<double, double, bool>> result;
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + csvPath);
    }
    if (skipFirstRow) {
        Histogram3D::getNextLineAndSplitIntoTokens(file, delimiter);
    }
    std::vector<std::string> tokens;
    int rowCounter = 0;
    while (file) {
        tokens = Histogram3D::getNextLineAndSplitIntoTokens(file, delimiter);
        if (tokens.size() == 0) {
            break;
        }
        if (tokens.size() <= std::max({ column1Number, column2Number, boolColumnNumber })) {
            throw std::runtime_error("Not enought values in row " + std::to_string(rowCounter) +
                ". Found " + std::to_string(tokens.size()) + ", needed " +
                std::to_string(std::max({ column1Number, column2Number, boolColumnNumber }) + 1));
        }
        double value1, value2;
        try {
            value1 = std::stod(tokens[column1Number]);
            value2 = std::stod(tokens[column2Number]);
        }
        catch (std::invalid_argument e) {
            throw std::runtime_error("Value in row " + std::to_string(rowCounter) +
                " can not be converted to float");
        }
        bool boolValue = (tokens[boolColumnNumber] == "1" || tokens[boolColumnNumber] == "true" ||
            tokens[boolColumnNumber] == "True");
        result.emplace_back(value1, value2, boolValue);
        ++rowCounter;
    }
    return result;
}

std::vector<std::tuple<double, double, bool>> Histogram3D::loadData(std::string const& csvPath, char delimiter,
    std::string const& column1Name, std::string const& column2Name, std::string const& boolColumnName) {
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + csvPath);
    }
    auto tokens = Histogram3D::getNextLineAndSplitIntoTokens(file, delimiter);
    int column1Number = Histogram3D::getColumnNumberByName(tokens, column1Name);
    int column2Number = Histogram3D::getColumnNumberByName(tokens, column2Name);
    int boolColumnNumber = Histogram3D::getColumnNumberByName(tokens, boolColumnName);
    return Histogram3D::loadData(csvPath, delimiter,
        column1Number, column2Number, boolColumnNumber, true);
}

Histogram3D::Histogram3D(int binsNumberAlongColumn1, int binsNumberAlongColumn2, std::vector<std::tuple<double, double, bool>>& data) {
	double minValue1, maxValue1, minValue2, maxValue2;
    auto minMaxValue1IteratorPair = std::minmax_element(data.begin(), data.end(),
        [](std::tuple<double, double, bool> const& t1, std::tuple<double, double, bool> const& t2) { return std::get<0>(t1) < std::get<0>(t2); });
    minValue1 = std::get<0>(*std::get<0>(minMaxValue1IteratorPair));
    maxValue1 = std::get<0>(*std::get<1>(minMaxValue1IteratorPair));
    auto minMaxValue2IteratorPair = std::minmax_element(data.begin(), data.end(),
        [](std::tuple<double, double, bool> const& t1, std::tuple<double, double, bool> const& t2) { return std::get<1>(t1) < std::get<1>(t2); });
    minValue2 = std::get<1>(*std::get<0>(minMaxValue2IteratorPair));
    maxValue2 = std::get<1>(*std::get<1>(minMaxValue2IteratorPair));
    trueBins.resize(binsNumberAlongColumn1);
    falseBins.resize(binsNumberAlongColumn1);
    for (int i = 0; i < binsNumberAlongColumn1; ++i) {
        trueBins[i].resize(binsNumberAlongColumn2);
        falseBins[i].resize(binsNumberAlongColumn2);
        for (int j = 0; j < binsNumberAlongColumn2; ++j) {
            double leftEdgeColumn1 = minValue1 + (maxValue1 - minValue1) / binsNumberAlongColumn1 * i;
            double rightEdgeColumn1 = minValue1 + (maxValue1 - minValue1) / binsNumberAlongColumn1 * (i + 1);
            double leftEdgeColumn2 = minValue2 + (maxValue2 - minValue2) / binsNumberAlongColumn2 * i;
            double rightEdgeColumn2 = minValue2 + (maxValue2 - minValue2) / binsNumberAlongColumn2 * (i + 1);
            trueBins[i][j] = Bin2D(leftEdgeColumn1, rightEdgeColumn1, leftEdgeColumn2, rightEdgeColumn2);
            falseBins[i][j] = Bin2D(leftEdgeColumn1, rightEdgeColumn1, leftEdgeColumn2, rightEdgeColumn2);
        }
    }
    for (auto const& [value1, value2, boolValue] : data) {
        int binIndex1 = std::clamp(static_cast<int>((value1 - minValue1) / (maxValue1 - minValue1) * binsNumberAlongColumn1), 0, binsNumberAlongColumn1 - 1);
        int binIndex2 = std::clamp(static_cast<int>((value2 - minValue2) / (maxValue2 - minValue2) * binsNumberAlongColumn2), 0, binsNumberAlongColumn2 - 1);
        if (boolValue) {
            trueBins[binIndex1][binIndex2].valuesCount += 1;
        }
        else {
            falseBins[binIndex1][binIndex2].valuesCount += 1;
        }
	}
    gridBinsStep[0] = binsNumberAlongColumn1 / 10.f;
    gridBinsStep[1] = binsNumberAlongColumn2 / 10.f;
    prepareForDrawing();
    this->data = std::move(data);
}

void Histogram3D::sortDataAndUpdateHistogramAndBins() {
    std::sort(data.begin(), data.end(), [](std::tuple<double, double, bool>const& t1, std::tuple<double, double, bool>const& t2) { return std::get<1>(t1) < std::get<1>(t2); });
    std::stable_sort(data.begin(), data.end(), [](std::tuple<double, double, bool>const& t1, std::tuple<double, double, bool>const& t2) { return std::get<0>(t1) < std::get<0>(t2); });
    double minValue1 = trueBins.front().front().minColumn1;
    double maxValue1 = trueBins.back().back().maxColumn1;
    double minValue2 = trueBins.front().front().minColumn2;
    double maxValue2 = trueBins.back().back().maxColumn2;
    for (auto const& [value1, value2, boolValue] : data) {
        int binIndex1 = std::clamp(static_cast<int>((value1 - minValue1) / (maxValue1 - minValue1) * trueBins.size()), 0, static_cast<int>(trueBins.size()) - 1);
        int binIndex2 = std::clamp(static_cast<int>((value2 - minValue2) / (maxValue2 - minValue2) * trueBins.front().size()), 0, static_cast<int>(trueBins.front().size()) - 1);
        if (boolValue) {
            trueBins[binIndex1][binIndex2].sortedValues.emplace_back(value1, value2);
        }
        else {
            falseBins[binIndex1][binIndex2].sortedValues.emplace_back(value1, value2);
        }
    }
    sectioningReady = true;
    data = std::vector<std::tuple<double, double, bool>>();
}

void Histogram3D::prepareForDrawing() {
    for (int i = 0; i < trueBins.size(); ++i) {
        for (int j = 0; j < trueBins[i].size(); ++j) {
            highestValuesCount = std::max(trueBins[i][j].valuesCount +
                falseBins[i][j].valuesCount, highestValuesCount);
        }
    }
    for (int i = 0; i < trueBins.size(); ++i) {
        for (int j = 0; j < trueBins[i].size(); ++j) {
            int totalCount = trueBins[i][j].valuesCount + falseBins[i][j].valuesCount;
            float trueToAllProportion;
            if (totalCount == 0) {
                trueToAllProportion = 0;
            }
            else {
                trueToAllProportion = static_cast<float>(trueBins[i][j].valuesCount) / totalCount;
            }
            const float left = xLow + (xHigh - xLow) / trueBins.size() * i;
            const float right = xLow + (xHigh - xLow) / trueBins.size() * (i + 1);
            const float _near = zLow + (zHigh - zLow) / trueBins[i].size() * j;
            const float _far = zLow + (zHigh - zLow) / trueBins[i].size() * (j + 1);
            const float top = minHeight + static_cast<float>(totalCount) / highestValuesCount * (maxHeight - minHeight);
            trueBins[i][j].leftBottomNearPoint = { left, minHeight, _near };
            trueBins[i][j].rightTopFarPoint = { right, minHeight + trueToAllProportion * (top - minHeight), _far };
            if (trueToAllProportion == 1) {
                trueBins[i][j].drawTopFace = true;
            }
            else {
                trueBins[i][j].drawTopFace = false;
            }
            falseBins[i][j].leftBottomNearPoint = { left, minHeight + trueToAllProportion * (top - minHeight), _near };
            falseBins[i][j].rightTopFarPoint = { right, top, _far };
            falseBins[i][j].drawTopFace = true;
        }
    }
}