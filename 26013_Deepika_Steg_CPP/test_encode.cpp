#include <iostream>
#include <string>
#include "encode.hpp"
#include "decode.hpp"
#include "types.hpp"

static void print_usage()
{
    std::cout << "For encoding: ./stego -e beautiful.bmp secret.txt [stego.bmp]\n";
    std::cout << "For decoding: ./stego -d stego.bmp [output]\n";
}

static OperationType check_operation_type(const std::string &option)
{
    if (option == "-e") // user has selected encoding
        return OperationType::encode;
    if (option == "-d") // user has selected decoding
        return OperationType::decode;
    return OperationType::unsupported;
}

int main(int argc, char *argv[])
{
    if (argc < 3) // validating minimum no. of arguments
    {
        std::cout << "Invalid Input\nPlease pass :  \n";
        print_usage();
        return 1;
    }

    std::cout << "Let's see what you've selected!\n";
    switch (check_operation_type(argv[1]))
    {
    case OperationType::encode:
    {
        if (argc < 4)
        {
            std::cout << "Error: Insufficient arguments given for encoding.\n";
            print_usage();
            return 1;
        }
        std::cout << "Hey, you have selected encoding!\n";
        Encoder steganography;
        if (steganography.read_and_validate_args(argc, argv) != Status::success)
        {
            std::cout << "Failed to validate encode arguments\n";
            return 1;
        }
        std::cout << "Read and validated encode arguments successfully\n";
        std::cout << "------Let us begin encoding------\n";
        if (steganography.do_encoding() != Status::success)
        {
            std::cout << "Failed to perform encoding\n";
            return 1;
        }
        std::cout << "Completed encoding successfully!\n";
        break;
    }
    case OperationType::decode:
    {
        std::cout << "Hey, you have selected decoding!\n";
        Decoder decoding;
        if (decoding.read_and_validate_args(argc, argv) != Status::success)
        {
            std::cout << "Failed to validate decode arguments\n";
            return 1;
        }
        std::cout << "Read and validated decode arguments successfully\n";
        std::cout << "------Let us begin decoding------\n";
        if (decoding.do_decoding() != Status::success)
        {
            std::cout << "Failed to perform decoding\n";
            return 1;
        }
        std::cout << "Completed decoding successfully!\n";
        break;
    }
    default:
        std::cout << "Invalid Input\nPlease pass :  \n";
        print_usage();
        return 1;
    }

    return 0;
}
