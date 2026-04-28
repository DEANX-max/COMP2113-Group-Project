// ========================= Entrypoint =========================
#include "Game.h"
#include "Utils.h"
#include <iostream>
#include <string>

/**
 * @brief Main function to run The Last Transmission.
 * @return int Exit status.
 */
int main() {
    Ansi ansi(true); // Enable ANSI colors
    
    show_word_visual(ansi);
    
    Game temp_game("Normal", true);
    temp_game.header();
    
    bool first_time = true;
    while (true) {
        if (!first_time) {
            std::cout << ansi.cyan;
            for(int i=0; i<64; ++i) std::cout << "=";
            std::cout << ansi.reset << "\n";
        }
        first_time = false;
        
        std::string difficulty;
        bool want_tutorial;
        select_difficulty(ansi, difficulty, want_tutorial);
        
        if (want_tutorial) {
            Game tut_game("Normal", true);
            tut_game.show_help();
            std::cout << "\nType 'continue' to proceed to difficulty selection: ";
            std::string temp;
            std::cin >> temp;
            
            std::cout << ansi.cyan;
            for(int i=0; i<64; ++i) std::cout << "=";
            std::cout << ansi.reset << "\n";
            select_difficulty(ansi, difficulty, want_tutorial);
        }
        
        Game game(difficulty, true);
        game.run(false);
        
        while (true) {
            std::cout << "Do you want to play again? (y/n): ";
            std::string again;
            std::cin >> again;
            
            if (again == "y" || again == "Y") {
                break;
            } else if (again == "n" || again == "N") {
                std::cout << "Thanks for playing! Safe travels, Commander.\n";
                return 0;
            } else {
                std::cout << ansi.red << "Invalid input. Please enter 'y' or 'n'." << ansi.reset << "\n";
            }
        }
    }
    
    return 0;
}