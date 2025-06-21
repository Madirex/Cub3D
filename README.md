# 🧱 Cub3D

**Cub3D** is a 3D graphical project inspired by the legendary **Wolfenstein 3D**, developed as part of the 42 curriculum. It uses raycasting to render a 3D world based on a 2D map.

## 📁 Project Structure

```
cub3D/
├── includes/ # Header files
├── libs/ # External libraries (libft, minilibx)
├── obj/ # Object files
├── src/ # Source code
├── maps/ # Example .cub map files
├── Makefile
└── main.c
```

## ⚙️ Compilation

To compile the project:

```bash
make
```

This will:

  Clone **libft** and **minilibx-linux** (if not already cloned)

  Compile both libraries

  Build the final executable: `cub3D`

## 🧹 Cleaning

```bash
make clean # Remove object files
make fclean # Remove object files + cub3D binary
make re # Full rebuild
```

## 🧪 Run test

To run a test map:

```bash
make test
```

Which executes:

```bash
./cub3D maps/test.cub
```

## 📚 Dependencies

This project uses:

  `libft`

  `minilibx-linux`

  X11 libraries (for Linux)

Make sure you have the following packages:

```bash
sudo apt-get install libx11-dev libxext-dev zlib1g-dev
```

## 🧠 Tips

  Maps must have the `.cub` extension.

  Assets (textures) should be placed in the correct path and defined inside the map file.

## Authors

- [Leroidubuffet](https://github.com/leroidubuffet/)
- [Madirex](https://github.com/madirex/)

