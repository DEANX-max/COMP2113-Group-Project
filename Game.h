// ========================= Game Class =========================
#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include "Models.h"
#include "Utils.h"

/**
 * @brief Main engine for The Last Transmission.
 */
class Game {
private:
    Ansi ansi;
    GameState* state;
    std::vector<Tool*> tools; // Dynamic memory management
    std::vector<std::string> messages;

    int time_limit;
    int oxygen_decay_turn;
    int trust_drift;
    int friendly_turns;
    int max_decrypt_uses;
    int max_turns;

public:
    Game(std::string difficulty, bool color = true);
    ~Game(); // Destructor to handle dynamic memory

    void header();
    void show_action_list();
    void status();
    
    bool list_tools();
    bool apply_tool(int idx);

    void act_follow_astra(int& o2_change, int& tr_change);
    void act_manual_checks(int& o2_change, int& tr_change);
    void act_question_astra(int& o2_change, int& tr_change);
    bool act_use_tool(int& o2_change, int& tr_change);
    bool word_guess_game(int& o2_change, int& tr_change);

    void maybe_trigger_event();
    void turn_intro();
    void end_of_turn_effects();
    void ending();
    void outcome_summary(int o2_change, int tr_change, const std::string& action_name);
    void show_help();

    void run(bool show_header = true);
    
    // File I/O Integrations
    void saveGame();
    void loadGame();
};

void show_word_visual(Ansi& ansi);
void select_difficulty(Ansi& ansi, std::string& out_diff, bool& out_tutorial);

#endif // GAME_H