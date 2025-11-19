#include "../include/pch.h"
#include "../include/Histogram3D.h"

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
     for(int columnNumber = 0; columnNumber < tokens.size(); ++columnNumber) {
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