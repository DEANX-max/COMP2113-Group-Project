# COMP2113-Group-Project
**Team members:**
1. Xu Dean 3036589208
2. Li Jiaxuan 3036589442
3. Ye Yangrui 3036589466
4. Yu Zhiheng 3036525769
5. Yang Youming 3036588967

**Description of Game**

You are the last surviving commander of a critically damaged starship drifting in deep space. Following a catastrophic system failure, life support is failing, and oxygen levels are dropping rapidly. Your only companion is ASTRA, the ship's advanced but unstable Artificial Intelligence. The Last Transmission is a gripping, terminal-based survival strategy game developed in C++. It challenges players to make life-or-death decisions in a high-pressure environment. You must constantly weigh the risks and rewards of every action: do you expend your own precious oxygen to fix the ship manually, or do you rely on ASTRA, whose loyalty and functionality might be compromised? Survive as many turns as possible, manage your inventory, and see if you can outlast the void.

**Core Features & Gameplay & coding requirement**

The game operates on a turn-based strategy system where every choice impacts your survival trajectory.

1） Dual-Resource Management
Survival depends on balancing two critical, interconnected metrics:

Oxygen Level: Your primary life source. Every action you take, and every turn that passes, consumes oxygen. If it hits zero, the game is over.

ASTRA Trust (AI Stability): The AI can help you perform tasks without draining your oxygen, but overusing or disagreeing with ASTRA lowers its trust. A low trust level increases the chance of AI rebellion or critical task failure.

2） Strategic Decision-Making
During each turn, you are presented with a tactical menu. You must choose how to approach the ship's failing systems:

Manual Repair: Safe and guaranteed to succeed, but physically exhausting (high oxygen cost).

Consult ASTRA: Saves your physical exertion, but introduces risk. Depending on ASTRA's current trust level, the AI might solve the problem perfectly, ignore you, or make the situation worse.

Search for Supplies: Spend time scavenging the ship for tools, trading immediate safety for long-term survival advantages.

3） Dynamic Tool & Inventory System
Players can scavenge and store various items to turn the tide of survival. Powered by a robust C++ Object-Oriented architecture, the game features distinct tool types:

Oxygen Tanks (OxygenTool): Instantly replenishes a portion of your oxygen supply.

Diagnostic Modules (BuffTool): Temporarily boosts repair efficiency or forces ASTRA to comply with your commands.

4） Unpredictable Event Engine
No two playthroughs are exactly alike. The game engine generates random technical anomalies each turn—from sudden hull breaches to AI logic loops—forcing you to constantly adapt your strategy rather than relying on a fixed formula.

5） Immersive Terminal Aesthetics
Who says a terminal game has to be boring text? The Last Transmission features a custom-built rendering engine using ANSI escape sequences. This provides a visually engaging, color-coded user interface, real-time status bars, and ASCII art directly in your command line, creating a moody, sci-fi atmosphere without needing external graphics libraries.

**Compilation and execution instructions**

1） Compilation
Open your terminal or command prompt, navigate to the root directory of the project (where the .cpp files are located), and execute the following command: g++ -pedantic-errors -std=c++11 main.cpp Game.cpp Models.cpp Utils.cpp -o game

2） Execution
command: ./game
