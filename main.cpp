#include "brainfuck.hpp"

int main(int argc, char **argv) {
    size_t tape_size;

    if (argc < 3) {
        fprintf(stderr, "Not enough arguments\n");
        return 1;
    }

    auto read_file = std::ifstream(argv[1]);
    if (!read_file) {
        fprintf(stderr, "Could not open file '%s' for reading\n", argv[1]);
        return 1;
    }

    auto write_file = std::ofstream(argv[2], std::ios_base::out | std::ios_base::trunc);
    if (!write_file) {
        read_file.close();
        fprintf(stderr, "Could not open file '%s' for writing\n", argv[2]);
        return 1;
    }

    // If there's an additional argument
    if (argc > 3) {
        char *end;
        long value = std::strtol(argv[3], &end, 10);
        // If failed
        if (end == argv[3] || value < 1) {
            read_file.close();
            write_file.close();
            fprintf(stderr, "Expected number bigger than 0\n");
            return 1;
        }
        tape_size = (size_t)value;
    } else {
        tape_size = BF_DEFAULT_TAPE_SIZE; // Default tape size
    }

    auto parser = BrainFuck::Parser(read_file);
    parser.parse_all();
    read_file.close();

    std::string *error_message = parser.get_error();
    if (error_message) {
        read_file.close();
        write_file.close();
        fprintf(stderr, "ParsingError: %*.s\n", (int)error_message->length(), error_message->data());
        return 1;
    }

    auto instructions = parser.get_instructions();

    // Generate the JavaScript code
    auto generator = BrainFuck::Generator(instructions, tape_size);
    generator.write_all();

    // Write to the output file
    auto generated_string = generator.get_generated();
    write_file.write(generated_string.data(), generated_string.length());
    write_file.close();

    for (auto *instruction : instructions) {
        delete instruction;
    }

    return 0;
}
