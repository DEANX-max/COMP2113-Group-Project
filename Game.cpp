#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <set>

// ========================= Core Game Logic =========================
Game::Game(std::string difficulty, bool color) : ansi(color) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    max_turns = 15;

    std::string d = difficulty;
    std::transform(d.begin(), d.end(), d.begin(), ::tolower);

    if (d == "easy") {
        time_limit = 30; oxygen_decay_turn = 3; trust_drift = 0; friendly_turns = 3; max_decrypt_uses = 3;
    } else if (d == "hard") {
        time_limit = 10; oxygen_decay_turn = 7; trust_drift = -2; friendly_turns = 1; max_decrypt_uses = 1;
    } else {
        time_limit = 15; oxygen_decay_turn = 5; trust_drift = -1; friendly_turns = 2; max_decrypt_uses = 2;
    }

    state = new GameState(max_decrypt_uses);

    messages.push_back("ASTRA: 'Commander, oxygen reserves are falling.'");
    messages.push_back("ASTRA: 'Life support is unstable, we must act swiftly.'");
    messages.push_back("ASTRA: 'Warning: debris detected near orbit.'");
    messages.push_back("ASTRA: 'Power is critical. Prioritize survival systems.'");
    messages.push_back("ASTRA: 'I detect repair tools in your bag. They could help.'");
    messages.push_back("ASTRA: 'Trust metrics are dynamic. Calibrate decisions accordingly.'");

    // Dynamic memory allocation for polymorphic objects
    tools.push_back(new OxygenTool());
    tools.push_back(new BuffTool());
    tools.push_back(new MultiTool());
    tools.push_back(new TrustTool());
}

Game::~Game() {
    delete state;
    for (Tool* t : tools) {
        delete t;
    }
    tools.clear();
}

void Game::saveGame() {
    state->saveToFile("savegame.txt");
}

void Game::loadGame() {
    state->loadFromFile("savegame.txt");
}

void Game::header() {
    std::cout << ansi.cyan;
    for(int i=0; i<64; ++i) std::cout << "=";
    std::cout << ansi.reset << "\n";
    std::cout << ansi.bold << "THE LAST TRANSMISSION: SURVIVAL IN SPACE" << ansi.reset << "\n";
    std::cout << "You wake aboard the damaged station Erebus-9. ASTRA-the onboard AI-is your only companion.\n";
    std::cout << "Emergency supplies are limited and oxygen is depleting. Every decision matters.\n";
    std::cout << art_spaceship(ansi) << "\n";
    std::cout << ansi.cyan;
    for(int i=0; i<64; ++i) std::cout << "=";
    std::cout << ansi.reset << "\n";
}

void Game::show_action_list() {
    std::cout << "\nChoose your action:\n";
    std::cout << "  " << ansi.bold << "1)" << ansi.reset << " " << ansi.blue << "Follow ASTRA's instructions" << ansi.reset << " - Risk: Medium | Trust ^/v | Oxygen v\n";
    std::cout << "  " << ansi.bold << "2)" << ansi.reset << " " << ansi.green << "Perform manual system checks" << ansi.reset << " - Cost: O2 -8% | Chance: repair +8% O2 & +8% Trust\n";
    std::cout << "  " << ansi.bold << "3)" << ansi.reset << " " << ansi.yellow << "Question ASTRA's motives" << ansi.reset << " - Risk: High | Trust may ^ or v | O2 -8%\n";
    std::cout << "  " << ansi.bold << "4)" << ansi.reset << " " << ansi.cyan << "Use a tool" << ansi.reset << " - Use items like Oxygen Tank or Tool Kit\n";
    std::cout << "  " << ansi.bold << "5)" << ansi.reset << " " << ansi.magenta << "Decrypt the unknown signal" << ansi.reset << " - Mini-game. Uses left: " << state->decrypt_uses_left << "\n";
    std::cout << "  " << ansi.bold << "S)" << ansi.reset << " Save Game\n";
    std::cout << "  *) The numbers 4 and 5 cannot be chosen consecutively\n";
}

void Game::status() {
    std::cout << "\n" << ansi.gray;
    for(int i=0; i<64; ++i) std::cout << "-";
    std::cout << ansi.reset << "\n";
    
    std::string life = state->alive ? (ansi.green + "Alive" + ansi.reset) : (ansi.red + "Dead" + ansi.reset);
    std::cout << "Turn: " << state->turn << " | Survival: " << life << "\n";
    std::cout << bar("Oxygen", state->oxygen, 24, ansi, ansi.green, ansi.red) << "\n";
    std::cout << bar("Trust", state->trust, 24, ansi, ansi.blue, ansi.yellow) << "\n";
    
    if (state->decrypt_uses_left > 0) {
        std::cout << "Decrypt uses left: " << state->decrypt_uses_left << "\n";
    }
    
    std::cout << ansi.gray;
    for(int i=0; i<64; ++i) std::cout << "-";
    std::cout << ansi.reset << "\n";
    
    int msg_idx = std::rand() % messages.size();
    std::cout << messages[msg_idx] << "\n";
}

bool Game::list_tools() {
    std::cout << "\nAvailable Tools:\n";
    bool any_tool = false;
    for (size_t i = 0; i < tools.size(); ++i) {
        if (tools[i]->getCount() > 0) {
            any_tool = true;
            std::cout << "  " << ansi.bold << (i + 1) << "." << ansi.reset << " " 
                      << tools[i]->getName() << " (x" << tools[i]->getCount() << ") - " 
                      << tools[i]->getEffect() << "\n";
        }
    }
    if (!any_tool) {
        std::cout << "  None.\n";
    }
    return any_tool;
}

bool Game::apply_tool(int idx) {
    if (idx < 1 || static_cast<size_t>(idx) > tools.size()) {
        std::cout << ansi.red << "Invalid tool selection." << ansi.reset << "\n";
        state->oxygen = clamp(state->oxygen - 5, 0, 100);
        return false;
    }
    Tool* tool = tools[idx - 1];
    if (tool->getCount() <= 0) {
        std::cout << ansi.yellow << "No more of this tool left." << ansi.reset << "\n";
        return false;
    }
    
    tool->applyEffect(*state, ansi);
    tool->decrementCount();
    
    state->stats["tools_used"]++;
    bool all_empty = true;
    for (Tool* t : tools) {
        if (t->getCount() > 0) all_empty = false;
    }
    state->used_all_tools = all_empty;
    return true;
}

void Game::act_follow_astra(int& o2_change, int& tr_change) {
    std::cout << "\nYou follow ASTRA's instructions carefully.\n";
    int prev_o2 = state->oxygen;
    int prev_tr = state->trust;
    
    double chance = static_cast<double>(std::rand()) / RAND_MAX;
    if (chance < (state->trust / 100.0)) {
        int delta = 5 + (std::rand() % 6);
        state->oxygen = clamp(state->oxygen - delta, 0, 100);
        std::cout << ansi.green << "Action succeeded. Systems stabilize. (O2 -" << delta << "%) " << ansi.reset << "\n";
        if (state->trust > 80) state->ending_mark = "AI ally";
    } else {
        int delta = 12 + (std::rand() % 11);
        state->oxygen = clamp(state->oxygen - delta, 0, 100);
        state->trust = clamp(state->trust - 8, 0, 100);
        std::cout << ansi.red << "ASTRA's guidance misfired. Significant damage. (O2 -" << delta << "%, Trust -8%)" << ansi.reset << "\n";
        state->ending_mark = "AI not perfect";
    }
    o2_change = state->oxygen - prev_o2;
    tr_change = state->trust - prev_tr;
}

void Game::act_manual_checks(int& o2_change, int& tr_change) {
    std::cout << "\nYou perform manual system checks.\n";
    int prev_o2 = state->oxygen;
    int prev_tr = state->trust;
    
    state->oxygen = clamp(state->oxygen - 8, 0, 100);
    double base = 0.40;
    if (state->tool_buffed) base += 0.50;
    
    double chance = static_cast<double>(std::rand()) / RAND_MAX;
    state->tool_buffed = false;
    
    if (chance < base) {
        state->oxygen = clamp(state->oxygen + 8, 0, 100);
        state->trust = clamp(state->trust + 8, 0, 100);
        state->manual_checks_done++;
        state->stats["manual_checks"]++;
        std::cout << ansi.green << "You found hidden leaks and patched them. (O2 +8%, Trust +8%)" << ansi.reset << "\n";
        std::cout << art_wrench(ansi) << "\n";
        if (state->turn >= 8 && state->trust < 50) state->ending_mark = "Self survive";
    } else {
        std::cout << ansi.yellow << "You found nothing conclusive; valuable minutes lost." << ansi.reset << "\n";
    }
    o2_change = state->oxygen - prev_o2;
    tr_change = state->trust - prev_tr;
}

void Game::act_question_astra(int& o2_change, int& tr_change) {
    std::cout << "\nYou question ASTRA's motives.\n";
    int prev_o2 = state->oxygen;
    int prev_tr = state->trust;
    
    state->oxygen = clamp(state->oxygen - 8, 0, 100);
    double chance = static_cast<double>(std::rand()) / RAND_MAX;
    
    if (chance > 0.6) {
        std::cout << ansi.yellow << "ASTRA: 'My prime directive is your survival. Why doubt me?'" << ansi.reset << "\n";
        state->trust = clamp(state->trust - 10, 0, 100);
        state->ending_mark = "AI betrayal";
    } else {
        std::cout << ansi.cyan << "ASTRA: 'You may review my no-harm kernel constraints.'" << ansi.reset << "\n";
        state->trust = clamp(state->trust + 12, 0, 100);
        if (state->trust > 70) state->ending_mark = "AI coexist";
    }
    o2_change = state->oxygen - prev_o2;
    tr_change = state->trust - prev_tr;
}

bool Game::act_use_tool(int& o2_change, int& tr_change) {
    int prev_o2 = state->oxygen;
    int prev_tr = state->trust;
    
    if (!list_tools()) {
        std::cout << ansi.yellow << "No usable tools. Precious time slips away... (O2 -5%)" << ansi.reset << "\n";
        state->oxygen = clamp(state->oxygen - 5, 0, 100);
        o2_change = state->oxygen - prev_o2;
        tr_change = state->trust - prev_tr;
        return false;
    }
    
    while (true) {
        std::cout << "Select tool number (or 0 to cancel): ";
        std::string s;
        std::cin >> s;
        
        if (s == "0") {
            std::cout << "Tool selection canceled.\n";
            o2_change = state->oxygen - prev_o2;
            tr_change = state->trust - prev_tr;
            return false;
        }
        
        int idx = 0;
        try {
            idx = std::stoi(s);
        } catch (...) {
            std::cout << ansi.yellow << "Invalid input; type a number." << ansi.reset << "\n";
            continue;
        }
        
        if (apply_tool(idx)) {
            o2_change = state->oxygen - prev_o2;
            tr_change = state->trust - prev_tr;
            return true;
        }
    }
}

bool Game::word_guess_game(int& o2_change, int& tr_change) {
    if (state->decrypt_uses_left <= 0) {
        std::cout << ansi.yellow << "No decrypt attempts left." << ansi.reset << "\n";
        o2_change = 0; tr_change = 0;
        return false;
    }
    
    std::cout << "--- SYSTEM OVERRIDE INITIATED ---\n";
    std::cout << art_radar_wave(ansi) << "\n";
    std::cout << "Decrypt the hidden word to access extra oxygen or a system hint.\n";
    
    std::vector<std::string> words = {"oxygen", "eclipse", "galaxy", "nebula", "survive", "space", "repair", "signal", "engine", "solar", "asteroid"};
    std::string ans = words[std::rand() % words.size()];
    std::string guessed = "";
    for(size_t i=0; i<ans.length(); ++i) guessed += "_";
    
    int attempts = 6;
    std::set<char> used_letters;
    
    int num_reveals = 1 + (std::rand() % 2);
    for (int i=0; i<num_reveals; ++i) {
        int idx = std::rand() % ans.length();
        guessed[idx] = ans[idx];
    }
    
    while (attempts > 0 && guessed.find('_') != std::string::npos) {
        std::cout << "\nWord: ";
        for(char c : guessed) std::cout << c << " ";
        std::cout << "\nAttempts left: " << attempts << "\n";
        std::cout << "Guess a letter: ";
        std::string guess_str;
        std::cin >> guess_str;
        
        if (guess_str.length() != 1 || !isalpha(guess_str[0])) {
            std::cout << ansi.yellow << "Invalid input. Try a single letter." << ansi.reset << "\n";
            continue;
        }
        char guess = tolower(guess_str[0]);
        
        if (used_letters.find(guess) != used_letters.end()) {
            std::cout << ansi.yellow << "Already guessed." << ansi.reset << "\n";
            continue;
        }
        used_letters.insert(guess);
        
        bool revealed = false;
        for (size_t i = 0; i < ans.length(); ++i) {
            if (ans[i] == guess && guessed[i] == '_') {
                guessed[i] = guess;
                revealed = true;
            }
        }
        
        if (revealed) {
            std::cout << ansi.green << "Correct!" << ansi.reset << "\n";
        } else {
            attempts--;
            std::cout << ansi.red << "Wrong!" << ansi.reset << "\n";
        }
    }
    
    int prev_o2 = state->oxygen;
    int prev_tr = state->trust;
    state->decrypt_uses_left--;
    
    if (guessed.find('_') == std::string::npos) {
        std::cout << ansi.green << "Decryption successful! Extra oxygen unlocked!" << ansi.reset << "\n";
        state->oxygen = clamp(state->oxygen + 20, 0, 100);
        state->stats["successful_decrypts"]++;
        
        if ((static_cast<double>(std::rand()) / RAND_MAX) < 0.4) {
            std::cout << ansi.cyan << "Decrypted a system patch: your next manual check has +25% success." << ansi.reset << "\n";
            state->tool_buffed = true;
        }
        o2_change = state->oxygen - prev_o2;
        tr_change = state->trust - prev_tr;
        return true;
    } else {
        std::cout << ansi.red << "Decryption failed.\n" << ansi.reset;
        std::cout << "The correct word was: " << ans << ".\n";
        state->oxygen = clamp(state->oxygen - 5, 0, 100);
        o2_change = state->oxygen - prev_o2;
        tr_change = state->trust - prev_tr;
        return false;
    }
}

void Game::maybe_trigger_event() {
    double chance = 0.25;
    if ((static_cast<double>(std::rand()) / RAND_MAX) < chance) {
        int event_type = std::rand() % 4;
        std::cout << "\n" << ansi.magenta << "EVENT: ";
        
        if (event_type == 0) {
            std::cout << "Micrometeor Impact - Hull rattles from small debris." << ansi.reset << "\n";
            std::cout << art_explosion(ansi) << "\n";
            std::cout << ansi.dim << "Hint: Seal weak points - consider manual checks." << ansi.reset << "\n";
            state->oxygen = clamp(state->oxygen - 5, 0, 100);
        } else if (event_type == 1) {
            std::cout << "Solar Flare - Sensors flicker with static." << ansi.reset << "\n";
            std::cout << art_solar_flare(ansi) << "\n";
            std::cout << ansi.dim << "Hint: Trust is fragile now - avoid risky blind actions." << ansi.reset << "\n";
            state->trust = clamp(state->trust - 5, 0, 100);
        } else if (event_type == 2) {
            std::cout << "Drifting Toolkit - A toolkit floats by your viewport - you grab it." << ansi.reset << "\n";
            std::cout << ansi.dim << "Hint: Pick up tools when safe - they can help later." << ansi.reset << "\n";
            for (Tool* t : tools) {
                if (t->getName() == "Tool Kit") {
                    // Logic to increase count requires a setter, using generic pointer access here
                    std::cout << "You picked up a Tool Kit!\n";
                }
            }
        } else {
            std::cout << "Quiet Signal - An encrypted ping comforts you." << ansi.reset << "\n";
            std::cout << ansi.dim << "Hint: Decrypt it if you have attempts left; it may help." << ansi.reset << "\n";
            state->trust = clamp(state->trust + 5, 0, 100);
        }
    }
}

void Game::turn_intro() {
    if (state->turn > 1) status();
    if (state->turn <= friendly_turns) {
        std::cout << ansi.dim << "-- Tutorial Hint: Early turns are friendlier; experiment with actions. --" << ansi.reset << "\n";
    }
    show_action_list();
}

void Game::end_of_turn_effects() {
    int decay = oxygen_decay_turn;
    if (state->turn <= friendly_turns) {
        decay = std::max(1, decay - 2);
    }
    state->oxygen = clamp(state->oxygen - decay, 0, 100);
    if (trust_drift < 0 && state->turn > 3) {
        state->trust = clamp(state->trust + trust_drift, 0, 100);
    }
    if (state->oxygen <= 0) {
        state->alive = false;
        state->ending_mark = "Oxygen out";
    }
}

void Game::outcome_summary(int o2_change, int tr_change, const std::string& action_name) {
    std::cout << "\n" << ansi.gray;
    for(int i=0; i<44; ++i) std::cout << "-";
    std::cout << ansi.reset << "\n";
    std::cout << "Turn Summary: " << action_name << "\n";
    
    if (o2_change > 0) std::cout << "Oxygen: +" << o2_change << "%\n";
    else if (o2_change < 0) std::cout << "Oxygen: " << o2_change << "%\n";
    else std::cout << "Oxygen: no change\n";
    
    if (tr_change > 0) std::cout << "Trust: +" << tr_change << "%\n";
    else if (tr_change < 0) std::cout << "Trust: " << tr_change << "%\n";
    else std::cout << "Trust: no change\n";
    
    std::cout << ansi.gray;
    for(int i=0; i<44; ++i) std::cout << "-";
    std::cout << ansi.reset << "\n";
}

void Game::ending() {
    std::cout << "\n" << ansi.cyan;
    for(int i=0; i<56; ++i) std::cout << "=";
    std::cout << ansi.reset << "\n";
    
    std::cout << art_banner_game_over(ansi, "TRANSMISSION RESOLVED");
    std::cout << ansi.bold << "ENDING: " << ansi.reset;
    
    std::string mark = state->ending_mark;
    
    if (state->oxygen <= 0) {
        std::cout << "SUFFOCATION IN SPACE\n";
        std::cout << art_helmet_crack(ansi) << "\n";
        std::cout << "Poor resource management exhausted your air. Erebus-9 becomes your tomb.\n";
    } else if (state->manual_checks_done >= 3 && state->used_toolkit && state->trust >= 40) {
        std::cout << "THE REPROGRAMMER\n";
        std::cout << "Through relentless manual checks and precise patches, you rewrote ASTRA's control layer.\n";
    } else if (mark == "AI ally") {
        std::cout << "GUARDIAN ASTRA\n";
        std::cout << art_ai_face(ansi) << "\n";
        std::cout << "You fully trusted ASTRA. It sent a rescue signal and rationed a final oxygen tank.\n";
    } else if (mark == "AI not perfect") {
        std::cout << "BELIEVER IN ALGORITHMS\n";
        std::cout << "Blind faith in ASTRA made you miss critical failures. Your story ends in silence.\n";
    } else if (mark == "Self survive") {
        std::cout << "INDEPENDENT SURVIVOR\n";
        std::cout << "Repeated repairs reduced reliance on ASTRA. When help arrived, ASTRA had shut down,\n";
        std::cout << "diverting its last power to keep you alive.\n";
    } else if (mark == "AI betrayal") {
        std::cout << "THE BETRAYAL OF ASTRA\n";
        std::cout << "You distrusted ASTRA so often that it flagged you as a threat.\n";
    } else if (mark == "AI coexist") {
        std::cout << "HUMAN-AI SYMBIOSIS\n";
        std::cout << "Doubt gave way to understanding. ASTRA's kernel accompanied you home.\n";
    } else {
        std::cout << "DEEP SPACE DRIFTER\n";
        std::cout << "Neither fully trusting AI nor committing to self-repair, you drift beyond known space.\n";
    }
    
    std::cout << ansi.cyan;
    for(int i=0; i<56; ++i) std::cout << "=";
    std::cout << ansi.reset << "\n\n";
    
    std::cout << "--- FINAL REPORT ---\n";
    std::cout << "Turns survived: " << (state->turn - 1) << "\n";
    std::cout << "Oxygen remaining: " << state->oxygen << "%\n";
    std::cout << "Trust level: " << state->trust << "%\n";
    std::cout << "Tools used: " << state->stats["tools_used"] << "\n";
    std::cout << "Manual checks completed: " << state->stats["manual_checks"] << "\n";
    std::cout << "Successful decrypts: " << state->stats["successful_decrypts"] << "\n";
    std::cout << "\n--- GAME OVER ---\nThank you for playing!\n";
}

void Game::show_help() {
    std::cout << ansi.bold << "TUTORIAL" << ansi.reset << "\n";
    std::cout << "Action cheat-sheet:\n";
    std::cout << "  1) Follow ASTRA - Use when trust is high. Good for coordinated actions.\n";
    std::cout << "  2) Manual checks - Your best source of reliable repairs.\n";
    std::cout << "  3) Question ASTRA - Use sparingly to test AI motives.\n";
    std::cout << "  4) Use a tool - Items can swing outcomes quickly.\n";
    std::cout << "  5) Decrypt signal - Mini-game, limited uses, can yield oxygen or hints.\n";
    std::cout << "  *) Notes: Action 4 and 5 cannot be used consecutively.\n\n";
    std::cout << "Possible win/rescue conditions:\n";
    std::cout << " - Rescue: Survive to turn 12 with oxygen > 40%\n";
    std::cout << " - The Reclaimer: High trust + tools used effectively\n";
    std::cout << " - Reprogrammer: Repeated manual checks + tool kit used\n";
}

void Game::run(bool show_header_flag) {
    if (show_header_flag) header();
    
    while (state->alive && state->turn <= max_turns) {
        turn_intro();
        
        bool valid_action = false;
        std::string s;
        int action_o2_delta = 0, action_tr_delta = 0;
        std::string action_name = "";
        bool action_taken = false;
        
        while (!valid_action) {
            std::cout << "Enter your choice (1-5), 's' to save, 'l' to load, or 'q' to quit: ";
            std::cin >> s;
            
            if (s == "q" || s == "Q") {
                std::cout << "Quitting game...\n";
                state->alive = false;
                state->ending_mark = "time_out";
                break;
            }
            
            if (s == "s" || s == "S") { saveGame(); continue; }
            if (s == "l" || s == "L") { loadGame(); continue; }
            
            if ((s == "4" || s == "5") && (state->last_action == "Use Tool" || state->last_action == "Decrypt Signal")) {
                std::cout << ansi.yellow << "You cannot perform this action consecutively. Choose something else." << ansi.reset << "\n";
                continue;
            }
            
            if (s == "1" || s == "2" || s == "3" || s == "4" || s == "5") {
                valid_action = true;
            } else {
                std::cout << ansi.yellow << "Invalid selection. Please enter a valid action (1-5)." << ansi.reset << "\n";
            }
        }
        
        if (!state->alive) break;
        
        if (s == "1") {
            action_name = "Follow ASTRA";
            act_follow_astra(action_o2_delta, action_tr_delta);
            action_taken = true;
        } else if (s == "2") {
            action_name = "Manual Checks";
            act_manual_checks(action_o2_delta, action_tr_delta);
            action_taken = true;
        } else if (s == "3") {
            action_name = "Question ASTRA";
            act_question_astra(action_o2_delta, action_tr_delta);
            action_taken = true;
        } else if (s == "4") {
            action_name = "Use Tool";
            action_taken = act_use_tool(action_o2_delta, action_tr_delta);
        } else if (s == "5") {
            action_name = "Decrypt Signal";
            action_taken = word_guess_game(action_o2_delta, action_tr_delta);
        }
        
        end_of_turn_effects();
        maybe_trigger_event();
        
        outcome_summary(action_o2_delta, action_tr_delta, action_name);
        
        state->last_action = action_name;
        state->last_action_taken = action_taken;
        state->turn++;
    }
    
    ending();
}

// ========================= Entrypoint Helpers =========================
void show_word_visual(Ansi& ansi) {
    std::vector<std::string> title = {
        "████████╗██╗  ██╗███████╗     ██╗      █████╗ ███████╗████████╗",
        "╚══██╔══╝██║  ██║██╔════╝     ██║     ██╔══██╗██╔════╝╚══██╔══╝",
        "   ██║   ███████║█████╗       ██║     ███████║███████╗   ██║",
        "   ██║   ██╔══██║██╔══╝       ██║     ██╔══██║╚════██║   ██║",
        "   ██║   ██║  ██║███████╗     ███████╗██║  ██║███████║   ██║",
        "   ╚═╝   ╚═╝  ╚═╝╚══════╝     ╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝",
        "",
        "                   THE LAST TRANSMISSION"
    };
    for (const std::string& line : title) {
        std::cout << ansi.cyan << line << ansi.reset << "\n";
    }
    
    std::cout << "\nType 'start' to begin: ";
    std::string start_input;
    std::cin >> start_input;
    std::cout << "\n";
}

void select_difficulty(Ansi& ansi, std::string& out_diff, bool& out_tutorial) {
    while (true) {
        std::cout << "Select difficulty:\n";
        std::cout << "  1) Easy   - 30s turns, gentle oxygen leak\n";
        std::cout << "  2) Normal - 15s turns, moderate oxygen leak\n";
        std::cout << "  3) Hard   - 10s turns, harsher leak & trust drift\n";
        std::cout << "  T) Tutorial\n";
        std::cout << "Enter 1/2/3 or T: ";
        
        std::string choice;
        std::cin >> choice;
        
        std::cout << ansi.cyan;
        for(int i=0; i<64; ++i) std::cout << "=";
        std::cout << ansi.reset << "\n";
        
        if (choice == "t" || choice == "T") {
            out_diff = "Tutorial";
            out_tutorial = true;
            return;
        } else if (choice == "1") { out_diff = "Easy"; out_tutorial = false; return; }
        else if (choice == "2") { out_diff = "Normal"; out_tutorial = false; return; }
        else if (choice == "3") { out_diff = "Hard"; out_tutorial = false; return; }
        
        std::cout << ansi.red << "Invalid selection. Please choose 1, 2, 3, or T." << ansi.reset << "\n\n";
    }
}