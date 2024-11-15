#!/bin/bash

INPUT_DIR="inputs"
OUTPUT_DIR="outputs"
EXPOUT_DIR="expected_outputs/cutfreq"
IMTOOL_SOA="./release-build/imtool-aos/imtool-aos"

cd ..
if [ ! -d "release-build" ]; then
    mkdir release-build
    cmake . -DCMAKE_BUILD_TYPE=Release -B release-build
fi

# Función para ejecutar pruebas
run_test() {
  local input_file=$1
  local cutfreq=$2
  local output_file=$3

  echo "Running cutfreq test for $input_file with cutfreq $cutfreq"

  # Leer el header de la imagen
  echo "Reading header for $input_file..."
  infile="$INPUT_DIR/$input_file"
  outfile="$OUTPUT_DIR/$output_file"

  # Ejecutar el programa con los parámetros necesarios
  if ! "$IMTOOL_SOA" "$infile" "$outfile" cutfreq "$cutfreq"; then
    echo "FAIL: Error generating $output_file."
  else
    echo "PASS: $output_file generated successfully."
  fi
}

# Test case para lake-large 100k
echo "Running cutfreq test for lake-large.ppm with cutfreq 100000"
if ! "$IMTOOL_SOA" "$INPUT_DIR/lake-large.ppm" "$OUTPUT_DIR/lake-large100k-AOS.ppm" cutfreq 100000; then
  echo "FAIL: Error generating lake-large100k-AOS.ppm."
else
  echo "PASS: lake-large100k-AOS.ppm generated successfully"
fi
# Test case para lake-large 162k
echo "Running cutfreq test for lake-large.ppm with cutfreq 162000"
if ! "$IMTOOL_SOA" "$INPUT_DIR/lake-large.ppm" "$OUTPUT_DIR/lake-large162k-AOS.ppm" cutfreq 162000; then
  echo "FAIL: Error generating lake-large162k-AOS.ppm."
else
  echo "PASS: lake-large162k-AOS.ppm generated successfully"
fi

echo "Imagenes generadas."
echo ""
echo "Comparando imagenes generadas con las esperadas..."

# Comparar las imágenes generadas
if ! diff "$OUTPUT_DIR/lake-large100k-AOS.ppm" "$EXPOUT_DIR/lake-large-100K.ppm"; then
  echo "FAIL: lake-large100k-AOS.ppm is different from lake-large-100K.ppm."
else
  echo "PASS: lake-large100k"
fi
# Comparar las imágenes generadas
if ! diff "$OUTPUT_DIR/lake-large162k-AOS.ppm" "$EXPOUT_DIR/lake-large162K.ppm"; then
  echo "FAIL: lake-large162k-AOS.ppm is different from lake-large-162K.ppm."
else
  echo "PASS: lake-large162k"
fi

echo "Tests finalizados."