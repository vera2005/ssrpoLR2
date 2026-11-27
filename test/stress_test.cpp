#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {
const int N_REQUESTS = 5000;
const std::string INPUT_FILE = "stress_input.txt";
const std::string OUTPUT_FILE = "stress_output.txt";
const std::string BINARY = "bin/airfield";
}

static void generateInput() {
    std::ofstream os(INPUT_FILE);
    assert(os.is_open());
    const char* types[] = {"passenger", "cargo", "military"};
    const char* sizes[] = {"small", "medium", "large"};
    for (int i = 0; i < N_REQUESTS; ++i) {
        os << "add " << types[i % 3] << " " << sizes[i % 3];
        if (i % 100 == 0) os << " emergency";
        os << "\n";
    }
    os << "work\n";
    os << "view\n";
    os << "save stress_save.data\n";
    os << "clean\n";
    os << "load stress_save.data\n";
    os << "view\n";
}

static int runAirfield() {
    std::ostringstream cmd;
    cmd << BINARY << " < " << INPUT_FILE << " > " << OUTPUT_FILE << " 2>&1";
    return std::system(cmd.str().c_str());
}

static bool checkOutput() {
    std::ifstream is(OUTPUT_FILE);
    if (!is.is_open()) return false;
    std::string content((std::istreambuf_iterator<char>(is)),
                         std::istreambuf_iterator<char>());
    bool has_processed = content.find("Requests processed") != std::string::npos;
    bool has_report    = content.find("Total:") != std::string::npos;
    bool has_saved     = content.find("Collection saved") != std::string::npos;
    bool has_cleaned   = content.find("Collection cleaned") != std::string::npos;
    bool has_loaded    = content.find("Loaded") != std::string::npos;
    return has_processed && has_report && has_saved && has_cleaned && has_loaded;
}

int main() {
    std::cout << "Generating stress input (" << N_REQUESTS << " requests)...\n";
    generateInput();

    std::cout << "Running airfield binary...\n";
    int rc = runAirfield();
    if (rc != 0) {
        std::cerr << "FAIL: airfield exited with code " << rc << "\n";
        return 1;
    }

    std::cout << "Checking output...\n";
    if (!checkOutput()) {
        std::cerr << "FAIL: output does not contain expected phrases\n";
        return 1;
    }

    std::cout << "Stress test passed!\n";
    return 0;
}
