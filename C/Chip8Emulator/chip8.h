#pragma once

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <chrono>
#include <array>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

struct chip8 {
    static constexpr uint16_t MEMORY_SIZE = 4096;
    static constexpr uint16_t ROM_START = 0x200;
    static constexpr uint16_t FONT_START = 0x50;
    static constexpr uint8_t SCREEN_WIDTH = 64;
    static constexpr uint8_t SCREEN_HEIGHT = 32;
    static constexpr int PIXEL_SCALE = 12; // unused without SDL; kept for compatibility

    uint16_t current_opcode = 0;

    // 4096 bytes of memory
    uint8_t memory[MEMORY_SIZE]{};

    // 16 8-bit registers (Vx)
    uint8_t V[16]{};

    // Registers
    uint16_t I = 0;
    uint16_t pc = ROM_START;

    // Graphics display (0 or 1 per pixel)
    uint8_t gfx[SCREEN_WIDTH * SCREEN_HEIGHT]{};
    bool draw_flag = false;

    // Timers (decrement at 60Hz in a real loop)
    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;

    // Stack & pointer
    uint16_t stack[16]{};
    uint16_t sp = 0;

    // Keypad state
    uint8_t key[16]{};

    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<uint16_t> random_byte{0, 255};

    // Console input/render support (no SDL).
    bool console_render_enabled = true;

    bool terminal_input_inited = false;
#ifndef _WIN32
    termios original_termios{};
#endif

    // Key state is "momentary" (set for one poll tick). Terminals don't provide key-up events.
    std::chrono::steady_clock::time_point last_draw_ts = std::chrono::steady_clock::now();

    ~chip8() {
        shutdownTerminalInput();
    }

    bool initTerminalInput() {
        if (terminal_input_inited) {
            return true;
        }

#ifdef _WIN32
        terminal_input_inited = true;
        return true;
#else
        if (tcgetattr(STDIN_FILENO, &original_termios) != 0) {
            std::perror("tcgetattr");
            return false;
        }

        termios raw = original_termios;
        raw.c_lflag = static_cast<tcflag_t>(raw.c_lflag & ~(ICANON | ECHO));
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) {
            std::perror("tcsetattr");
            return false;
        }

        terminal_input_inited = true;
        return true;
#endif
    }

    void shutdownTerminalInput() {
        if (!terminal_input_inited) {
            return;
        }

#ifdef _WIN32
        terminal_input_inited = false;
#else
        tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
        terminal_input_inited = false;
#endif
    }

    void initialize(const std::string& rom_path) {
        pc = ROM_START;
        current_opcode = 0;
        I = 0;
        sp = 0;
        delay_timer = 0;
        sound_timer = 0;
        draw_flag = false;

        std::memset(memory, 0, sizeof(memory));
        std::memset(V, 0, sizeof(V));
        std::memset(gfx, 0, sizeof(gfx));
        std::memset(stack, 0, sizeof(stack));
        std::memset(key, 0, sizeof(key));

        const uint8_t chip8_fontset[80] = {
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

        for (int i = 0; i < 80; ++i) {
            memory[FONT_START + i] = chip8_fontset[i];
        }

        std::ifstream input(rom_path, std::ios::binary);
        if (!input) {
            throw std::runtime_error("Failed to open ROM: " + rom_path);
        }

        std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
        if (bytes.empty()) {
            throw std::runtime_error("ROM is empty: " + rom_path);
        }
        if (bytes.size() > (MEMORY_SIZE - ROM_START)) {
            throw std::runtime_error("ROM is too large for CHIP-8 memory.");
        }

        std::memcpy(memory + ROM_START, bytes.data(), bytes.size());
    }

    void emulateCycle() {
        // Fetch opcode
        if (pc + 1 >= MEMORY_SIZE) {
            throw std::runtime_error("Program counter out of bounds");
        }
        current_opcode = static_cast<uint16_t>(memory[pc] << 8) | memory[pc + 1];

        const uint8_t x = static_cast<uint8_t>((current_opcode & 0x0F00u) >> 8);
        const uint8_t y = static_cast<uint8_t>((current_opcode & 0x00F0u) >> 4);
        const uint8_t n = static_cast<uint8_t>(current_opcode & 0x000Fu);
        const uint8_t kk = static_cast<uint8_t>(current_opcode & 0x00FFu);
        const uint16_t nnn = static_cast<uint16_t>(current_opcode & 0x0FFFu);

        switch (current_opcode & 0xF000u) {
        case 0x0000:
            if (current_opcode == 0x00E0 /* CLS */) {
                std::memset(gfx, 0, sizeof(gfx));
                draw_flag = true;
                pc += 2;
            } else if (current_opcode == 0x00EE /* RET */) {
                if (sp == 0) {
                    throw std::runtime_error("Stack underflow on RET");
                }
                --sp;
                pc = static_cast<uint16_t>(stack[sp] + 2);
            } else {
                // 0nnn (SYS) is ignored on modern interpreters
                pc += 2;
            }
            break;

        case 0x1000: // 1nnn (JP)
            pc = nnn;
            break;

        case 0x2000: // 2nnn (CALL)
            if (sp >= 16) {
                throw std::runtime_error("Stack overflow on CALL");
            }
            stack[sp] = pc;
            ++sp;
            pc = nnn;
            break;

        case 0x3000: // 3xkk (SE Vx, byte)
            pc += (V[x] == kk) ? 4 : 2;
            break;

        case 0x4000: // 4xkk (SNE Vx, byte)
            pc += (V[x] != kk) ? 4 : 2;
            break;

        case 0x5000: // 5xy0 (SE Vx, Vy)
            if (n != 0) {
                throw std::runtime_error("Unknown opcode: " + std::to_string(current_opcode));
            }
            pc += (V[x] == V[y]) ? 4 : 2;
            break;

        case 0x6000: // 6xkk (LD Vx, byte)
            V[x] = kk;
            pc += 2;
            break;

        case 0x7000: // 7xkk (ADD Vx, byte)
            V[x] = static_cast<uint8_t>(V[x] + kk);
            pc += 2;
            break;

        case 0x8000:
            switch (n) {
            case 0x0: // 8xy0 LD Vx, Vy
                V[x] = V[y];
                break;
            case 0x1: // 8xy1 OR Vx, Vy
                V[x] = static_cast<uint8_t>(V[x] | V[y]);
                break;
            case 0x2: // 8xy2 AND Vx, Vy
                V[x] = static_cast<uint8_t>(V[x] & V[y]);
                break;
            case 0x3: // 8xy3 XOR Vx, Vy
                V[x] = static_cast<uint8_t>(V[x] ^ V[y]);
                break;
            case 0x4: { // 8xy4 ADD Vx, Vy with carry
                const uint16_t sum = static_cast<uint16_t>(V[x]) + static_cast<uint16_t>(V[y]);
                V[0xF] = (sum > 0xFFu) ? 1 : 0;
                V[x] = static_cast<uint8_t>(sum & 0xFFu);
                break;
            }
            case 0x5: // 8xy5 SUB Vx, Vy with borrow
                V[0xF] = (V[x] >= V[y]) ? 1 : 0;
                V[x] = static_cast<uint8_t>(V[x] - V[y]);
                break;
            case 0x6: // 8xy6 (shift VX)
                V[0xF] = static_cast<uint8_t>(V[x] & 0x01u);
                V[x] = static_cast<uint8_t>(V[x] >> 1);
                break;
            case 0x7: // 8xy7 SUBN VY from VX
                V[0xF] = (V[y] >= V[x]) ? 1 : 0;
                V[x] = static_cast<uint8_t>(V[y] - V[x]);
                break;
            case 0xE: // 8xyE (shift VX)
                V[0xF] = static_cast<uint8_t>((V[x] & 0x80u) >> 7);
                V[x] = static_cast<uint8_t>(V[x] << 1);
                break;
            default:
                throw std::runtime_error("Unknown opcode: " + std::to_string(current_opcode));
            }
            pc += 2;
            break;

        case 0x9000: // 9xy0 (SNE Vx, Vy)
            if (n != 0) {
                throw std::runtime_error("Unknown opcode: " + std::to_string(current_opcode));
            }
            pc += (V[x] != V[y]) ? 4 : 2;
            break;

        case 0xA000: // Annn (LD I, addr)
            I = nnn;
            pc += 2;
            break;

        case 0xB000: // Bnnn (JP V0, addr)
            pc = static_cast<uint16_t>(nnn + V[0]);
            break;

        case 0xC000: // Cxkk (RND Vx, byte)
            V[x] = static_cast<uint8_t>(random_byte(rng) & kk);
            pc += 2;
            break;

        case 0xD000: { // Dxyn (DRW Vx, Vy, nibble)
            const uint8_t start_x = static_cast<uint8_t>(V[x] % SCREEN_WIDTH);
            const uint8_t start_y = static_cast<uint8_t>(V[y] % SCREEN_HEIGHT);

            V[0xF] = 0;

            for (uint8_t row = 0; row < n; ++row) {
                const uint8_t sprite_byte = memory[I + row];
                const uint8_t py = static_cast<uint8_t>((start_y + row) % SCREEN_HEIGHT);

                for (uint8_t col = 0; col < 8; ++col) {
                    if ((sprite_byte & (0x80u >> col)) == 0) {
                        continue;
                    }

                    const uint8_t px = static_cast<uint8_t>((start_x + col) % SCREEN_WIDTH);
                    const uint16_t idx = static_cast<uint16_t>(px + (py * SCREEN_WIDTH));

                    if (gfx[idx] == 1) {
                        V[0xF] = 1;
                    }
                    gfx[idx] ^= 1;
                }
            }

            draw_flag = true;
            pc += 2;
            break;
        }

        case 0xE000: // Ex9E (SKP Vx) and ExA1 (SKNP Vx)
            if (kk == 0x9E) {         // Ex9E
                pc += (key[V[x] & 0x0Fu] != 0) ? 4 : 2;
            } else if (kk == 0xA1) {  // ExA1
                pc += (key[V[x] & 0x0Fu] == 0) ? 4 : 2;
            } else {
                throw std::runtime_error("Unknown opcode: " + std::to_string(current_opcode));
            }
            break;

        case 0xF000: // Fx07, Fx0A, Fx15, Fx18, Fx1E, Fx29, Fx33, Fx55, Fx65
            switch (kk) {
            case 0x07: // Fx07
                V[x] = delay_timer;
                pc += 2;
                break;

            case 0x0A: { // Fx0A (WAIT for key press, store in Vx)
                bool key_pressed = false;
                for (uint8_t i = 0; i < 16; ++i) {
                    if (key[i] != 0) {
                        V[x] = i;
                        key_pressed = true;
                        break;
                    }
                }
                if (key_pressed) {
                    pc += 2;
                }
                break;
            }

            case 0x15: // Fx15 (LD DT, Vx)
                delay_timer = V[x];
                pc += 2;
                break;

            case 0x18: // Fx18 (LD ST, Vx)
                sound_timer = V[x];
                pc += 2;
                break;

            case 0x1E: { // Fx1E (ADD I, Vx)
                const uint16_t sum = static_cast<uint16_t>(I + V[x]);
                V[0xF] = (sum > 0x0FFFu) ? 1 : 0;
                I = sum;
                pc += 2;
                break;
            }

            case 0x29: // Fx29 (LD F, Vx)
                I = static_cast<uint16_t>(FONT_START + 5 * (V[x] & 0x0Fu));
                pc += 2;
                break;

            case 0x33: // Fx33 (LD B, Vx)
                memory[I] = static_cast<uint8_t>(V[x] / 100);
                memory[I + 1] = static_cast<uint8_t>((V[x] / 10) % 10);
                memory[I + 2] = static_cast<uint8_t>(V[x] % 10);
                pc += 2;
                break;

            case 0x55: // Fx55 (store V0 to Vx in memory starting at I)
                for (uint8_t r = 0; r <= x; ++r) {
                    memory[I + r] = V[r];
                }
                pc += 2;
                break;

            case 0x65: // Fx65 (fill V0 to Vx with memory starting at I)
                for (uint8_t r = 0; r <= x; ++r) {
                    V[r] = memory[I + r];
                }
                pc += 2;
                break;

            default:
                throw std::runtime_error("Unknown opcode: " + std::to_string(current_opcode));
            }
            break;

        default:
            throw std::runtime_error("Unknown opcode: " + std::to_string(current_opcode));
        }

    }

    void tickTimers() {
        if (delay_timer > 0) {
            --delay_timer;
        }

        if (sound_timer > 0) {
            if (sound_timer == 1) {
                std::cout << "BEEP!\n";
            }
            --sound_timer;
        }
    }

    static uint8_t mapCharToKey(char ch) {
        // Common CHIP-8 keyboard layout (one-character keys):
        // 1 2 3 C      -> 1 2 3 4
        // 4 5 6 D      -> Q W E R
        // 7 8 9 E      -> A S D F
        // A 0 B F      -> Z X C V
        switch (ch) {
        case '1': return 0x1;
        case '2': return 0x2;
        case '3': return 0x3;
        case '4': return 0xC;

        case 'q': case 'Q': return 0x4;
        case 'w': case 'W': return 0x5;
        case 'e': case 'E': return 0x6;
        case 'r': case 'R': return 0xD;

        case 'a': case 'A': return 0x7;
        case 's': case 'S': return 0x8;
        case 'd': case 'D': return 0x9;
        case 'f': case 'F': return 0xE;

        case 'z': case 'Z': return 0xA;
        case 'x': case 'X': return 0x0;
        case 'c': case 'C': return 0xB;
        case 'v': case 'V': return 0xF;
        default: return 0xFF;
        }
    }

    // Poll console input and update `key[]`.
    // Returns true if user requested quit.
    bool pollInput() {
        std::memset(key, 0, sizeof(key));

        if (!terminal_input_inited) {
            // If caller forgot to init, still function in a best-effort way.
            // On POSIX this will be line-buffered; on Windows _kbhit won't work.
        }

        auto handleChar = [&](char ch) -> bool {
            // Quit on ESC.
            if (static_cast<unsigned char>(ch) == 27) {
                return true;
            }

            // Optional quit convenience without consuming keypad keys.
            if (ch == '`' || ch == '~') {
                return true;
            }

            const uint8_t k = mapCharToKey(ch);
            if (k != 0xFF) {
                key[k] = 1;
            }
            return false;
        };

#ifdef _WIN32
        while (_kbhit()) {
            const int v = _getch();
            if (v == 0 || v == 224) {
                // Special key prefix; consume the next byte.
                (void)_getch();
                continue;
            }
            if (handleChar(static_cast<char>(v))) {
                return true;
            }
        }
        return false;
#else
        fd_set set;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        while (select(STDIN_FILENO + 1, &set, nullptr, nullptr, &timeout) > 0 && FD_ISSET(STDIN_FILENO, &set)) {
            char ch = 0;
            const ssize_t nread = read(STDIN_FILENO, &ch, 1);
            if (nread <= 0) {
                break;
            }
            if (handleChar(ch)) {
                return true;
            }

            FD_ZERO(&set);
            FD_SET(STDIN_FILENO, &set);
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;
        }
        return false;
#endif
    }

    // Minimal console renderer: prints 64x32 as ASCII when draw_flag is set.
    // Uses ANSI escape codes to keep it to a single terminal "frame".
    void renderConsole() {
        if (!console_render_enabled || !draw_flag) {
            return;
        }
        draw_flag = false;

        // Throttle a bit so we don't spam the terminal if CPU is very fast.
        const auto now = std::chrono::steady_clock::now();
        if (now - last_draw_ts < std::chrono::milliseconds(16)) {
            return;
        }
        last_draw_ts = now;

        std::cout << "\x1b[H\x1b[2J";
        for (uint8_t y = 0; y < SCREEN_HEIGHT; ++y) {
            for (uint8_t x = 0; x < SCREEN_WIDTH; ++x) {
                const uint8_t p = gfx[x + (y * SCREEN_WIDTH)];
                std::cout << (p ? '#' : ' ');
            }
            std::cout << "\n";
        }
        std::cout.flush();
    }
};