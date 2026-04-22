/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as Comma Separated Values (CSV).
 **/
#include "Csv.h"
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

void tsunami_lab::io::Csv::write(t_real i_dxy, t_idx i_nx, t_idx i_ny,
                                 t_idx i_stride, t_real const *i_h,
                                 t_real const *i_hu, t_real const *i_hv,
                                 std::ostream &io_stream) {
    // write the CSV header
    io_stream << "x,y";
    if (i_h != nullptr) {
        io_stream << ",height";
    }
    if (i_hu != nullptr) {
        io_stream << ",momentum_x";
    }
    if (i_hv != nullptr) {
        io_stream << ",momentum_y";
    }
    io_stream << "\n";

    // iterate over all cells
    for (t_idx l_iy = 0; l_iy < i_ny; l_iy++) {
        for (t_idx l_ix = 0; l_ix < i_nx; l_ix++) {
            // derive coordinates of cell center
            t_real l_posX = (l_ix + 0.5) * i_dxy;
            t_real l_posY = (l_iy + 0.5) * i_dxy;

            t_idx l_id = l_iy * i_stride + l_ix;

            // write data
            io_stream << l_posX << "," << l_posY;
            if (i_h != nullptr) {
                io_stream << "," << i_h[l_id];
            }
            if (i_hu != nullptr) {
                io_stream << "," << i_hu[l_id];
            }
            if (i_hv != nullptr) {
                io_stream << "," << i_hv[l_id];
            }
            io_stream << "\n";
        }
    }
    io_stream << std::flush;
}

static std::string trim(const std::string &s) {
    const char *ws = " \t\r\n";
    auto l = s.find_first_not_of(ws);
    if (l == std::string::npos) {
        return "";
    }
    auto r = s.find_last_not_of(ws);
    return s.substr(l, r - l + 1);
}

std::vector<std::tuple<double, double, double, double, double>>
tsunami_lab::io::Csv::readFive(std::istream &io_stream) {
    std::vector<std::tuple<double, double, double, double, double>> rows;
    std::string line;

    std::string field;
    std::vector<std::string> fields;

    while (std::getline(io_stream, line)) {
        std::string t = trim(line);
        if (t.find("#", 0) == 0) {
            continue;
        }

        std::istringstream ss(line);
        fields.clear();
        while (std::getline(ss, field, ',')) {
            fields.push_back(trim(field));
        }

        if (fields.size() != 5) {
            continue;
        }

        try {
            double a = std::stod(fields[0]);
            double b = std::stod(fields[1]);
            double c = std::stod(fields[2]);
            double d = std::stod(fields[3]);
            double e = std::stod(fields[4]);
            rows.emplace_back(a, b, c, d, e);
        } catch (...) {
            continue;
        }
    }

    return rows;
}
