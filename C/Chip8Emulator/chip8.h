#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <functional>

typedef struct chip8{

    using Instruction = std::function<void(void)>;
    std::unordered_map<uint16_t, Instruction> instructions;

    uint16_t current_opcode;

    // 4096 bytes of memory
    //
    // **Memory map:
    //   0x000-0x1FF - Chip 8 interpreter (contains font set in emu)   [bytes 0-511]
    //   0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)  [bytes 80-160]
    //   0x200-0xFFF - Program ROM and work RAM (most programs)        [bytes 512-4095]
    //
    uint8_t memory[4096];

    // 16 8-bit registers (Vx)
    uint8_t V[16];

    // Registers 
    uint16_t I; // Stores memory addresses (rightmost bits used)
    uint16_t pc;

    // Graphics display
    uint8_t gfx[64 * 32];

    // Timers
    // **When above 0, timers immediately decrement at rate of 60Hz
    uint8_t delay_timer;
    uint8_t sound_timer;

    // Stack & pointer
    uint16_t stack[16];
    uint16_t sp;

    // Keypad
    uint8_t key[16];

    //----------------------------------------------Functions----------------------------------------------

    void initialize(const std::string& ROM){
        pc             = 0x200;  // Program counter starts at 0x200
        current_opcode = 0;      // Reset current opcode	
        I              = 0;      // Reset index register
        sp             = 0;      // Reset stack pointer
        delay_timer    = 0;      // Reset delay timer
        sound_timer    = 0;      // Reset sound timer
        
        // Clear display
        // for(int i = 0; i < 64; i++){
        //      std::memset(gfx[i], 0, 32);
        // }

        // Clear memory
        std::memset(memory, 0, 4096);

        // Clear registers V0-VF
        std::memset(V, 0, 16);

        // Clear stack
        std::memset(stack, 0, 16);

        // Font set
        unsigned char chip8_fontset[80] =
        { 
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };
        
        // Load fontset into memory space
        for(int i = 0; i < 80; ++i){
            memory[i] = chip8_fontset[i];
        }

        // Reset timers
        delay_timer = 0;
        sound_timer = 0;

        // Load the ROM into program memory.
        std::ifstream input("ff.txt", std::ios::in | std::ios::binary);
        std::vector<uint8_t> bytes(
                (std::istreambuf_iterator<char>(input)),
                (std::istreambuf_iterator<char>()));
        if (bytes.size() > 4096) {
            throw std::runtime_error("File size is bigger than max rom size.");
        } else if (bytes.size() <= 0) {
            throw std::runtime_error("No file or empty file.");
        }
        std::memcpy(memory + 0x200, bytes.data(), bytes.size());

        build_instruction_set();
    };

    void emulate_cycle()
    {
        // Read in the big-endian opcode word.
        current_opcode = memory[pc] << 8 |
            memory[pc + 1];

        auto instr = instructions.find(current_opcode);
        if (instr != instructions.end()) {
            instr->second();
        } else {
            throw std::runtime_error("Couldn't find instruction for opcode " +
            std::to_string(current_opcode));
        }

        // TODO: Update sound and delay timers. 
    };

    void build_instruction_set(){

        // Prepping instructions
        instructions.clear();
        instructions.reserve(0xFFFF);

        // 
        instructions[0x00E0] = [this]() {
            frame.SetAll(0);
            pc += 2;
        };

        // 
        instructions[0x00EE] = [this]() {
            pc = stack[--sp] + 2;
        };

        for(int opcode = 0x1000; opcode < 0xFFFF; opcode++){

            uint16_t nnn =  opcode & 0x0FFF;
            uint8_t n =     opcode & 0x000F;
            uint8_t x =     (opcode & 0x0F00) >> 8;
            uint8_t y =     (opcode & 0x00F0) >> 4;
            uint8_t kk =    opcode & 0x00FF;

            if ((opcode & 0xF000) == 0x1000) {
                instructions[opcode] = GenJP(nnn);
            } else if ((opcode & 0xF000) == 0x2000) {
                instructions[opcode] = GenCALL(nnn);
            }
        }
    };

    Instruction GenJP(uint16_t addr) {
        return [this, addr]() {  pc = addr; };
    };

    Instruction GenCALL(uint16_t addr) {
        return [this, addr]() {
            stack[sp++] = pc;
            pc = addr;
        };
    };

    Instruction GenSE(uint8_t reg, uint8_t val) {
        return [this, reg, val]() {
            V[reg] == val ? (pc += 4) : (pc += 2);
        };
    };

    Instruction GenADD(uint8_t reg_x, uint8_t reg_y) {
        return [this, reg_x, reg_y]() {
            uint16_t res = V[reg_x] += V[reg_y];
            V[0xF] = res > 0xFF; // set carry flag
            V[reg_x] = res;
            pc += 2;
        };
    };

    Instruction GenSUB(uint8_t reg_x, uint8_t reg_y) {
        return [this, reg_x, reg_y]() {
            V[0xF] = V[reg_x] > V[reg_y]; // set not borrow flag
            V[reg_x] -= V[reg_y];
            pc += 2;
        };
    };

    Instruction GenLDSPRITE(uint8_t reg) {
        return [this, reg]() {
            uint8_t digit = V[reg];
            I = 0x50 + (5 * digit);
            pc += 2;
        };
    }

    Instruction GenSTREG(uint8_t reg) {
        return [this, reg]() {
            for (uint8_t v = 0; v <= reg; v++) {
                memory[I + v] = V[v];
            }
            pc += 2;
        };
    }

    Instruction GenLDREG(uint8_t reg) {
        return [this, reg]() {
            for (uint8_t v = 0; v <= reg; v++) {
                V[v] = memory[I + v];
            }
            pc += 2;
        };
    }

} chip8;