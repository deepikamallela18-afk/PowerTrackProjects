#ifndef DECODE_HPP
#define DECODE_HPP

#include <cstdint>
#include <fstream>
#include <string>
#include "types.hpp"

/*
 * Decodes a secret file from the LSBs of a stego BMP image.
 * Holds info about the stego image and the output file.
 */
class Decoder
{
public:
    /* Read and validate Decode args from the command line */
    Status read_and_validate_args(int argc, char *argv[]);

    /* Perform the decoding */
    Status do_decoding();

    const std::string &output_file_name() const { return output_fname; }

private:
    /* Stego Image Info */
    std::string stego_image_fname;
    std::ifstream stego_image;

    /* Output File Info */
    std::string output_fname;
    std::string extn_output_file;
    std::uint32_t size_extn = 0;
    std::uint32_t size_secret_file = 0;

    /* Open stego image */
    Status open_files();

    /* Skip BMP image header (54 bytes) */
    Status skip_bmp_header();

    /* Decode Magic String */
    Status decode_magic_string(const std::string &magic_string);

    /* Decode extension size */
    Status decode_secret_file_extn_size();

    /* Decode secret file extension */
    Status decode_secret_file_extn();

    /* Decode secret file size */
    Status decode_secret_file_size();

    /* Decode secret file data */
    Status decode_secret_file_data();

    /* Decoding function, which does the real extraction */
    Status decode_data_from_image(char *data, std::size_t size);

    /* Decode a byte from LSB of image data buffer */
    static char decode_byte_from_lsb(const char *image_buffer);

    /* Decode 32-bit integer from LSB */
    Status decode_integer_from_lsb(std::uint32_t &value);
};

#endif
