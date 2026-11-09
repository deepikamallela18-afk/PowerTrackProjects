#include <iostream>
#include "encode.hpp"
#include "common.hpp"

/* Function Definitions */

Status Encoder::read_and_validate_args(int argc, char *argv[])
{
    if (file_extension(argv[2]) == ".bmp")
    {
        std::cout << "Received a proper bmp file\n";
        src_image_fname = argv[2];
    }
    else
    {
        std::cout << "Failed to receive proper bmp file\n";
        return Status::failure;
    }

    if (file_extension(argv[3]) == ".txt")
    {
        std::cout << "Received a proper txt file\n";
        secret_fname = argv[3];
    }
    else
    {
        std::cout << "Failed to receive proper txt file\n";
        return Status::failure;
    }

    if (argc > 4)
    {
        std::cout << "Received the output file name\n";
        stego_image_fname = argv[4];
    }
    else
    {
        std::cout << "Haven't received the output file name, hence creating it as stego.bmp\n";
        stego_image_fname = "stego.bmp";
    }
    return Status::success;
}

/*
 * Get File streams for i/p and o/p files
 * Return Value: success, or failure on file errors
 */
Status Encoder::open_files()
{
    src_image.open(src_image_fname, std::ios::binary);
    if (!src_image)
    {
        std::cerr << "ERROR: Unable to open file " << src_image_fname << '\n';
        return Status::failure;
    }

    secret.open(secret_fname, std::ios::binary);
    if (!secret)
    {
        std::cerr << "ERROR: Unable to open file " << secret_fname << '\n';
        return Status::failure;
    }

    stego_image.open(stego_image_fname, std::ios::binary);
    if (!stego_image)
    {
        std::cerr << "ERROR: Unable to open file " << stego_image_fname << '\n';
        return Status::failure;
    }

    return Status::success;
}

/* Get image size
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes each, little endian
 */
std::uint32_t Encoder::get_image_size_for_bmp()
{
    unsigned char buf[8] = {};
    src_image.seekg(18, std::ios::beg);
    src_image.read(reinterpret_cast<char *>(buf), sizeof buf);

    std::uint32_t width = buf[0] | buf[1] << 8 | buf[2] << 16 | std::uint32_t(buf[3]) << 24;
    std::uint32_t height = buf[4] | buf[5] << 8 | buf[6] << 16 | std::uint32_t(buf[7]) << 24;
    std::cout << "width = " << width << '\n';
    std::cout << "height = " << height << '\n';

    return width * height * 3;
}

std::uint32_t Encoder::get_file_size(std::ifstream &file)
{
    file.seekg(0, std::ios::end);
    return static_cast<std::uint32_t>(file.tellg());
}

Status Encoder::check_capacity()
{
    image_capacity = get_image_size_for_bmp();
    size_secret_file = get_file_size(secret);

    // magic + ext size + ext + file size + data, 8 image bytes per secret byte
    std::uint64_t needed = (MAGIC_STRING.size() + 4 + extn_secret_file.size() + 4 +
                            std::uint64_t(size_secret_file)) * 8;
    return image_capacity > needed ? Status::success : Status::failure;
}

Status Encoder::copy_bmp_header()
{
    char header[BMP_HEADER_SIZE];
    src_image.seekg(0, std::ios::beg);
    if (!src_image.read(header, sizeof header))
        return Status::failure;
    stego_image.write(header, sizeof header);
    return stego_image ? Status::success : Status::failure;
}

Status Encoder::encode_magic_string(const std::string &magic_string)
{
    return encode_data_to_image(magic_string.data(), magic_string.size());
}

Status Encoder::encode_data_to_image(const char *data, std::size_t size)
{
    char image_data[8];
    for (std::size_t i = 0; i < size; i++)
    {
        if (!src_image.read(image_data, sizeof image_data))
            return Status::failure;
        encode_byte_to_lsb(data[i], image_data);
        stego_image.write(image_data, sizeof image_data);
    }
    return stego_image ? Status::success : Status::failure;
}

void Encoder::encode_byte_to_lsb(char data, char *image_buffer)
{
    auto byte = static_cast<unsigned char>(data);
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = static_cast<char>((image_buffer[i] & 0xFE) | ((byte >> (7 - i)) & 1));
    }
}

Status Encoder::encode_extension_size(std::uint32_t size)
{
    return encode_integer_to_lsb(size);
}

Status Encoder::encode_integer_to_lsb(std::uint32_t value)
{
    char buf[32];
    if (!src_image.read(buf, sizeof buf))
        return Status::failure;
    // encode 32 bits in 32 bytes, MSB first
    for (int i = 0; i < 32; i++)
    {
        buf[i] = static_cast<char>((buf[i] & 0xFE) | ((value >> (31 - i)) & 1));
    }
    stego_image.write(buf, sizeof buf);
    return stego_image ? Status::success : Status::failure;
}

Status Encoder::encode_secret_file_extn(const std::string &file_extn)
{
    return encode_data_to_image(file_extn.data(), file_extn.size());
}

Status Encoder::encode_secret_file_size(std::uint32_t file_size)
{
    return encode_integer_to_lsb(file_size);
}

Status Encoder::encode_secret_file_data()
{
    std::vector<char> data(size_secret_file);
    secret.clear();
    secret.seekg(0, std::ios::beg);
    if (!secret.read(data.data(), data.size()))
        return Status::failure;
    return encode_data_to_image(data.data(), data.size());
}

Status Encoder::copy_remaining_img_data()
{
    stego_image << src_image.rdbuf();
    return stego_image ? Status::success : Status::failure;
}

Status Encoder::do_encoding()
{
    std::cout << "Let's open the files in the required mode!\n";
    if (open_files() == Status::success)
    {
        std::cout << "Opened the required files in required mode successfully\n";
    }
    else
    {
        std::cout << "Failed to open files\n";
        return Status::failure;
    }

    extn_secret_file = file_extension(secret_fname);
    if (extn_secret_file.size() > MAX_FILE_SUFFIX)
    {
        std::cout << "Secret file extension is too long\n";
        return Status::failure;
    }

    std::cout << "Check if " << src_image_fname << " file is suitable for encoding\n";
    if (check_capacity() == Status::success)
    {
        std::cout << "Yes, " << src_image_fname << " file is suitable for encoding\n";
    }
    else
    {
        std::cout << "No, " << src_image_fname << " file is not suitable for encoding\n";
        return Status::failure;
    }

    std::cout << "Copying the header from " << src_image_fname << " to " << stego_image_fname << " as it is\n";
    if (copy_bmp_header() == Status::success)
    {
        std::cout << "Done, copied!\n";
    }
    else
    {
        std::cout << "Failed, couldn't copy.\n";
        return Status::failure;
    }

    std::cout << "Started to encode magic string \n";
    if (encode_magic_string(MAGIC_STRING) == Status::success)
    {
        std::cout << "Done! Completed encoding magic string \n";
    }
    else
    {
        std::cout << "Failed to encode magic string. \n";
        return Status::failure;
    }

    if (encode_extension_size(extn_secret_file.size()) == Status::success)
    {
        std::cout << "Encoded secret file extension size successfully!\n";
    }
    else
    {
        std::cout << "Failed to encode extension size\n";
        return Status::failure;
    }

    if (encode_secret_file_extn(extn_secret_file) == Status::success)
    {
        std::cout << "Encoded secret file extension successfully! \n";
    }
    else
    {
        std::cout << "Failed to encode secret file extension. \n";
        return Status::failure;
    }

    if (encode_secret_file_size(size_secret_file) == Status::success)
    {
        std::cout << "Encoded secret file size successfully! \n";
    }
    else
    {
        std::cout << "Failed to encode secret file size. \n";
        return Status::failure;
    }

    if (encode_secret_file_data() == Status::success)
    {
        std::cout << "Encoded secret file data successfully! \n";
    }
    else
    {
        std::cout << "Failed to encode secret file data. \n";
        return Status::failure;
    }

    if (copy_remaining_img_data() == Status::success)
    {
        std::cout << "Copied remaining image data successfully! \n";
    }
    else
    {
        std::cout << "Failed to copy remaining image data. \n";
        return Status::failure;
    }
    return Status::success;
}
