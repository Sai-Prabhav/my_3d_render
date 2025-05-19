# 3D Renderer

This project is a simple 3D rendering engine written in C. It allows you to create, manipulate, and render 3D objects such as cubes, points, and lines. The rendered output is saved as SVG files.

## How to Use

### Prerequisites
- A C compiler (e.g., `gcc`)



### Compilation
To compile the program, use the following command:
```bash
cd csketch/sketch
make
```
for the first time to compile then
```bash
make
```
in the root directory

### Running the Program
Run the compiled program with:
```bash
./render
```

The program will:
1. Create a series of cubes.
2. Rotate and render them frame by frame.
3. Save the final output as `cube.svg`.

### Using `draw_from_file`

The `draw_from_file` function allows you to create and render 3D objects from a text file. The file should contain commands to define objects, points, lines, or cubes.

#### File Format
The file should contain commands in the following format:

1. **Cube**
   ```
   cube x y z size
   ```
   - `x`, `y`, `z`: Coordinates of the cube's center.
   - `size`: Size of the cube.

2. **Point**
   ```
   point x y z
   ```
   - `x`, `y`, `z`: Coordinates of the point.

3. **Line**
   ```
   line x1 y1 z1 x2 y2 z2
   ```
   - `x1`, `y1`, `z1`: Start point of the line.
   - `x2`, `y2`, `z2`: End point of the line.

4. **Custom Object**
   ```
   obj max_points max_lines
   x1 y1 z1
   x2 y2 z2
   ...
   line_start_1 line_end_1
   line_start_2 line_end_2
   ...
   ```
   - `max_points`: Number of points in the object.
   - `max_lines`: Number of lines in the object.
   - Followed by the coordinates of each point.
   - Then, the start and end indices (1-based) of each line.

#### Example Input File
```text
cube 0 0 0 10
point 5 5 5
line 0 0 0 10 10 10
obj 4 2
0 0 0
10 0 0
0 10 0
0 0 10
1 2
3 4
```

#### Running with `draw_from_file`
To use the `draw_from_file` function, modify the `main` function to include:
```c
draw_from_file("path/to/your/input_file.txt");
```

Replace `"path/to/your/input_file.txt"` with the path to your input file.

### Output
The program will render the objects defined in the file and save the output as an SVG file (`sketch.svg`).

## Notes
- Ensure the `frames` directory exists if you enable frame saving.
- Modify parameters like `zoom`, `plane_dist`, and `max_frames` in the code to customize the rendering.

Enjoy creating and rendering 3D objects!
