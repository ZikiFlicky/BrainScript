#include "brainfuck.hpp"

namespace BrainFuck {

const std::string generation_string1 = \
R"(const prompt = require('prompt-sync')({sigint: true});
const TapeSize = )";

const std::string generation_string2 = \
R"(;
let inputBuffer = ""
let tape = new Array(TapeSize);
let index = 0;

function initializeTape() {
    for (let i = 0; i < TapeSize; ++i)
        tape[i] = 0;
}

function runInstructionGetChar() {
    let character;
    // Loop when there aren't characters in the buffer to emulate the getchar function
    while (inputBuffer.length === 0)
        inputBuffer = prompt("") + "\n";
    character = inputBuffer[0];
    inputBuffer = inputBuffer.substring(1);
    // Get the numeric value of the character and validate it
    characterNumeric = character.charCodeAt(0);
    if (characterNumeric < 0 || characterNumeric > 255)
        throw Error("Character not ascii");
    // Set the character
    tape[index] = characterNumeric;
}

function runInstructionPutChar() {
    process.stdout.write(String.fromCharCode(tape[index]));
}

function runInstructionInc() {
    let current = tape[index];
    // Decide whether to wraparound
    tape[index] = (current + 1) % 256;
}

function runInstructionDec() {
    let current = tape[index];
    // Decide whether to wraparound
    tape[index] = ((current - 1) % 256 + 256) % 256;
}

function runInstructionShl() {
    if (index == 0) {
        throw Error("Cannot move left when on the leftmost cell");
    }
    --index;
}

function runInstructionShr() {
    if (index == TapeSize - 1) {
        throw Error("Cannot move right when on the rightmost cell");
    }
    ++index;
}

function main() {
    initializeTape();
    // The generated code
)";

const std::string generation_string3 = \
R"(}

main();
process.exit();
)";

bool Lexer::read() {
    if (m_stream.read(&m_character, 1))
        return true;
    else
        return false;
}

size_t Lexer::get_position() {
    return m_stream.tellg();
}

void Lexer::set_position(size_t position) {
    m_stream.seekg(position);
}

bool Lexer::lex(TokenType &out) {
    bool found = false;

    while (!found) {
        if (!read())
            return false;
        // Until proven not found
        found = true;
        switch (m_character) {
        case '+':
            out = TokenType::Plus;
            break;
        case '-':
            out = TokenType::Minus;
            break;
        case '[':
            out = TokenType::LeftBracket;
            break;
        case ']':
            out = TokenType::RightBracket;
            break;
        case '<':
            out = TokenType::LessThan;
            break;
        case '>':
            out = TokenType::GreaterThan;
            break;
        case ',':
            out = TokenType::Comma;
            break;
        case '.':
            out = TokenType::Dot;
            break;
        default:
            found = false;
        }
    }

    return true;
}

bool Parser::lex() {
    return m_lexer->lex(m_token);
}

bool Parser::expect_token(TokenType token) {
    size_t backtrack = m_lexer->get_position();

    if (!lex())
        return false;

    if (m_token == token)
        return true;

    m_lexer->set_position(backtrack);
    return false;
}

bool Parser::parse_instruction(Instruction* &instruction) {
    if (!lex())
        return false;
    switch (m_token) {
    case TokenType::Comma:
        instruction = (Instruction*)new GetCharInstruction();
        break;
    case TokenType::Dot:
        instruction = (Instruction*)new PutCharInstruction();
        break;
    case TokenType::Plus:
        instruction = (Instruction*)new IncInstruction();
        break;
    case TokenType::Minus:
        instruction = (Instruction*)new DecInstruction();
        break;
    case TokenType::LessThan:
        instruction = (Instruction*)new ShlInstruction();
        break;
    case TokenType::GreaterThan:
        instruction = (Instruction*)new ShrInstruction();
        break;
    case TokenType::LeftBracket: {
        auto instructions = std::vector<Instruction*>();

        for (;;) {
            Instruction *inner_instruction;

            if (expect_token(TokenType::RightBracket))
                break;

            if (!parse_instruction(inner_instruction)) {
                if (!m_has_error)
                    set_error("Expected instruction or ']'");
                return false;
            }

            instructions.push_back(inner_instruction);
        }

        instruction = (Instruction*)new LoopInstruction(instructions);
        break;
    }
    case TokenType::RightBracket:
        set_error("Unexpected ']'");
        return false;
    default:
        assert(0);
    }

    return true;
}

std::string *Parser::get_error() {
    if (m_has_error)
        return &m_error_message;
    return nullptr;
}

void Parser::set_error(std::string message) {
    m_has_error = true;
    m_error_message = message;
}

void Parser::parse_all() {
    Instruction *instruction;
    m_instructions = std::vector<Instruction*>();
    for (;;) {
        if (!parse_instruction(instruction))
            break;
        m_instructions.push_back(instruction);
    }
}

std::vector<Instruction*> Parser::get_instructions() {
    return m_instructions;
}

void PutCharInstruction::write(Generator &generator) {
    generator.write_line("runInstructionPutChar();");
}

void GetCharInstruction::write(Generator &generator) {
    generator.write_line("runInstructionGetChar();");
}

void IncInstruction::write(Generator &generator) {
    generator.write_line("runInstructionInc();");
}

void DecInstruction::write(Generator &generator) {
    generator.write_line("runInstructionDec();");
}

void ShlInstruction::write(Generator &generator) {
    generator.write_line("runInstructionShl();");
}

void ShrInstruction::write(Generator &generator) {
    generator.write_line("runInstructionShr();");
}

void LoopInstruction::write(Generator &generator) {
    generator.write_line("while (tape[index]) {");
    generator.increase_indent();
    for (Instruction* &instruction : m_instructions) {
        generator.write_instruction(instruction);
    }
    generator.decrease_indent();
    generator.write_line("}");
}

void Generator::write_string(std::string string) {
    m_stream.write(string.data(), string.length());
}

void Generator::write_indent() {
    for (size_t i = 0; i < 4 * m_indent; ++i)
        m_stream.write(" ", 1);
}

void Generator::write_line(std::string line) {
    write_indent(); // Indentation
    write_string(line); // Actual line
    m_stream.write("\n", 1); // Newline
}

void Generator::increase_indent() {
    ++m_indent;
}

void Generator::decrease_indent() {
    --m_indent;
}

void Generator::write_instruction(Instruction *instruction) {
    instruction->write(*this);
}

void Generator::write_all_instructions() {
    for (auto* instruction : m_instructions) {
        write_instruction(instruction);
    }
}

void Generator::write_all() {
    // Write until tape size
    write_string(generation_string1);
    // Write tape size
    auto tape_size_string = std::to_string(m_tape_size);
    write_string(tape_size_string);
    // Write until generated code
    write_string(generation_string2);
    // Write generated code
    write_all_instructions();
    // Write until end of file
    write_string(generation_string3);
}

std::string Generator::get_generated() {
    return m_stream.str();
}

}
