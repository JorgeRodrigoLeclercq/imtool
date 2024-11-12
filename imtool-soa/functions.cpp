#include "functions.hpp"
#include <algorithm> // For std::clamp
#include <cmath>
#include <bits/algorithmfwd.h>
#include <fstream>
#include <iostream>

// Guardar la información del header de la imagen ppm en magic_number, width, height y max_color
void get_header(std::ifstream &infile, ImageHeader &header) {
  const int MAX_IGNORE_CHARS = 256;
    // Leer el header
    infile >> header.magic_number >> header.dimensions.width >> header.dimensions.height >> header.max_color;

    if (header.magic_number != "P6") {
        std::cerr << "Error: This program only supports P6 (binary) format" << '\n';
        exit(1);
    }

    // Saltar espacios blancos
    infile.ignore(MAX_IGNORE_CHARS, '\n');

    // Log el header
    std::cout << "Magic Number: " << header.magic_number << '\n';
    std::cout << "Width: " << header.dimensions.width << '\n';
    std::cout << "Height: " << header.dimensions.height << '\n';
    std::cout << "Max Color: " << header.max_color << '\n';
}

void get_pixels(std::ifstream &infile, SoA &pixel_data, unsigned long long pixel_count, bool is_16_bit) {
    const int BIT_SHIFT = 8;
    const int FIRST_PIXELS = 10;

  if (is_16_bit) {
    // max_color > 255, por lo tanto, bits por píxel = 2, en little-endian
    for (unsigned long long int i = 0; i < pixel_count; ++i) {
      uint8_t red1 = 0;
      uint8_t red2 = 0;
      uint8_t green1 = 0;
      uint8_t green2 = 0;
      uint8_t blue1 = 0;
      uint8_t blue2 = 0;

      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      infile.read(reinterpret_cast<char*>(&red1), 1);  // Leer primer byte de rojo
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      infile.read(reinterpret_cast<char*>(&red2), 1);  // Leer segundo byte de rojo

      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      infile.read(reinterpret_cast<char*>(&green1), 1);  // Leer primer byte de verde
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      infile.read(reinterpret_cast<char*>(&green2), 1);  // Leer segundo byte de verde

      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      infile.read(reinterpret_cast<char*>(&blue1), 1);  // Leer primer byte de azul
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      infile.read(reinterpret_cast<char*>(&blue2), 1);  // Leer segundo byte de azul

      // Little-endian: combinar los dos bytes para cada componente de color
      pixel_data.r[i] = static_cast<uint16_t>(red1 | (red2 << BIT_SHIFT));
      pixel_data.g[i] = static_cast<uint16_t>(green1 | (green2 << BIT_SHIFT));
      pixel_data.b[i] = static_cast<uint16_t>(blue1 | (blue2 << BIT_SHIFT));
    }
  } else {
    // max_color <= 255, por lo tanto, bits por píxel = 1
    for (unsigned long long i = 0; i < pixel_count; ++i) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      infile.read(reinterpret_cast<char*>(&pixel_data.r[i]), 1);  // Leer un byte de rojo

      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      infile.read(reinterpret_cast<char*>(&pixel_data.g[i]), 1);  // Leer un byte de verde

      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      infile.read(reinterpret_cast<char*>(&pixel_data.b[i]), 1);  // Leer un byte de azul
    }
  }

  // Log de los primeros píxeles
  std::cout << "Primeros píxeles (valores RGB):" << '\n';
  for (unsigned long long int i = 0; i < std::min(static_cast<unsigned long long int>(FIRST_PIXELS), pixel_count); ++i) {
    std::cout << "Píxel " << i << ": "
              << "R = " << static_cast<int>(pixel_data.r[i]) << ", "
              << "G = " << static_cast<int>(pixel_data.g[i]) << ", "
              << "B = " << static_cast<int>(pixel_data.b[i]) << '\n';
  }
}

void write_info(std::ofstream& outfile, ImageHeader& header, SoA& pixel_data, bool is_16_bit) {
    const uint16_t MASK_BYTE = 0xFF;
    const int BITS_PER_BYTE = 8;

  outfile << header.magic_number << "\n";
  outfile << header.dimensions.width << " " << header.dimensions.height << "\n";
  outfile << header.max_color << "\n";

  if (is_16_bit) {
    for (std::size_t i = 0; i < pixel_data.r.size(); ++i) {
      uint8_t red1 = pixel_data.r[i] & MASK_BYTE;
      uint8_t red2 = (pixel_data.r[i] >> BITS_PER_BYTE) & MASK_BYTE;
      uint8_t green1 = pixel_data.g[i] & MASK_BYTE;
      uint8_t green2 = (pixel_data.g[i] >> BITS_PER_BYTE) & MASK_BYTE;
      uint8_t blue1 = pixel_data.b[i] & MASK_BYTE;
      uint8_t blue2 = (pixel_data.b[i] >> BITS_PER_BYTE) & MASK_BYTE;

      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      outfile.write(reinterpret_cast<const char*>(&red1), 1);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      outfile.write(reinterpret_cast<const char*>(&red2), 1);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      outfile.write(reinterpret_cast<const char*>(&green1), 1);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      outfile.write(reinterpret_cast<const char*>(&green2), 1);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      outfile.write(reinterpret_cast<const char*>(&blue1), 1);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      outfile.write(reinterpret_cast<const char*>(&blue2), 1);
    }
  } else {
    // Para imágenes de 8 bits por componente
    for (std::size_t i = 0; i < pixel_data.r.size(); ++i) {
      // Escribir un byte de cada componente de color
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      outfile.write(reinterpret_cast<const char*>(&pixel_data.r[i]), 1);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      outfile.write(reinterpret_cast<const char*>(&pixel_data.g[i]), 1);
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      outfile.write(reinterpret_cast<const char*>(&pixel_data.b[i]), 1);
    }
  }
}

constexpr int MAX_COLOR_8BIT = 255;

void maxlevel(int new_maxlevel, bool& is_16_bit, SoA& pixel_data, ImageHeader& header) {
  // Determinar si la salida será de 8 o 16 bits
  is_16_bit = new_maxlevel > MAX_COLOR_8BIT;

  // Escalar los componentes de color sin redondeo para cada canal
  for (size_t i = 0; i < pixel_data.r.size(); ++i) {
    uint16_t r_scaled = static_cast<uint16_t>(
        std::clamp(static_cast<int>(static_cast<float>(pixel_data.r[i]) * static_cast<float>(new_maxlevel) / static_cast<float>(header.max_color)),
                   0, new_maxlevel));
    uint16_t g_scaled = static_cast<uint16_t>(
        std::clamp(static_cast<int>(static_cast<float>(pixel_data.g[i]) * static_cast<float>(new_maxlevel) / static_cast<float>(header.max_color)),
                   0, new_maxlevel));
    uint16_t b_scaled = static_cast<uint16_t>(
        std::clamp(static_cast<int>(static_cast<float>(pixel_data.b[i]) * static_cast<float>(new_maxlevel) / static_cast<float>(header.max_color)),
                   0, new_maxlevel));

    // Asignar los valores escalados
    pixel_data.r[i] = r_scaled;
    pixel_data.g[i] = g_scaled;
    pixel_data.b[i] = b_scaled;
  }

  // Actualizar max_color al nuevo nivel máximo
  header.max_color = new_maxlevel;
}

