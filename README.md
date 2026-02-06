# Diablo 2 DS1 Map Editor

A comprehensive map editor for Diablo 2 DS1 (map) files, featuring full support for tiles, objects, animations, and all Diablo 2 assets.

## Features

- **Complete DS1 Map Editing**: Load, edit, and save Diablo 2 map files
- **Multi-Layer Support**: Edit floors, walls, shadows, roofs, and special tiles
- **Object Placement**: Add and configure monsters, NPCs, and interactive objects
- **Animation Preview**: View and edit animated tiles and objects
- **Asset Management**: Full support for DT1 tiles, palettes, and COF animations
- **Multiple Act Support**: Works with all 5 Diablo 2 acts
- **Windowed Interface**: Modern windowed graphics mode for easy editing

## System Requirements

- **Operating System**: Windows 10/11
- **Development Tools**: Visual Studio 2022 (Community or higher)
- **Runtime**: Microsoft Visual C++ Redistributable
- **Graphics**: DirectX-compatible graphics card
- **Memory**: 512MB RAM minimum

## Quick Start

### Using Pre-built Release

1. Download the latest release from the `release/` folder
2. Extract all files to a directory
3. Run any of the `.bat` files to load example maps:
   - `Duriel.bat` - Duriel's chamber (Act 2)
   - `Tristram.bat` - Tristram (Act 1)
   - `Harogath.bat` - Barbarian town (Act 5)

### Building from Source

#### Prerequisites

1. Install **Visual Studio 2022** with C++ development tools
2. Ensure Windows SDK is installed
3. Clone or download this repository

#### Build Steps

1. Open VS Code in the project directory
2. Press `Ctrl+Shift+P` and run **"Tasks: Run Task"**
3. Select **"Full Build (Compile + Link)"**
4. The executable will be created in `bin/ds1editor.exe`

#### VS Code Build Tasks

- **`Ctrl+Shift+P` → "Tasks: Run Build Task"** - Quick release build
- **"Full Build (Compile + Link)"** - Complete rebuild from source
- **"Clean Project"** - Remove all build artifacts
- **"Package Release"** - Create distribution package

## Usage

### Command Line Interface

```bash
ds1editor.exe [ds1_file] [level_def_id] [seed]
```

**Parameters:**
- `ds1_file`: Path to DS1 map file (e.g., `ds1\\duriel.ds1`)
- `level_def_id`: Level definition ID from Diablo 2 (e.g., `17` for Duriel's Lair)
- `seed`: Random seed for map generation (typically `481`)

**Examples:**
```bash
# Load Duriel's chamber
ds1editor.exe ds1\\duriel.ds1 17 481

# Load Tristram
ds1editor.exe ds1\\tristram.ds1 32 481

# Interactive mode (no parameters)
ds1editor.exe
```

### VS Code Launch Configurations

Press `F5` or use the debug menu to select:

- **"Launch DS1 Editor - Duriel Map"** - Load Duriel's chamber
- **"Launch DS1 Editor - Tristram Map"** - Load Tristram
- **"Launch DS1 Editor - Custom Map"** - Prompt for custom map parameters
- **"Debug DS1 Editor"** - Debug mode with breakpoints

### GUI Controls

#### Navigation
- **Arrow Keys**: Scroll the map view
- **Page Up/Down**: Zoom in/out
- **Home**: Center view on map

#### Editing Tools
- **Mouse Left Click**: Select/place tiles or objects
- **Mouse Right Click**: Context menu or delete
- **Tab**: Switch between editing layers
- **Space**: Toggle between different tile sets

#### Layers
- **Floor Tiles**: Ground textures and walkable surfaces
- **Wall Tiles**: Walls and collision barriers  
- **Shadow Layer**: Shadow casting tiles
- **Roof Layer**: Ceiling and roof tiles
- **Special Layer**: Special effects and triggers
- **Object Layer**: Monsters, NPCs, and interactive objects

## Project Structure

```
Diablo 2 Map Editor/
├── bin/                    # Executable and runtime files
│   ├── ds1editor.exe      # Main executable
│   ├── ds1edit.ini        # Configuration file
│   ├── Data/              # Core Diablo 2 data files
│   ├── data/             # Game asset data
│   ├── ds1/               # Sample DS1 map files
│   └── *.bat              # Launch scripts
├── src/                   # Source code
│   ├── *.c, *.h          # Main editor source files
│   └── mpq/               # MPQ archive handling
├── .vscode/               # VS Code configuration
│   ├── launch.json        # Debug configurations
│   ├── tasks.json         # Build tasks
│   └── settings.json      # Workspace settings
├── docs/                  # Documentation and tutorials
├── include/               # Header files
├── lib/                   # Static libraries
└── README.md             # This file
```

## Configuration

### Graphics Settings (`ds1edit.ini`)

```ini
# Display configuration
screen_width = 800
screen_height = 600
full_screen = NO          # Use windowed mode
color_depth = 8           # Color depth (8, 16, 24, 32)

# Data paths
mod_dir = data           # Mod directory for assets
```

### Asset Directories

The editor expects the following directory structure:

```
Data/
├── Global/
│   ├── Palette/          # Act palettes (pal.dat files)
│   ├── Tiles/            # DT1 tile files
│   ├── Objects/          # Object definitions
│   ├── Monsters/         # Monster animations (COF)
│   └── AnimData.d2       # Animation database
└── [MOD_DIR]/            # Mod-specific overrides
```

## Development

### Architecture

- **Core Engine**: C language with Allegro 4.4.3 graphics library
- **Graphics**: DirectX integration for Windows
- **File Formats**: Native DS1, DT1, COF, and D2 format support
- **Memory Management**: Custom allocation with cleanup routines

### Key Components

- **`main.c`**: Application entry point and main loop
- **`interfac.c`**: User interface and input handling  
- **`dt1_draw.c`**: Tile rendering and display
- **`ds1misc.c`**: DS1 file loading and manipulation
- **`editobj.c`**: Object editing functionality
- **`mpq/`**: MPQ archive extraction and handling

### Adding New Features

1. **Tile Support**: Add new DT1 processing in `dt1misc.c`
2. **Object Types**: Extend object definitions in `editobj.c`
3. **File Formats**: Implement new loaders following existing patterns
4. **UI Elements**: Add interface components in `interfac.c`

## Troubleshooting

### Common Issues

**"Can not set video mode"**
- Solution: Set `full_screen = NO` in `ds1edit.ini`
- Alternative: Try different `color_depth` values (16, 24, 32)

**"File not found" errors**
- Ensure `mod_dir` setting points to correct data directory
- Verify all DT1 files have proper case-sensitive names
- Check that palettes exist for the target act

**Build failures**
- Verify Visual Studio 2022 is installed with C++ tools
- Ensure all source files are present
- Run "Clean Project" task before rebuilding

### Performance Tips

- Use windowed mode for better performance
- Reduce `screen_width` and `screen_height` for slower systems
- Set `color_depth = 16` for better compatibility

## License

This project builds upon the original Diablo 2 DS1 Editor and includes:

- **Allegro Library 4.4.3**: Used under Allegro license
- **MPQ Handling**: Modified from Tom Amigo's MPQ library
- **Original Editor**: Enhanced and modernized version

See individual source files for specific licensing terms.

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Acknowledgments

- **Original DS1 Editor**: Foundation for this enhanced version
- **Allegro Community**: Graphics library and support
- **Tom Amigo**: MPQ library implementation
- **Diablo 2 Modding Community**: File format documentation and tools

---

**Happy Mapping!** 🗺️⚔️
