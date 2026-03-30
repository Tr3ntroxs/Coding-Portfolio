#include "chip8.h"
#include <chrono>
#include <thread>

int main(int argc, char **argv) 
{
    try {
        if (argc < 2) {
            std::cerr << "Usage: chip8 <rom_path>\n";
            return 1;
        }

        chip8 cpu;
        cpu.initialize(argv[1]);

        if (!cpu.initTerminalInput()) {
            std::cerr << "Failed to initialize terminal input.\n";
            return 1;
        }

        constexpr double cpu_hz = 700.0;
        constexpr double timer_hz = 60.0;
        const auto cpu_step = std::chrono::duration<double>(1.0 / cpu_hz);
        const auto timer_step = std::chrono::duration<double>(1.0 / timer_hz);

        auto previous = std::chrono::steady_clock::now();
        std::chrono::duration<double> cpu_accumulator{0.0};
        std::chrono::duration<double> timer_accumulator{0.0};

        bool quit = false;
        while (!quit) {
            quit = cpu.pollInput();

            const auto now = std::chrono::steady_clock::now();
            const auto frame_delta = now - previous;
            previous = now;

            cpu_accumulator += frame_delta;
            timer_accumulator += frame_delta;

            while (cpu_accumulator >= cpu_step) {
                cpu.emulateCycle();
                cpu_accumulator -= cpu_step;
            }

            while (timer_accumulator >= timer_step) {
                cpu.tickTimers();
                timer_accumulator -= timer_step;
            }

            cpu.renderConsole();

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
    return 0;
}