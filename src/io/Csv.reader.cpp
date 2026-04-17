#include "Csv.reader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <cctype>

static std::string trim(const std::string &s) {
    const char* ws = " \t\r\n";
    auto l = s.find_first_not_of(ws);
    if (l == std::string::npos) return "";
    auto r = s.find_last_not_of(ws);
    return s.substr(l, r - l + 1);
}

std::vector<std::tuple<double,double,double,double,double>>
readCsvFiveColumns(const std::string &filename,
                   const std::string &comment_prefix,
                   const std::string &header_start) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Failed to open file: " + filename);

    std::vector<std::tuple<double,double,double,double,double>> rows;
    std::string line;
    while (std::getline(in, line)) {
        std::string t = trim(line);
        if (t.empty()) continue;
        if (!comment_prefix.empty() && t.rfind(comment_prefix, 0) == 0) continue;
        if (!header_start.empty() && t.rfind(header_start, 0) == 0) continue;

        std::istringstream ss(line);
        std::string field;
        std::vector<std::string> fields;
        while (std::getline(ss, field, ',')) fields.push_back(trim(field));

        if (fields.size() != 5) {
            // skip malformed lines
            continue;
        }

        try {
            double a = std::stod(fields[0]);
            double b = std::stod(fields[1]);
            double c = std::stod(fields[2]);
            double d = std::stod(fields[3]);
            double e = std::stod(fields[4]);
            rows.emplace_back(a,b,c,d,e);
        } catch (...) {
            // skip unparsable rows
            continue;
        }
    }

    return rows;
}
