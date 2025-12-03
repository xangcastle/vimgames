#include "launcher.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include <unistd.h>
#include <limits.h>

#ifdef __APPLE__
// Helper to get absolute path
std::string getAbsolutePath(const char* path) {
    char abs_path[PATH_MAX];
    if (realpath(path, abs_path)) {
        return std::string(abs_path);
    }
    return std::string(path);
}

void checkAndLaunchInWindow(int argc, char** argv) {
    bool force_window = false;
    
    // Check for flag
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--new-window") {
            force_window = true;
            break;
        }
    }

    // Check for bad terminal
    const char* term = getenv("TERM");
    if (term == nullptr || std::string(term).empty() || std::string(term) == "dumb") {
        force_window = true;
    }

    if (force_window) {
        std::string prog = getAbsolutePath(argv[0]);
        std::string args = "";
        
        // Reconstruct args, skipping --new-window
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg != "--new-window") {
                args += " " + arg;
            }
        }

        // Command to run in the new terminal
        // We quote the program path
        std::string command_to_run = "\\\"" + prog + "\\\"" + args + "; exit";
        
        // AppleScript to launch Terminal
        std::string osa_cmd = "osascript -e 'tell application \"Terminal\" to do script \"" + command_to_run + "\"'";
        osa_cmd += " -e 'tell application \"Terminal\" to activate'";

        int result = system(osa_cmd.c_str());
        if (result == 0) {
            exit(0);
        } else {
            std::cerr << "Failed to launch in new window." << std::endl;
        }
    }
}
#else
void checkAndLaunchInWindow(int argc, char** argv) {
    // No-op for non-macOS
}
#endif
