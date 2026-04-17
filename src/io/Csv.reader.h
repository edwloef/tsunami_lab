#ifndef CSV_FIVE_COLUMNS_H
#define CSV_FIVE_COLUMNS_H

#include <string>
#include <vector>
#include <tuple>

// Reads a CSV with five numeric columns per row, skipping comment lines and a header.
// - filename: path to the CSV file.
// - comment_prefix: lines that start (after trimming leading whitespace) with this are ignored (default "#").
// - header_start: a non-empty string which, if a trimmed line starts with it, will be treated as the header and skipped (default "hLeft,").
// Returns a vector of rows, each row as std::tuple<double,double,double,double,double>.
// Throws std::runtime_error if the file cannot be opened.
std::vector<std::tuple<double,double,double,double,double>>
readCsvFiveColumns(const std::string &filename,
                   const std::string &comment_prefix = "#",
                   const std::string &header_start = "hLeft,");

#endif // CSV_FIVE_COLUMNS_H
