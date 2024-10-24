#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <map>

// Array of Structures (AoS)
struct Pixel{
    uint8_t r, g, b;

    bool operator<(const Pixel &other) const {
        return std::tie(r, g, b) < std::tie(other.r, other.g, other.b);
    }
};

// Guardar la información del header de la imagen ppm en magic_number, width, height y max_color
void get_header(std::ifstream &infile, std::string &magic_number, int &width, int &height, int &max_color);

// Guardar los pixeles de la imagen ppm en una estructura AoS
void get_pixels(std::ifstream &infile, std::vector<Pixel> &pixel_data, int pixel_count, bool is_16_bit);

// Ecribir la información de la imagen en el archivo de salida
void write_info(std::ofstream &outfile, const std::string &magic_number, int width, int height, int max_color, const std::vector<Pixel> &pixel_data, bool is_16_bit);

// Comprimir la imagen
void write_cppm(std::ofstream &cppm_outfile, const std::vector<Pixel> &pixel_data, int width, int height, int max_color);

template<typename T>
T clamp(const T& value, const T& low, const T& high) {
    return (value < low) ? low : (value > high) ? high : value;
}
void maxlevel(std::vector<Pixel> &pixel_data, int new_maxlevel, int max_color);
#endif //FUNCTIONS_HPP