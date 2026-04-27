#include "Utils.h"

Ansi::Ansi(bool enable) {
    enabled = enable;
    if (enabled) {
        reset = "\033[0m";
        bold = "\033[1m";
        dim = "\033[2m";
        red = "\033[31m";
        green = "\033[32m";
        yellow = "\033[33m";
        blue = "\033[34m";
        magenta = "\033[35m";
        cyan = "\033[36m";
        gray = "\033[90m";
    } else {
        reset = ""; bold = ""; dim = ""; red = ""; green = ""; 
        yellow = ""; blue = ""; magenta = ""; cyan = ""; gray = "";
    }
}

int clamp(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

std::string bar(const std::string& label, int pct, int width, Ansi& ansi, const std::string& good_color, const std::string& warn_color) {
    int filled = static_cast<int>((pct / 100.0) * width);
    int empty = width - filled;
    std::string color = (pct >= 50) ? good_color : warn_color;
    
    std::string result = ansi.bold + label;
    while (result.length() < 8 + ansi.bold.length()) {
        result += " ";
    }
    result += ansi.reset + " [" + color;
    
    for (int i = 0; i < filled; ++i) {
        result += "X"; // ASCII alternative for block
    }
    result += ansi.reset;
    for (int i = 0; i < empty; ++i) {
        result += " ";
    }
    
    result += "] ";
    if (pct < 10) result += "  ";
    else if (pct < 100) result += " ";
    result += std::to_string(pct) + "%";
    
    return result;
}

std::string art_spaceship(Ansi& a) {
    std::string s = "";
    s += "                 .\n";
    s += "                .:.\n";
    s += "              .::::.\n";
    s += "     " + a.cyan + "   _" + a.magenta + "_/::::\\__" + a.reset + "\n";
    s += "   " + a.cyan + "  / \\" + a.magenta + "::::::::::" + a.cyan + "/ \\" + a.reset + "\n";
    s += "  " + a.cyan + "  |  |" + a.magenta + "==[ EREBUS-9 ]==" + a.cyan + "|  |" + a.reset + "\n";
    s += "   " + a.cyan + "  \\_/" + a.magenta + "::::::::::" + a.cyan + "\\_/" + a.reset + "\n";
    s += "     " + a.magenta + "__\\:::::::::::/__" + a.reset + "\n";
    s += "   " + a.green + "  /_/  /_/  /_/  /_/" + a.reset;
    return s;
}

std::string art_radar_wave(Ansi& a, int width) {
    std::string base = "";
    for(int i = 0; i < width / 2; ++i) base += "~";
    std::string line1 = a.green + "~~///" + base + "\\\\~~" + a.reset;
    std::string line2 = a.magenta + "\\\\~~" + base + "///~~" + a.reset;
    return line1 + "\n" + line2;
}

std::string art_explosion(Ansi& a) {
    std::string s = "";
    s += a.yellow;
    s += "         _.-^^---....,,--       \n";
    s += "     _--                  --_  \n";
    s += "    <                        >)\n";
    s += "    |   MICROMETEOR IMPACT   | \n";
    s += "    \\._                   _./  \n";
    s += "       '''--. . , ; .--'''     \n";
    s += "             | |   |           \n";
    s += "          .-=||  | |=-.        \n";
    s += "          `-=#$%&%$#=-'        \n";
    s += "             | ;  :|           \n";
    s += "    _____.,-#%&$@%#&#~,._____\n";
    s += a.reset;
    return s;
}

std::string art_solar_flare(Ansi& a) {
    std::string s = "";
    s += a.yellow;
    s += "  .       .   .        .\n";
    s += "   .  * .   *\n";
    s += "      .   .  * .       SOLAR FLARE\n";
    s += " .   .     .       .  .\n";
    s += a.reset;
    s += art_radar_wave(a, 40);
    return s;
}

std::string art_wrench(Ansi& a) {
    std::string s = "";
    s += a.cyan;
    s += "     __\n";
    s += " ___( o)-=={  REPAIR SUCCESS }==-\n";
    s += " \\__  )\n";
    s += "    ||      \"\" SPARKS \"\"\n";
    s += "    ||__   * * * *\n";
    s += "    (___)  * * * *\n";
    s += a.reset;
    return s;
}

std::string art_ai_face(Ansi& a) {
    std::string s = "";
    s += a.cyan + a.bold;
    s += "      _____________\n";
    s += "     /   _______   \\\n";
    s += "    /   /  _  _ \\   \\\n";
    s += "   |   |  (o)(o) |   |   GUARDIAN ASTRA\n";
    s += "   |   |    __   |   |   \"Coordinated survival achieved.\"\n";
    s += "   |   |  \\____/ |   |\n";
    s += "    \\   \\_______/   /\n";
    s += "     \\_____________/\n";
    s += a.reset;
    return s;
}

std::string art_helmet_crack(Ansi& a) {
    std::string s = "";
    s += a.gray;
    s += "       .-\"\"\"-.\n";
    s += "     .'  _   _ '.\n";
    s += "    /   (_) (_)  \\\n";
    s += "   |  .-----.    |   OXYGEN OUT\n";
    s += "   |  |  |  |    |   \"Silence takes Erebus-9.\"\n";
    s += "   |  |  |  |    |\n";
    s += "    \\  '-----'  /\n";
    s += "     '.       .'\n";
    s += "       '-.__.-'\n";
    s += a.reset;
    return s;
}

std::string art_reclaimer_ship(Ansi& a) {
    std::string s = "";
    s += a.green;
    s += "         /\\\n";
    s += "        /  \\      THE RECLAIMER\n";
    s += "   ____/====\\____  \"You stabilized the station.\"\n";
    s += "  /____\\____/____\\\n";
    s += "      /  ||  \\\n";
    s += "     /___||___\\\n";
    s += "        _||_\n";
    s += "       /_/\\_\\\n";
    s += a.reset;
    return s;
}

std::string art_skull(Ansi& a) {
    std::string s = "";
    s += a.red;
    s += "      .-.\\.\n";
    s += "     (o o)  MUTUAL DESTRUCTION\n";
    s += "     | O \\  \"No victors-only wreckage.\"\n";
    s += "      \\   \\\n";
    s += "       `~~~'\n";
    s += a.reset;
    return s;
}

std::string art_banner_game_over(Ansi& a, const std::string& title) {
    std::string s = "";
    s += a.red + a.bold + "\n";
    for(int i=0; i<60; ++i) s += "=";
    s += "\n";
    
    int padding = (60 - title.length()) / 2;
    for(int i=0; i<padding; ++i) s += " ";
    s += title + "\n";
    
    for(int i=0; i<60; ++i) s += "=";
    s += a.reset + "\n";
    return s;
}