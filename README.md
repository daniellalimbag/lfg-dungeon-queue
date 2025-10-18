# LFG Dungeon Queue

This is a Looking for Group dungeon queue system for STDISCM Problem Set 2.

## Building and Running

### 1. Clone or Open the Project
- Open the project folder in VS Code.

### 2. Build the Project
VS Code uses tasks defined in `.vscode/tasks.json` for building.

- **Automatic Build**: When you run or debug, VS Code will build using the pre-launch task.
- **Manual Build**:
  - Open Terminal in VS Code (View > Terminal).
  - Run: `g++ -std=c++20 -O2 -Wall -Wextra -pedantic player.cpp party.cpp dungeoninstance.cpp queuemanager.cpp utils.cpp main.cpp -o lfg_dungeon_queue.exe -pthread`
- **Expected Output**: Generates `lfg_dungeon_queue.exe` in the project root.

### 3. Running the Program

#### Interactive Mode (Prompts for Input)
- Use the VS Code debugger for interactive input.
- Launch Configuration:
  - In `.vscode/launch.json`, ensure `"args": []` and `"externalConsole": true` (or `"console": "integratedTerminal"`).
- Run: F5 or Run and Debug > "Run LFG Dungeon Queue".
- The program prompts: `Enter n t h d t1 t2: ` (e.g., `3 2 2 6 3 10`).
- **Note**: With `"externalConsole": true`, input may require a separate console window. Switch to `"console": "integratedTerminal"` if needed for better input handling.

#### Command-Line Mode (Direct Arguments)
- Run from Terminal: `./lfg_dungeon_queue.exe <n> <t> <h> <d> <t1> <t2>`
  - `<n>`: Number of dungeon instances (e.g., 3).
  - `<t>`: Number of Tank players (e.g., 2).
  - `<h>`: Number of Healer players (e.g., 2).
  - `<d>`: Number of DPS players (e.g., 6).
  - `<t1>`: Minimum run time in seconds (e.g., 3).
  - `<t2>`: Maxmum run time in seconds (e.g., 10).
- Example: `./lfg_dungeon_queue.exe 3 2 2 6 3 10`
