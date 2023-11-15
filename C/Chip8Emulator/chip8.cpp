#include "chip8.h" // Your cpu core implementation

int main(int argc, char **argv) 
{
    try {
        chip8 cpu;
        cpu.initialize("/path/to/program/file");
        bool quit = false;
        while (!quit) {
            cpu.emulate_cycle();
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what();
        return 1;
    }
    return 0;
}