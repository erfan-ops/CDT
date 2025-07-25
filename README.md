# Delaunay Flow - Geometric Live Desktop Wallpaper

## Description
Delaunay Flow is a visually striking and interactive live desktop wallpaper that uses Constrained Delaunay Triangulation to generate and color a field of triangles across your screen. Each triangle's color is determined by its vertical position, creating a smooth gradient that flows from top to bottom.

## Features
- 🔺 Dynamic triangle mesh generated via Constrained Delaunay Triangulation
- 🌈 Smooth vertical color gradient with multiple color stops
- ⚙️ Fully customizable via `settings.json`
- 🚀 High-performance OpenGL rendering
- 💻 Built in C++ with real-time responsiveness

## Requirements
- OS: Windows 10/11
- GPU: OpenGL-compatible
- Compiler: Visual Studio 2022 (recommended)

## Installation
1. **Download**: Get the latest version of Delaunay Flow from the [Releases](https://github.com/erfan-ops/CDT/releases) page.
2. **Launch**: Run `CDT.exe` to enjoy your new live wallpaper.

## Building
1. Clone the repository:
   ```bash
   git clone https://github.com/erfan-ops/CDT.git
   ```
2. Open the project in Visual Studio 2022.
3. Build the solution.
4. Run the executable from the output directory.

## Configuration
Customize the wallpaper by editing `settings.json`:
```json
{
  "fps": 120,
  "background-colors": [
    [ 0.98, 0.33, 0.33, 1.0 ],
    [ 0.98, 0.55, 0.15, 1.0 ],
    [ 0.96, 0.82, 0.20, 1.0 ],
    [ 0.47, 0.30, 0.58, 1.0 ]
  ],

  "stars": {
    "radius": 8,
    "color": [ 1, 1, 1, 1 ],
    "count": 180,
    "min-speed": 0,
    "max-speed": 30,
    "segments": 8,
    "draw-stars": false
  },

  "offset-bounds": 300
}
```

- `fps`: Target frames per second.
- `background-colors`: Gradient stops (RGBA format) interpolated based on triangle Y position.
- `stars`: Optional star config.
- `offset-bounds`: The screen offset which enables the stars to go pass though screen boundaries.

## Contribution
Contributions are welcome! If you have an idea for improvement or want to submit a fix, open an issue or send a pull request.

## License
This project is licensed under the [MIT License](LICENSE).

## Acknowledgments
- OpenGL for rendering
- CDT algorithms for triangle generation
- Windows API for desktop integration
- Visual Studio for development

🎨 Transform your desktop into a geometric canvas with **Delaunay Flow**! 🔻✨
