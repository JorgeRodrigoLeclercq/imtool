#include <gtest/gtest.h>
#include <gsl/gsl>
#include <vector>
#include "../imgaos/maxlevel.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)

// Caso base de prueba: Verifica que el valor de max_color se actualiza y los píxeles se escalan correctamente
TEST(MaxLevelTest, TestLevelUpdate) {
  // Inicializar ImageHeader con max_color a 255
  ImageHeader header{"P6", {100, 100}, 255};  // Imagen de 100x100 con max_color 255
  bool is_16_bit = false;

  // Crear algunos píxeles para probar con un std::vector<Pixel>
  std::vector<Pixel> pixel_data = {
    Pixel(100, 150, 200),
    Pixel(50, 75, 100)
  };

  // Convertir std::vector a gsl::span
  gsl::span<Pixel> pixel_span(pixel_data);

  // Llamar a la función maxlevel
  maxlevel(500, is_16_bit, pixel_span, header);

  // Verificar que max_color se actualiza correctamente
  EXPECT_EQ(header.max_color, 500);
  EXPECT_TRUE(is_16_bit);  // Debería ser 16 bits

  // Verificar que los valores de los píxeles se han escalado correctamente
  EXPECT_EQ(pixel_data[0].channels.red, 196);
  EXPECT_EQ(pixel_data[0].channels.green, 294);
  EXPECT_EQ(pixel_data[0].channels.blue, 392);

  EXPECT_EQ(pixel_data[1].channels.red, 98);
  EXPECT_EQ(pixel_data[1].channels.green, 147);
  EXPECT_EQ(pixel_data[1].channels.blue, 196);
}

// Test para verificar si el valor de is_16_bit se ajusta correctamente
TEST(MaxLevelTest, TestIs16Bit) {
  ImageHeader header{"P6", {100, 100}, 255};  // Inicializar con max_color 255
  bool is_16_bit = false;

  std::vector<Pixel> pixel_data = {
    Pixel(100, 150, 200)
  };

  gsl::span<Pixel> pixel_span(pixel_data);

  // Probar con un nuevo nivel mayor a 255, lo que debería establecer is_16_bit en true
  maxlevel(1000, is_16_bit, pixel_span, header);
  EXPECT_TRUE(is_16_bit);

  // Probar con un nuevo nivel menor o igual a 255, lo que debería mantener is_16_bit en false
  maxlevel(255, is_16_bit, pixel_span, header);
  EXPECT_FALSE(is_16_bit);
}

// Test para verificar la escala de píxeles con 8 bits
TEST(MaxLevelTest, TestScale8Bit) {
  ImageHeader header{"P6", {2, 2}, 255};  // Imagen de 2x2 con max_color 255
  bool is_16_bit = false;

  std::vector<Pixel> pixel_data = {
    Pixel(255, 255, 255),  // Píxel con color máximo
    Pixel(0, 0, 0)         // Píxel con color mínimo
  };

  // Convertir std::vector a gsl::span
  gsl::span<Pixel> pixel_span(pixel_data);

  maxlevel(255, is_16_bit, pixel_span, header);

  // Verificar que los valores de los píxeles no cambian para 8 bits
  EXPECT_EQ(pixel_data[0].channels.red, 255);
  EXPECT_EQ(pixel_data[0].channels.green, 255);
  EXPECT_EQ(pixel_data[0].channels.blue, 255);

  EXPECT_EQ(pixel_data[1].channels.red, 0);
  EXPECT_EQ(pixel_data[1].channels.green, 0);
  EXPECT_EQ(pixel_data[1].channels.blue, 0);
}

// Test para verificar la escala de píxeles con 16 bits
TEST(MaxLevelTest, TestScale16Bit) {
  ImageHeader header{"P6", {2, 2}, 255};  // Imagen de 2x2 con max_color 255
  bool is_16_bit = false;

  std::vector<Pixel> pixel_data = {
    Pixel(255, 255, 255),  // Píxel con color máximo
    Pixel(0, 0, 0)         // Píxel con color mínimo
  };

  // Convertir std::vector a gsl::span
  gsl::span<Pixel> pixel_span(pixel_data);

  maxlevel(1000, is_16_bit, pixel_span, header);  // Actualizar a 16 bits

  // Verificar que los valores de los píxeles se escalan correctamente para 16 bits
  EXPECT_EQ(pixel_data[0].channels.red, 1000);
  EXPECT_EQ(pixel_data[0].channels.green, 1000);
  EXPECT_EQ(pixel_data[0].channels.blue, 1000);

  EXPECT_EQ(pixel_data[1].channels.red, 0);
  EXPECT_EQ(pixel_data[1].channels.green, 0);
  EXPECT_EQ(pixel_data[1].channels.blue, 0);
}
// Test para verificar el cambio de representación de píxeles (3 bytes a 6 bytes y viceversa)
TEST(MaxLevelTest, TestChangeRepresentation) {
  // Imagen con max_color 255 (representación 3 bytes por píxel)
  ImageHeader header{"P6", {2, 2}, 255};  // Imagen 2x2 con 255 como valor máximo
  bool is_16_bit = false;  // Inicialmente no es de 16 bits

  std::vector<Pixel> pixel_data = {
    Pixel(255, 255, 255),  // Píxel con color máximo
    Pixel(0, 0, 0)         // Píxel con color mínimo
  };

  // Convertir std::vector a gsl::span
  gsl::span<Pixel> pixel_span(pixel_data);

  // Escalar a 16 bits (max_color > 255)
  maxlevel(1000, is_16_bit, pixel_span, header);

  // Verificar que se haya cambiado a 16 bits (6 bytes por píxel)
  EXPECT_EQ(header.max_color, 1000);
  EXPECT_TRUE(is_16_bit);  // La imagen ahora es de 16 bits

  // Verificar que los valores de los píxeles se han escalado correctamente para 16 bits
  EXPECT_EQ(pixel_data[0].channels.red, 1000);
  EXPECT_EQ(pixel_data[0].channels.green, 1000);
  EXPECT_EQ(pixel_data[0].channels.blue, 1000);

  // Escalar de nuevo a 8 bits (max_color <= 255)
  maxlevel(255, is_16_bit, pixel_span, header);

  // Verificar que se ha vuelto a 8 bits (3 bytes por píxel)
  EXPECT_EQ(header.max_color, 255);
  EXPECT_FALSE(is_16_bit);  // La imagen ahora es de 8 bits

  // Verificar que los valores de los píxeles no se desborden al escalar de vuelta a 8 bits
  EXPECT_EQ(pixel_data[0].channels.red, 255);
  EXPECT_EQ(pixel_data[0].channels.green, 255);
  EXPECT_EQ(pixel_data[0].channels.blue, 255);

  EXPECT_EQ(pixel_data[1].channels.red, 0);
  EXPECT_EQ(pixel_data[1].channels.green, 0);
  EXPECT_EQ(pixel_data[1].channels.blue, 0);
}
// Test para verificar que los valores máximos de los canales no se excedan
TEST(MaxLevelTest, TestExtremeColorValues) {
  ImageHeader header{"P6", {2, 2}, 255};  // Imagen 2x2 con max_color 255
  bool is_16_bit = false;

  std::vector<Pixel> pixel_data = {
    Pixel(255, 255, 255),  // Píxel con valor máximo
    Pixel(0, 0, 0)         // Píxel con valor mínimo
  };

  // Convertir std::vector a gsl::span
  gsl::span<Pixel> pixel_span(pixel_data);

  // Escalar a un max_color mayor que 255
  maxlevel(1000, is_16_bit, pixel_span, header);

  // Verificar que los valores no excedan el nuevo max_color
  EXPECT_EQ(pixel_data[0].channels.red, 1000);
  EXPECT_EQ(pixel_data[0].channels.green, 1000);
  EXPECT_EQ(pixel_data[0].channels.blue, 1000);

  // Escalar nuevamente a un max_color pequeño, como 100
  maxlevel(100, is_16_bit, pixel_span, header);

  // Verificar que los valores de los píxeles no excedan el límite de 100
  EXPECT_EQ(pixel_data[0].channels.red, 100);  // El valor original era 255, pero ahora se debe reducir
  EXPECT_EQ(pixel_data[0].channels.green, 100);
  EXPECT_EQ(pixel_data[0].channels.blue, 100);

  // Verificar que los valores se mantienen dentro de los límites establecidos
  EXPECT_EQ(pixel_data[1].channels.red, 0);
  EXPECT_EQ(pixel_data[1].channels.green, 0);
  EXPECT_EQ(pixel_data[1].channels.blue, 0);
}


// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)


