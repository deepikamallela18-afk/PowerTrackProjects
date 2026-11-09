#ifndef ENCODE_HPP
#define ENCODE_HPP

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "types.hpp"

/*
 * Encodes a secret file into the LSBs of a 24-bit BMP image.
 * Holds info about the source image, the secret file and the
 * output (stego) image.
 */
class Encoder
{
public:
    /* Read and validate Encode args from the command line */
    Status read_and_validate_args(int argc, char *argv[]);

    /* Perform the encoding */
    Status do_encoding();

private:
    /* Source Image info */
    std::string src_image_fname;
    std::ifstream src_image;
    std::uint32_t image_capacity = 0;

    /* Secret File Info */
    std::string secret_fname;
    std::ifstream secret;
    std::string extn_secret_file;
    std::uint32_t size_secret_file = 0;

    /* Stego Image Info */
    std::string stego_image_fname;
    std::ofstream stego_image;

    /* Open i/p and o/p files */
    Status open_files();

    /* Check that the image can hold the secret */
    Status check_capacity();

    /* Get image size (width * height * 3) */
    std::uint32_t get_image_size_for_bmp();

    /* Get file size */
    static std::uint32_t get_file_size(std::ifstream &file);

    /* Copy bmp image header */
    Status copy_bmp_header();

    /* Store Magic String */
    Status encode_magic_string(const std::string &magic_string);

    /* Encode extension size */
    Status encode_extension_size(std::uint32_t size);

    /* Encode secret file extension */
    Status encode_secret_file_extn(const std::string &file_extn);

    /* Encode secret file size */
    Status encode_secret_file_size(std::uint32_t file_size);

    /* Encode secret file data */
    Status encode_secret_file_data();

    /* Encode function, which does the real encoding */
    Status encode_data_to_image(const char *data, std::size_t size);

    /* Encode a byte into LSB of image data array */
    static void encode_byte_to_lsb(char data, char *image_buffer);

    /* Encode 32-bit integer to LSB */
    Status encode_integer_to_lsb(std::uint32_t value);

    /* Copy remaining image bytes from src to stego image after encoding */
    Status copy_remaining_img_data();
};

#endif
