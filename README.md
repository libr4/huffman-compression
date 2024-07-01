## Huffman Compressor and Decompressor

This repository contains C++ implementations of Huffman coding for file compression and decompression. Huffman coding is a popular algorithm used for lossless data compression.

### Instructions to Run the Compressor

Compile:

```bash
g++ compressor.cpp -o compressor
```

Execute:

```bash
./compressor
```

When the program runs, it will print the following message in the terminal:

```bash
"Nome do arquivo com extensao (example.ext):"
```

You should provide the name of a file, along with its extension, in the format "example.ext". For example, to test with the compressor's own source code, enter "compressor.cpp" without quotes. The input file must be in the same directory as the compressor. The output file will have the same name as the input file but with the .huff extension, and it will be written in the same directory as the compressor.

### Instructions to Run the Decompressor

To compile the decompressor, run the command:

```bash
g++ decompressor.cpp -o decompressor
```

To run the decompressor:

```bash
./decompressor
```

Provide the name of the compressed file including the .huff extension (e.g., example.huff). Both the compressed file and its corresponding .metadata file must be in the same directory as the decompressor. The decompressed output file (input_filenameDescomp.extension) will be saved in the same directory.