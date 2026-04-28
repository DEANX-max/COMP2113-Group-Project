// ========================= Utilities & ASCII Art =========================
#ifndef UTILS_H
#define UTILS_H

#include <string>

/**
 * @brief Handles ANSI escape codes for terminal colors.
 */
class Ansi {
public:
    bool enabled;
    std::string reset;
    std::string bold;
    std::string dim;
    std::string red;
    std::string green;
    std::string yellow;
    std::string blue;
    std::string magenta;
    std::string cyan;
    std::string gray;

    /**
     * @brief Constructor for Ansi
     * @param enable Boolean to enable or disable color output
     */
    Ansi(bool enable = true);
};

// ASCII Art Function Declarations
std::string art_spaceship(Ansi& a);
std::string art_radar_wave(Ansi& a, int width = 56);
std::string art_explosion(Ansi& a);
std::string art_solar_flare(Ansi& a);
std::string art_wrench(Ansi& a);
std::string art_ai_face(Ansi& a);
std::string art_helmet_crack(Ansi& a);
std::string art_reclaimer_ship(Ansi& a);
std::string art_skull(Ansi& a);
std::string art_banner_game_over(Ansi& a, const std::string& title = "GAME OVER");

// Utility Functions
int clamp(int v, int lo, int hi);
std::string bar(const std::string& label, int pct, int width, Ansi& ansi, const std::string& good_color, const std::string& warn_color);

#endif // UTILS_H