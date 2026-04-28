#include "Models.h"
#include <fstream>
#include <iostream>

// ========================= GameState Implementation =========================
GameState::GameState(int max_decrypt) {
    oxygen = 100;
    trust = 50;
    alive = true;
    turn = 1;
    ending_mark = "";
    tool_buffed = false;
    manual_checks_done = 0;
    used_toolkit = false;
    used_all_tools = false;
    decrypt_uses_left = max_decrypt;
    last_action = "";
    last_action_taken = false;
    
    stats["turns_survived"] = 0;
    stats["tools_used"] = 0;
    stats["manual_checks"] = 0;
    stats["successful_decrypts"] = 0;
}

void GameState::saveToFile(const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << oxygen << "\n" << trust << "\n" << turn << "\n";
        outFile << alive << "\n" << decrypt_uses_left << "\n";
        outFile << manual_checks_done << "\n" << tool_buffed << "\n";
        outFile << stats["tools_used"] << "\n" << stats["manual_checks"] << "\n" << stats["successful_decrypts"] << "\n";
        outFile.close();
        std::cout << "Game saved successfully to " << filename << std::endl;
    } else {
        std::cout << "Error: Could not open file for saving." << std::endl;
    }
}

bool GameState::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        inFile >> oxygen >> trust >> turn >> alive >> decrypt_uses_left;
        inFile >> manual_checks_done >> tool_buffed;
        inFile >> stats["tools_used"] >> stats["manual_checks"] >> stats["successful_decrypts"];
        inFile.close();
        std::cout << "Game loaded successfully from " << filename << std::endl;
        return true;
    }
    std::cout << "No save file found." << std::endl;
    return false;
}

// ========================= Tool Implementation =========================
Tool::Tool(std::string n, std::string e, int c, std::string k) {
    name = n;
    effect = e;
    count = c;
    kind = k;
}

std::string Tool::getName() const { return name; }
std::string Tool::getEffect() const { return effect; }
int Tool::getCount() const { return count; }
void Tool::decrementCount() { if (count > 0) count--; }
std::string Tool::getKind() const { return kind; }

OxygenTool::OxygenTool() : Tool("Oxygen Tank", "Restore 20% oxygen.", 1, "oxygen") {}
void OxygenTool::applyEffect(GameState& state, Ansi& ansi) {
    state.oxygen = clamp(state.oxygen + 20, 0, 100);
    std::cout << ansi.green << "Oxygen Tank used. Oxygen +20%." << ansi.reset << std::endl;
}

BuffTool::BuffTool() : Tool("Tool Kit", "Boost next manual check success by +50%.", 1, "buff") {}
void BuffTool::applyEffect(GameState& state, Ansi& ansi) {
    state.tool_buffed = true;
    state.used_toolkit = true;
    std::cout << ansi.cyan << "Tool Kit primed. Next manual check +50% success." << ansi.reset << std::endl;
}

TrustTool::TrustTool() : Tool("Emergency Beacon", "Broadcast clean telemetry (+20% trust). Consumes a turn.", 1, "trust") {}
void TrustTool::applyEffect(GameState& state, Ansi& ansi) {
    state.trust = clamp(state.trust + 20, 0, 100);
    std::cout << ansi.blue << "Emergency Beacon deployed. Trust +20%." << ansi.reset << std::endl;
}

MultiTool::MultiTool() : Tool("Nano-Repair Drone", "Fix micro-leaks (+10% O2) and calm ASTRA (+10% trust).", 1, "multi") {}
void MultiTool::applyEffect(GameState& state, Ansi& ansi) {
    state.oxygen = clamp(state.oxygen + 10, 0, 100);
    state.trust = clamp(state.trust + 10, 0, 100);
    std::cout << ansi.green << "Nano-Repair Drone patched leaks (+10% O2) and soothed ASTRA (+10 trust)." << ansi.reset << std::endl;
}