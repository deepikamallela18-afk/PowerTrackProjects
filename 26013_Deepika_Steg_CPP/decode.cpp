#include <iostream>
#include <vector>
#include "decode.hpp"
#include "common.hpp"

/* Function Definitions */

Status Decoder::read_and_validate_args(int argc, char *argv[])
{
    // Check if stego image has a .bmp extension
    if (file_extension(argv[2]) == ".bmp")
    {
        std::cout << "Received a proper stego bmp file\n";
        stego_image_fname = argv[2];
    }
    else
    {
        std::cout << "Failed to receive proper stego bmp file\n";
        return Status::failure;
    }

    if (argc > 3)
    {
        std::cout << "Received the output file name\n";
        output_fname = argv[3];
    }
    else
    {
        std::cout << "Haven't received the output file name, hence creating it as decoded\n";
        output_fname = "decoded";
    }
    return Status::success;
}

Status Decoder::open_files()
{
    stego_image.open(stego_image_fname, std::ios::binary);
    if (!stego_image)
    {
        std::cerr << "ERROR: Unable to open file " << stego_image_fname << '\n';
        return Status::failure;
    }
    return Status::success;
}

Status Decoder::skip_bmp_header()
{
    stego_image.seekg(BMP_HEADER_SIZE, std::ios::beg);
    return stego_image ? Status::success : Status::failure;
}

char Decoder::decode_byte_from_lsb(const char *image_buffer)
{
    unsigned value = 0;
    for (int i = 0; i < 8; i++)
    {
        value = (value << 1) | (image_buffer[i] & 1);
    }
    return static_cast<char>(value);
}

Status Decoder::decode_data_from_image(char *data, std::size_t size)
{
    char image_buffer[8];
    for (std::size_t i = 0; i < size; i++)
    {
        if (!stego_image.read(image_buffer, sizeof image_buffer))
            return Status::failure;
        data[i] = decode_byte_from_lsb(image_buffer);
    }
    return Status::success;
}

Status Decoder::decode_integer_from_lsb(std::uint32_t &value)
{
    char image_buffer[32];
    if (!stego_image.read(image_buffer, sizeof image_buffer))
        return Status::failure;

    value = 0;
    for (int i = 0; i < 32; i++)
    {
        value = (value << 1) | (image_buffer[i] & 1);
    }
    return Status::success;
}

Status Decoder::decode_magic_string(const std::string &magic_string)
{
    std::string decoded_magic(magic_string.size(), '\0');
    if (decode_data_from_image(&decoded_magic[0], decoded_magic.size()) == Status::failure)
        return Status::failure;

    return decoded_magic == magic_string ? Status::success : Status::failure;
}

Status Decoder::decode_secret_file_extn_size()
{
    if (decode_integer_from_lsb(size_extn) == Status::failure)
        return Status::failure;
    return size_extn <= MAX_FILE_SUFFIX ? Status::success : Status::failure;
}

Status Decoder::decode_secret_file_extn()
{
    extn_output_file.assign(size_extn, '\0');
    return decode_data_from_image(&extn_output_file[0], extn_output_file.size());
}

Status Decoder::decode_secret_file_size()
{
    return decode_integer_from_lsb(size_secret_file);
}

Status Decoder::decode_secret_file_data()
{
    std::vector<char> data(size_secret_file);
    if (decode_data_from_image(data.data(), data.size()) == Status::failure)
        return Status::failure;

    output_fname += extn_output_file;

    std::ofstream output(output_fname, std::ios::binary);
    if (!output)
    {
        std::cerr << "ERROR: Unable to open file " << output_fname << '\n';
        return Status::failure;
    }
    output.write(data.data(), data.size());
    return output ? Status::success : Status::failure;
}

Status Decoder::do_decoding()
{
    std::cout << "Let's open the stego file in the required mode!\n";
    if (open_files() == Status::success)
    {
        std::cout << "Opened the stego file successfully\n";
    }
    else
    {
        std::cout << "Failed to open stego file\n";
        return Status::failure;
    }

    std::cout << "Skipping BMP header...\n";
    if (skip_bmp_header() == Status::success)
    {
        std::cout << "Header skipped successfully!\n";
    }
    else
    {
        std::cout << "Failed to skip BMP header\n";
        return Status::failure;
    }

    std::cout << "Started to decode magic string\n";
    if (decode_magic_string(MAGIC_STRING) == Status::success)
    {
        std::cout << "Done! Magic string matched successfully\n";
    }
    else
    {
        std::cout << "Failed to match magic string\n";
        return Status::failure;
    }

    std::cout << "Decoding secret file extension size...\n";
    if (decode_secret_file_extn_size() == Status::success)
    {
        std::cout << "Decoded extension size successfully!\n";
    }
    else
    {
        std::cout << "Failed to decode extension size\n";
        return Status::failure;
    }

    std::cout << "Decoding secret file extension...\n";
    if (decode_secret_file_extn() == Status::success)
    {
        std::cout << "Decoded secret file extension successfully!\n";
    }
    else
    {
        std::cout << "Failed to decode secret file extension\n";
        return Status::failure;
    }

    std::cout << "Decoding secret file size...\n";
    if (decode_secret_file_size() == Status::success)
    {
        std::cout << "Decoded secret file size successfully!\n";
    }
    else
    {
        std::cout << "Failed to decode secret file size\n";
        return Status::failure;
    }

    std::cout << "Decoding secret file data...\n";
    if (decode_secret_file_data() == Status::success)
    {
        std::cout << "Decoded secret file data and wrote to " << output_fname << " successfully!\n";
    }
    else
    {
        std::cout << "Failed to decode secret file data\n";
        return Status::failure;
    }

    return Status::success;
}
