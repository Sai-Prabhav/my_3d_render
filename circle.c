#include <sketch.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#define n 8

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
point_3d eye = {15, 0, 0};
double plane_dist = 0.3;
double zoom = 25;

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

point_2d project(point_3d point)
{
    // eye will be perpendecular to the plane

    point_2d return_2d;
    point_3d line_vector = sub_3d(point, eye);
    point_3d plane_point = mult_3d(eye, plane_dist);
    double lam = dot_3d(eye, sub_3d(plane_point, eye)) / dot_3d(line_vector, eye);
    // printf("lambda is : %lf\n\n", lam);
    point_3d on_plane = add_3d(eye, mult_3d(line_vector, lam));
    // print_point(on_plane);

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
    // print_point(on_plane);
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

int numb_obj = 0;
// int size_obj=0;
obj_3d *objs;

obj_3d *add_obj(obj_3d obj, obj_3d objs[])
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
    return objs;
}

point_3d a = {0, 0, 100};
point_3d b = {1, 0, 0};
point_3d lis[30] = {{3, 4, -100}, {3, 4, -100}, {3, 4, -100}};

void calc()
{
    turn_y(&eye, (point_3d){0, 0, 0}, 0.03);
    turn_obj_x(objs[2], (point_3d){0, 0, 0}, -0.1);
    turn_obj_z(objs[3], (point_3d){0, 0, 0}, -0.1);
    turn_obj_x(objs[1], (point_3d){0, 0, 0}, 0.1);
    turn_obj_z(objs[0], (point_3d){0, 0, 0}, 0.05);
    turn_obj_z(objs[5], (point_3d){0, 0, 0}, 0.05);
    turn_obj_y(objs[5], (point_3d){0, 0, 0}, 0.05);
    turn_obj_x(objs[5], (point_3d){0, 0, 0}, -0.05);
    turn_obj_x(objs[4], (point_3d){0, 0, 0}, -0.05);
    translate_obj(objs[4], (point_3d){0, 0.2, 0});
    print_point(eye);
}

void draw()
{
    clear_sketch();
    for (int no_ob = 0; no_ob < numb_obj; no_ob++)
    {
        draw_obj(objs[no_ob]);
    }
    save_sketch("hello.svg");
}

int main(int argc, char const *argv[])
{
    objs = (obj_3d *)malloc(sizeof(obj_3d));
    objs = add_obj(cube((point_3d){0, 0, 0}, 10), objs);
    objs = add_obj(cube((point_3d){0, 0, 5}, 5), objs);
    objs = add_obj(cube((point_3d){0, 0, -5}, 5), objs);
    objs = add_obj(cube((point_3d){0, 0, 0}, 5), objs);
    objs = add_obj(cube((point_3d){0, -20, 0}, 5), objs);
    objs = add_obj(cube((point_3d){0, 0, 0}, 15), objs);

    set_size(1200);
    set_stroke_width(3);
    for (int l = 0; l < 200; l++)
    {
        calc();
        draw();
        usleep(100000);
    }

    free(objs);
    // abs_3d(point_3d{3, -4, 100});
    // point_3d p = {10, 934, 34};
    // project(p);
    // save_sketch("hell0.svg");
    return 0;
}
