# Fantasy Pet Game

A C++ turn-based fantasy pet battle game built with SFML. Players choose a pet, train and equip it, then battle using class-specific abilities and status effects.

## Game systems

- Four playable pet classes: Dragon, Phoenix, Unicorn, and Griffin.
- Turn-based battles, active abilities, cooldowns, and status effects.
- Pet leveling, experience, training, and management.
- Inventory with healing, mana, shield, and speed items.
- Arena, shop, guild, and leaderboard screens.

## Requirements

- C++11-compatible compiler.
- SFML 2.x development libraries: Graphics, Window, System, and Audio.

The project was originally built with SFML 2.x. Configure your compiler and linker with the SFML include and library directories. For a MinGW build, the command is typically:

```powershell
g++ main.cpp -std=c++11 -I<sfml-include> -L<sfml-lib> -o FantasyPetGame.exe -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

The exact SFML package must match your compiler and architecture. With a shared SFML build, place the matching runtime DLLs beside the executable or add their directory to `PATH`.

## Run

Start the game with the repository root as the working directory. It loads its images and audio from that directory. `leaderboard.txt` is created locally when the game saves scores and is intentionally ignored by Git.

The Arial font file is not included. On Windows, the game falls back to the system Arial font. On other systems, place a compatible font at the repository root named `arial.ttf`, or update the font fallback in `main.cpp`.

## Assets

The repository includes the original game images and background music required by the interface and battle effects.
