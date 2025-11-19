#pragma once

struct Bin2D {
	double minColumn1, minColumn2, maxColumn1, maxColumn2;
	int valuesCount = 0;
	std::vector<std::pair<double, double>> sortedValues;
	// ?zmienna pomagaj¹ca w rysowaniu przekroji, np. indeks wartoœci na granicy przekroju

	Bin2D() {};

	Bin2D(double min1, double max1, double min2, double max2)
		: minColumn1{ min1 }, maxColumn1{ max1 }, minColumn2{ min2 }, maxColumn2{ max2 } {
	};
};