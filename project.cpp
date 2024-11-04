#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <tuple>

using namespace std;

// Function to read and normalize text files
string readAndNormalizeFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
        return "";
    }
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    
    // Converting to uppercase
    transform(content.begin(), content.end(), content.begin(), ::toupper);
    
    // Removing non-alphanumeric characters
    content.erase(remove_if(content.begin(), content.end(), [](char c) {
        return !isalnum(c) && !isspace(c);
    }), content.end());
    
    return content;
}

// Function to calculate word frequencies
void calculateWordFrequencies(const string& text, map<string, int>& wordFreq) {
    istringstream stream(text);
    string word;
    set<string> commonWords = {"A", "AND", "AN", "OF", "IN", "THE"};
    
    while (stream >> word) {
        if (commonWords.find(word) == commonWords.end()) {
            wordFreq[word]++;
        }
    }
}

// Function to normalize word frequencies
void normalizeFrequencies(const map<string, int>& wordFreq, map<string, double>& normalizedFreq) {
    int totalWords = accumulate(wordFreq.begin(), wordFreq.end(), 0, [](int sum, const pair<string, int>& p) {
        return sum + p.second;
    });
    
    for (const auto& pair : wordFreq) {
        normalizedFreq[pair.first] = static_cast<double>(pair.second) / totalWords;
    }
}

// Function to calculate similarity index
double calculateSimilarityIndex(const map<string, double>& freq1, const map<string, double>& freq2) {
    double similarity = 0.0;
    
    for (const auto& pair : freq1) {
        if (freq2.find(pair.first) != freq2.end()) {
            similarity += pair.second + freq2.at(pair.first);
        }
    }
    
    return similarity;
}

int main() {
    const string bookListFile = "book_names.txt";
    vector<string> fileNames;
    ifstream bookList(bookListFile);
    string fileName;

    // Read the list of book file names
    while (getline(bookList, fileName)) {
        fileNames.push_back(fileName);
    }
    bookList.close();

    int numFiles = fileNames.size();
    vector<string> fileContents(numFiles);
    vector<map<string, double>> normalizedFrequencies(numFiles);

    // Read and normalize files
    for (int i = 0; i < numFiles; ++i) {
        string filePath = "Book-Txt\\" + fileNames[i]; // Please adjust the file path as needed
        fileContents[i] = readAndNormalizeFile(filePath);
    }

    // Calculate word frequencies and normalize them
    for (int i = 0; i < numFiles; ++i) {
        map<string, int> wordFreq;
        calculateWordFrequencies(fileContents[i], wordFreq);
        normalizeFrequencies(wordFreq, normalizedFrequencies[i]);
    }

    // Create similarity matrix
    vector<vector<double>> similarityMatrix(numFiles, vector<double>(numFiles, 0.0));
    for (int i = 0; i < numFiles; ++i) {
        for (int j = i + 1; j < numFiles; ++j) {
            similarityMatrix[i][j] = calculateSimilarityIndex(normalizedFrequencies[i], normalizedFrequencies[j]);
        }
    }

    // Identify top 10 similar pairs
    vector<tuple<int, int, double>> similarityPairs;
    for (int i = 0; i < numFiles; ++i) {
        for (int j = i + 1; j < numFiles; ++j) {
            similarityPairs.push_back(make_tuple(i, j, similarityMatrix[i][j]));
        }
    }

    sort(similarityPairs.begin(), similarityPairs.end(), [](const tuple<int, int, double>& a, const tuple<int, int, double>& b) {
        return get<2>(a) > get<2>(b);
    });

    for (int i = 0; i < 10; ++i) {
        cout << "Pair: (" << fileNames[get<0>(similarityPairs[i])] << ", " << fileNames[get<1>(similarityPairs[i])] << ") - Similarity: " << get<2>(similarityPairs[i]) << endl;
    }

    return 0;
}