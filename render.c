#include <sketch.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
// x-> right
// y-> up
// z-> out of screen
// turn is lock wise when you look from + to -

// x,y,z cordinates
typedef struct
{
    double x;
    double y;
    double z;
} point_3d;
typedef struct
{
    double x;
    double y;
} point_2d;

typedef struct
{
    int start;
    int end;
} line;

typedef struct
{
    point_3d *points;
    line *lines;
    int len_points;
    int len_lines;

} obj_3d;

// int numb_obj = 0;
// obj_3d *objs;
int point_radius = 3;
point_3d o = {0, 0, 0};
point_3d eye_p = {0, 0, 0};
point_3d eye = {0, 0, 20};
double plane_dist = 0.6;
double zoom = 70;
int frame_number = 0;
int numb_obj = 0;
obj_3d *objs;
int max_frames = 500;
int numb_cube = 20;

void save_frame()
{
    char file_path[50];
    sprintf(file_path, "frames/frame_%d.svg", frame_number + 10000);
    save_sketch(file_path);
    printf("saved: %s\n", file_path);
    ++frame_number;
}

void print_num(double x, char *tex)
{
    printf("%s: %lf\n", tex, x);
}

void print_point(point_3d p)
{
    printf("X: %lf\tY: %lf\tZ: %lf\n", p.x, p.y, p.z);
}

double e_dis(point_3d a, point_3d b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

double abs_3d(point_3d a)
{
    point_3d c = {0, 0, 0};
    return e_dis(a, c);
}

point_3d add_3d(point_3d a, point_3d b)
{
    point_3d r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

point_3d sub_3d(point_3d a, point_3d b)
{
    point_3d r;
    r.y = a.y - b.y;
    r.x = a.x - b.x;
    r.z = a.z - b.z;
    return r;
}

point_3d mult_3d(point_3d a, double b)
{
    point_3d r;
    r.y = a.y * b;
    r.x = a.x * b;
    r.z = a.z * b;
    return r;
}

point_3d norm_3d(point_3d a)
{

    return mult_3d(a, 1 / (abs_3d(a)));
}

double dot_3d(point_3d a, point_3d b)
{
    double r;
    r = 0;
    r += a.y * b.y;
    r += a.x * b.x;
    r += a.z * b.z;
    return r;
}

void turn_z(point_3d *point, point_3d p, double theta)
{
    theta = -theta;
    point_3d new_point = {0, 0, 0};

    point->x -= p.x;
    point->y -= p.y;
    point->z -= p.z;

    new_point.x = point->x * cos(theta) - point->y * sin(theta);
    new_point.y = point->x * sin(theta) + point->y * cos(theta);
    new_point.z = point->z;
    new_point.x += p.x;
    new_point.y += p.y;
    new_point.z += p.z;

    point->x = new_point.x;
    point->y = new_point.y;
    point->z = new_point.z;
}

void turn_x(point_3d *point, point_3d p, double theta)
{
    point_3d new_point = {0, 0, 0};

    point->x -= p.x;
    point->y -= p.y;
    point->z -= p.z;

    new_point.x = point->x;
    new_point.y = point->y * cos(theta) - point->z * sin(theta);
    new_point.z = point->y * sin(theta) + point->z * cos(theta);
    new_point.x += p.x;
    new_point.y += p.y;
    new_point.z += p.z;

    point->x = new_point.x;
    point->y = new_point.y;
    point->z = new_point.z;
}

void turn_y(point_3d *point, point_3d p, double theta)
{
    theta = -theta;
    point_3d new_point = {0, 0, 0};

    point->x -= p.x;
    point->y -= p.y;
    point->z -= p.z;

    new_point.x = point->x * cos(theta) + point->z * sin(theta);
    new_point.y = point->y;
    new_point.z = -point->x * sin(theta) + point->z * cos(theta);

    new_point.x += p.x;
    new_point.y += p.y;
    new_point.z += p.z;

    point->x = new_point.x;
    point->y = new_point.y;
    point->z = new_point.z;
}

void turn_point(point_3d *point, point_3d p, point_3d angle)
{
    turn_x(point, p, angle.x);
    turn_y(point, p, angle.y);
    turn_z(point, p, angle.z);
}

void turn_obj(obj_3d obj, point_3d p, point_3d angle)
{
    for (int i = 0; i < obj.len_points; i++)
    {
        turn_point(&obj.points[i], p, angle);
    }
}

void turn_obj_x(obj_3d obj, point_3d p, double angle)
{
    for (int i = 0; i < obj.len_points; i++)
    {
        turn_x(&obj.points[i], p, angle);
    }
}

void turn_obj_y(obj_3d obj, point_3d p, double angle)
{
    for (int i = 0; i < obj.len_points; i++)
    {
        turn_y(&obj.points[i], p, angle);
    }
}

void turn_obj_z(obj_3d obj, point_3d p, double angle)
{
    for (int i = 0; i < obj.len_points; i++)
    {
        turn_z(&obj.points[i], p, angle);
    }
}

void translate_obj(obj_3d obj, point_3d diff)
{
    point_3d sum;
    for (int i = 0; i < obj.len_points; i++)
    {
        sum = add_3d(obj.points[i], diff);
        obj.points[i].x = sum.x;
        obj.points[i].y = sum.y;
        obj.points[i].z = sum.z;
    }
}

obj_3d create_obj(point_3d points[], line lines[], int len_points, int len_lines)
{
    obj_3d obj;
    obj.len_lines = len_lines;
    obj.len_points = len_points;
    obj.lines = (line *)calloc(len_lines, sizeof(line));
    obj.points = (point_3d *)calloc(len_points, sizeof(point_3d));
    for (int k = 0; k < len_lines; k++)
    {
        obj.lines[k] = lines[k];
    }
    for (int i = 0; i < len_points; i++)
    {
        obj.points[i] = points[i];
    }
    return obj;
}

obj_3d cube(point_3d center, double size)
{
    point_3d cube_points[8] = {
        (point_3d){size / 2, size / 2, size / 2},
        (point_3d){-size / 2, size / 2, size / 2},
        (point_3d){size / 2, -size / 2, size / 2},
        (point_3d){-size / 2, -size / 2, size / 2},
        (point_3d){size / 2, size / 2, -size / 2},
        (point_3d){-size / 2, size / 2, -size / 2},
        (point_3d){size / 2, -size / 2, -size / 2},
        (point_3d){-size / 2, -size / 2, -size / 2}};
    for (int i = 0; i < 8; i++)
    {
        cube_points[i] = add_3d(cube_points[i], center);
    }

    line cube_line[12] = {
        (line){0, 1},
        (line){0, 2},
        (line){0, 4},
        (line){1, 3},
        (line){1, 5},
        (line){2, 3},
        (line){2, 6},
        (line){3, 7},
        (line){4, 5},
        (line){4, 6},
        (line){5, 7},
        (line){6, 7}};
    return create_obj(cube_points, cube_line, 8, 12);
}

void add_obj(obj_3d obj)
{
    if (numb_obj == 0)
    {
        objs = (obj_3d *)malloc(sizeof(obj_3d));
    }

    ++numb_obj;
    objs = (obj_3d *)realloc(objs, sizeof(obj_3d) * (numb_obj));
    // objs[numb_obj - 1]->len_lines=obj.;
    objs[numb_obj - 1].len_lines = obj.len_lines;
    objs[numb_obj - 1].len_points = obj.len_points;
    objs[numb_obj - 1].lines = obj.lines;
    objs[numb_obj - 1].points = obj.points;
}

point_2d project(point_3d point)
{
    // eye will be perpendecular to the plane

    point_2d return_2d;
    point_3d line_vector = sub_3d(point, eye);
    point_3d plane_point = mult_3d(eye, plane_dist);
    double lam = dot_3d(eye, sub_3d(plane_point, eye)) / dot_3d(line_vector, eye);
    point_3d on_plane = add_3d(eye, mult_3d(line_vector, lam));

    point_3d per = eye;
    // turn the y
    double theta_y = atan(per.x / per.z);
    turn_y(&per, o, theta_y);

    // turn x
    double theta_x = atan(per.y / per.z);
    turn_x(&per, o, theta_x);
    // print_point(per);

    // do the same tranformation to the point
    turn_y(&on_plane, plane_point, theta_y);
    turn_x(&on_plane, plane_point, theta_x);

    return_2d.x = on_plane.x;
    return_2d.y = on_plane.y;

    return return_2d;
}

void draw_points(point_2d points[], int len_points)
{

    for (int i = 0; i < len_points; i++)
    {
        draw_circle(points[i].x, points[i].y, point_radius);
    }
}

void draw_lines(obj_3d obj, point_2d points[])
{
    for (int i = 0; i < obj.len_lines; i++)
    {

        draw_line(points[obj.lines[i].start].x, points[obj.lines[i].start].y, points[obj.lines[i].end].x, points[obj.lines[i].end].y);
    }
}

void draw_obj(obj_3d obj)
{
    point_2d points_2d[obj.len_points];
    for (int k = 0; k < obj.len_points; k++)
    {
        point_2d my_point_2d = project(obj.points[k]);
        points_2d[k].x = my_point_2d.x * zoom;
        points_2d[k].y = my_point_2d.y * zoom;
    }
    // exit(0);
    draw_points(points_2d, obj.len_points);
    draw_lines(obj, points_2d);
}

double get_num(char text[], int *i)
{
    int digits;
    char num[5];
    digits = 0;
    *i += 1;
    while (text[*i] != ' ' && text[*i] != '\0')
    {
        num[digits++] = text[*i];
        *i += 1;
    }
    num[digits++] = '\0';
    return atof(num);
}

void get_str(char text[], int *i, char str[])
{
    int digits;
    digits = 0;
    while (text[*i] != ' ' && text[*i] != '\0')
    {
        str[digits++] = text[*i];
        *i += 1;
    }
    str[digits++] = '\0';
}

void draw_from_file(char path[])
{
    FILE *fptr;
    fptr = fopen(path, "r");
    char text_line[100];
    int i;
    char num[5];
    int digits;
    while (fgets(text_line, 100, fptr))
    {
        printf("%s", text_line);
        i = 0;
        printf("%c\n", text_line[i]);
        char command[10];
        get_str(text_line, &i, command);

        if (strcmp(command, "cube") == 0)
        {
            i += 1;
            double x = get_num(text_line, &i);
            double y = get_num(text_line, &i);
            double z = get_num(text_line, &i);
            double size = get_num(text_line, &i);
            add_obj(cube((point_3d){x, y, z}, size));
        }
        else if (strcmp(command, "point") == 0)
        {
            point_3d dot_point[1] = {get_num(text_line, &i), get_num(text_line, &i), get_num(text_line, &i)};
            line lines[0];
            add_obj(create_obj(dot_point, lines, 1, 0));
        }
        else if (strcmp(command, "line") == 0)
        {
            point_3d line_points[2];
            line_points[0] = (point_3d){get_num(text_line, &i), get_num(text_line, &i), get_num(text_line, &i)};
            line_points[1] = (point_3d){get_num(text_line, &i), get_num(text_line, &i), get_num(text_line, &i)};
            line lines[1] = {0, 1};
            add_obj(create_obj(line_points, lines, 2, 1));
        }
        else if (strcmp(command, "obj") == 0)
        {
            /*
                the format of this is
                obj max_points max_lines
                point1_x point1_y point2_z
                point2_x point2_y point2_z
                point3_x point3_y point3_z
                ...
                point_max_points_x point_max_points_y point_max_points_z
                line_1_start line_1_end
                line_2_start line_2_end
                line_3_start line_3_end
                ...
                line_max_line_start line_max_line_end
            */
            int max_points = get_num(text_line, &i);
            int max_lines = get_num(text_line, &i);
            point_3d obj_points[max_points];
            line obj_lines[max_lines];

            fgets(text_line, 100, fptr);
            i = -1;
            for (int point_index = 0; point_index < max_points; point_index++)
            {
                obj_points[point_index] = (point_3d){get_num(text_line, &i), get_num(text_line, &i), get_num(text_line, &i)};
                fgets(text_line, 100, fptr);
                i = -1;
            }

            // fgets(text_line, 100, fptr);
            for (int line_index = 0; line_index < max_lines; line_index++)
            {
                int point_1 = get_num(text_line, &i);
                obj_lines[line_index] = (line){point_1 - 1, get_num(text_line, &i) - 1};
                fgets(text_line, 100, fptr);
                i = -1;
            }
            add_obj(create_obj(obj_points, obj_lines, max_points, max_lines));
        }
    }
}

void calc()
{
    for (int ff = 0; ff < numb_cube; ff++)
    {
        turn_obj_z(objs[ff], (point_3d){0, 0, 0}, 0.01 * ((ff % 5) + 1));
        turn_obj_x(objs[ff], (point_3d){0, 0, 0}, 0.01 * (((ff + 0) % 5) + 1));
        turn_obj_y(objs[ff], (point_3d){0, 0, 0}, 0.01 * (((ff + 0) % 5) + 1));
    }
}

void draw()
{
    clear_sketch();
    for (int no_ob = 0; no_ob < numb_obj; no_ob++)
    {
        draw_obj(objs[no_ob]);
    }
    save_sketch("sketch.svg");
    // save_frame();
}

int main(int argc, char const *argv[])
{
    set_sketch_size(1099, 1000);
    set_stroke_width(3);

    if (0 == 0)
    {
        objs = (obj_3d *)malloc(sizeof(obj_3d));
        for (int q = 0; q < numb_cube; q++)
        {
            add_obj(cube((point_3d){0, 0, 0}, (q + 1) / 2.5 * 50 / numb_cube));
        }

        for (int l = 0; l < max_frames; l++)
        {
            calc();
            draw();
            usleep(100000);
        }
        free(objs);
        
    }

    save_sketch("cube.svg");
    return 0;
}
