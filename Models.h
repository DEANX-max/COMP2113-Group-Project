// ========================= Data Models =========================
#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <map>
#include "Utils.h"

/**
 * @brief Class managing the current state of the game.
 */
class GameState {
public:
    int oxygen;
    int trust;
    bool alive;
    int turn;
    std::string ending_mark;
    bool tool_buffed;
    int manual_checks_done;
    bool used_toolkit;
    bool used_all_tools;
    int decrypt_uses_left;
    std::string last_action;
    bool last_action_taken;
    
    std::map<std::string, int> stats;

    GameState(int max_decrypt);
    
    // File I/O Methods
    void saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);
};

/**
 * @brief Abstract Base Class for Tools.
 */
class Tool {
protected:
    std::string name;
    std::string effect;
    int count;
    std::string kind;
public:
    Tool(std::string n, std::string e, int c, std::string k);
    virtual ~Tool() {}

    std::string getName() const;
    std::string getEffect() const;
    int getCount() const;
    void decrementCount();
    std::string getKind() const;
    
    virtual void applyEffect(GameState& state, Ansi& ansi) = 0;
};

// Derived Tool Classes
class OxygenTool : public Tool {
public:
    OxygenTool();
    void applyEffect(GameState& state, Ansi& ansi) override;
};

class BuffTool : public Tool {
public:
    BuffTool();
    void applyEffect(GameState& state, Ansi& ansi) override;
};

class TrustTool : public Tool {
public:
    TrustTool();
    void applyEffect(GameState& state, Ansi& ansi) override;
};

class MultiTool : public Tool {
public:
    MultiTool();
    void applyEffect(GameState& state, Ansi& ansi) override;
};

#endif // MODELS_H