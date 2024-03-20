#include <iostream>
#include <vector>

class Histogram {
private:
    std::vector<int> data;

public:
    Histogram(int size) {
        data.resize(size);
    }

    void clear() {
        for (int i = 0; i < data.size(); i++) {
            data[i] = 0;
        }
    }

    void addValue(int value) {
        data[value]++;
    }

    int getValue(int value) const {
        return data[value];
    }

    int getSize() const {
        return data.size();
    }
};

class PyramidOfHistograms {
private:
    std::vector<Histogram*> histograms;
    int numLevels;

public:
    PyramidOfHistograms(int numLevels, int size) {
        this->numLevels = numLevels;

        histograms.resize(numLevels);
        for (int i = 0; i < numLevels; i++) {
            histograms[i] = new Histogram(size);
        }
    }

    ~PyramidOfHistograms() {
        for (int i = 0; i < numLevels; i++) {
            delete histograms[i];
        }
    }

    void clear() {
        for (int i = 0; i < numLevels; i++) {
            histograms[i]->clear();
        }
    }

    void addToHistogram(int value, int level) {
        histograms[level]->addValue(value);
    }

    int search(int value) const {
        int count = 0;

        for (int i = 0; i < numLevels; i++) {
            int histogramSize = histograms[i]->getSize();
            int searchRange = histogramSize / 2;

            int lowerBound = value - searchRange;
            int upperBound = value + searchRange;

            if (lowerBound < 0) {
                upperBound += abs(lowerBound);
                lowerBound = 0;
            }

            if (upperBound >= histogramSize) {
                lowerBound -= (upperBound - histogramSize) + 1;
                upperBound = histogramSize - 1;
            }

            for (int j = lowerBound; j <= upperBound; j++) {
                count += histograms[i]->getValue(j);
            }

            value /= 2;
        }

        return count;
    }
};

int main() {
    PyramidOfHistograms pyramid(4, 256);

    pyramid.addToHistogram(100, 0);
    pyramid.addToHistogram(150, 1);
    pyramid.addToHistogram(175, 2);
    pyramid.addToHistogram(200, 3);

    std::cout << "Count of values between 160-170: " << pyramid.search(165) << std::endl;
}