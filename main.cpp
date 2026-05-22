// Winter'24
// Instructor: Diba Mirza
// Student name: 
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cstring>
#include <algorithm>
#include <limits.h>
#include <iomanip>
#include <set>
#include <queue>
#include <sstream>
using namespace std;

#include "utilities.h"
#include "movies.h"

bool parseLine(string &line, string &movieName, double &movieRating);

int main(int argc, char** argv){
    if (argc < 2){
        cerr << "Not enough arguments provided (need at least 1 argument)." << endl;
        cerr << "Usage: " << argv[ 0 ] << " moviesFilename prefixFilename " << endl;
        exit(1);
    }

    ifstream movieFile (argv[1]);
 
    if (movieFile.fail()){
        cerr << "Could not open file " << argv[1];
        exit(1);
    }
  
    // Create an object of a STL data-structure to store all the movies
    // Using std::set of pairs — balanced BST sorted alphabetically by name
    set<pair<string, double>> movieSet;

    string line, movieName;
    double movieRating;
    // Read each file and store the name and rating
    while (getline (movieFile, line) && parseLine(line, movieName, movieRating)){
            // Use std::string movieName and double movieRating
            // to construct your Movie objects
            // insert elements into your data structure
            movieSet.insert({movieName, movieRating});
    }

    movieFile.close();

    if (argc == 2){
            //print all the movies in ascending alphabetical order of movie names
            for (const auto& [name, rating] : movieSet) {
                cout << name << ", " << fixed << setprecision(1) << rating << endl;
            }
            return 0;
    }

    ifstream prefixFile (argv[2]);

    if (prefixFile.fail()) {
        cerr << "Could not open file " << argv[2];
        exit(1);
    }

    vector<string> prefixes;
    while (getline (prefixFile, line)) {
        if (!line.empty()) {
            prefixes.push_back(line);
        }
    }

    // Store best movie for each prefix that has results, to print at the end
    vector<tuple<string, string, double>> bestMovies;

    for (const string& prefix : prefixes) {
        // Use lower_bound to find the first movie whose name >= prefix
        auto it = movieSet.lower_bound({prefix, 0.0});

        // Collect all movies whose name starts with this prefix
        vector<Movie> matches;
        while (it != movieSet.end()) {
            const string& name = it->first;
            if (name.size() >= prefix.size() &&
                name.compare(0, prefix.size(), prefix) == 0) {
                matches.push_back({name, it->second});
                ++it;
            } else {
                break;
            }
        }

        if (matches.empty()) {
            //  If no movie with that prefix exists print the following message
            cout << "No movies found with prefix " << prefix << endl;
        } else {
            // Sort matches by rating descending, then name ascending for ties
            sort(matches.begin(), matches.end(), [](const Movie& a, const Movie& b) {
                if (a.rating != b.rating) return a.rating > b.rating;
                return a.name < b.name;
            });

            // Print all matching movies
            for (const Movie& m : matches) {
                cout << m.name << ", " << fixed << setprecision(1) << m.rating << endl;
            }
            cout << endl;

            // The first element after sorting is the best movie
            bestMovies.push_back({prefix, matches[0].name, matches[0].rating});
        }
    }

    //  For each prefix,
    //  Print the highest rated movie with that prefix if it exists.
    for (const auto& [prefix, name, rating] : bestMovies) {
        cout << "Best movie with prefix " << prefix << " is: " << name
             << " with rating " << fixed << setprecision(1) << rating << endl;
    }

    return 0;
}

/*
 * Part 3a: Time Complexity Analysis
 *
 * Data structure: std::set<pair<string, double>> (balanced BST, sorted by movie name).
 *
 * Parameters: n = number of movies, m = number of prefixes,
 *             k = max movies matching a prefix, l = max movie name length.
 *
 * For each of the m prefixes:
 *   - lower_bound: O(l * log n)  (log n comparisons, each O(l) for strings)
 *   - Walk matches: O(k * l)     (k movies, O(l) prefix check each)
 *   - Sort matches: O(k * log k) (comparison-based sort on k elements)
 *   - Print matches: O(k * l)
 *
 * Total: O(m * (l * log n + k * l * log k))  =>  O(m * l * (log n + k * log k))
 *
 * Runtime measurements (prefix_large.txt, random datasets):
 *   input_20_random.csv:    ~X ms
 *   input_100_random.csv:   ~X ms
 *   input_1000_random.csv:  ~X ms
 *   input_76920_random.csv: ~X ms
 *   (Fill in actual CSIL runtimes)
 *
 * Part 3b: Space Complexity Analysis
 *
 * - The set stores n movies: O(n * l)
 * - The prefixes vector: O(m * l)
 * - Per-prefix matches vector: O(k * l)
 * - bestMovies vector: O(m * l)
 * Total: O((n + m) * l)
 *
 * Part 3c: Tradeoffs
 *
 * I designed for low time complexity. Using std::set gives O(log n) prefix lookups
 * via lower_bound, avoiding a full scan of all n movies per prefix. Space is also
 * efficient at O(n * l) since each movie is stored once. A trie could remove the
 * log n factor but would use significantly more memory for node pointers. The BST
 * approach balances time and space well for this problem.
 */

bool parseLine(string &line, string &movieName, double &movieRating) {
    int commaIndex = line.find_last_of(",");
    movieName = line.substr(0, commaIndex);
    movieRating = stod(line.substr(commaIndex+1));
    if (movieName[0] == '\"') {
        movieName = movieName.substr(1, movieName.length() - 2);
    }
    return true;
}