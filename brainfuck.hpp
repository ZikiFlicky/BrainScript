#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <sstream>

#define BF_DEFAULT_TAPE_SIZE 128

namespace BrainFuck {

class Generator;

class Instruction {
public:
    Instruction() {}
    virtual ~Instruction() {}
    virtual void write(Generator &generator) = 0;
};

class PutCharInstruction : public Instruction {
public:
    PutCharInstruction() {}
    ~PutCharInstruction() {}
    void write(Generator &generator);
};

class GetCharInstruction : public Instruction {
public:
    GetCharInstruction() {}
    ~GetCharInstruction() {}
    void write(Generator &generator);
};

class IncInstruction : public Instruction {
public:
    IncInstruction() {}
    ~IncInstruction() {}
    void write(Generator &generator);
};

class DecInstruction : public Instruction {
public:
    DecInstruction() {}
    ~DecInstruction() {}
    void write(Generator &generator);
};

class ShlInstruction : public Instruction {
public:
    ShlInstruction() {}
    ~ShlInstruction() {}
    void write(Generator &generator);
};

class ShrInstruction : public Instruction {
public:
    ShrInstruction() {}
    ~ShrInstruction() {}
    void write(Generator &generator);
};

class LoopInstruction : public Instruction {
public:
    LoopInstruction(std::vector<Instruction*> instructions) : m_instructions(instructions) {}
    ~LoopInstruction() {
        for (Instruction *instruction : m_instructions) {
            delete instruction;
        }
    }
    void write(Generator &generator);

private:
    std::vector<Instruction*> m_instructions;
};

enum TokenType {
    Comma = 1,
    Dot,
    LeftBracket,
    RightBracket,
    LessThan,
    GreaterThan,
    Plus,
    Minus
};

class Lexer {
public:
    Lexer(std::ifstream &stream) : m_stream(stream) {}

    size_t get_position();
    void set_position(size_t position);
    bool lex(TokenType &out);

private:
    bool read();

    char m_character;
    std::ifstream &m_stream;
};

class Parser {
public:
    Parser(std::ifstream &stream) : m_stream(stream) {
        m_lexer = new Lexer(stream);
    }
    ~Parser() {
        delete m_lexer;
    }

    std::string *get_error();
    void set_error(std::string message);

    void parse_all();
    std::vector<Instruction*> get_instructions();

private:
    bool lex();
    bool expect_token(TokenType token);
    bool parse_instruction(Instruction* &instruction);
    Lexer *m_lexer;
    TokenType m_token;
    std::ifstream &m_stream;
    std::vector<Instruction*> m_instructions;

    bool m_has_error = false;
    std::string m_error_message;
};

class Generator {
public:
    Generator(std::vector<Instruction*> instructions, size_t tape_size) : m_instructions(instructions), m_tape_size(tape_size) {
        m_stream = std::stringstream();
    }

    void write_line(std::string line);

    void increase_indent();
    void decrease_indent();

    void write_instruction(Instruction *instruction);
    void write_all();

    std::string get_generated();
private:

    std::vector<Instruction*> m_instructions;
    size_t m_tape_size;
    size_t m_indent = 1; // Start with indent 1
    std::stringstream m_stream;

    void write_string(std::string string);
    void write_indent();
    void write_all_instructions();
};

}
