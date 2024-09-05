#include <sketch.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#define n 8
// x,y,z cordinates
int int_frame = 100000000;
double eye[3] = {0, 0, -100};
double points[n][3] = {
    {-100, 1, 10},
    {100, 1, 10},
    {100, 100, 10},
    {-100, 100, 10},
    {-100, 1, 110},
    {100, 1, 110},
    {100, 100, 110},
    {-100, 100, 110}};
int k = 0;
double pro_point[n][2];
double p[3] = {0, 50, 60};
void print_array(double arr[3])
{
    for (int i = 0; i < 3; i++)
    {
        printf("%3.2f ", arr[i]);
    }
    printf("\n");
}
void print_points()
{
    for (int i = 0; i < n; i++)
    {
        print_array(points[i]);
    }
}
double e_dis(double x[3], double y[3])
{
    return sqrt(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2) + pow(x[2] - y[2], 2));
}

void turn_point_z(double points[n][3], int i, double theta, double p[3])
{
    double new_point[3] = {0, 0, 0};
    for (int j = 0; j < 3; j++)
    {
        points[i][j] -= p[j];
    }
    new_point[0] = points[i][0] * cos(theta) - points[i][1] * sin(theta);
    new_point[1] = points[i][0] * sin(theta) + points[i][1] * cos(theta);
    new_point[2] = points[i][2];
    for (int j = 0; j < 3; j++)
    {
        new_point[j] += p[j];
        points[i][j] = new_point[j];
    }
}

void turn_point_x(double points[n][3], int i, double theta, double p[3])
{
    double new_point[3] = {0, 0, 0};

    for (int j = 0; j < 3; j++)
    {
        points[i][j] -= p[j];
    }

    new_point[0] = points[i][0];
    new_point[1] = points[i][1] * cos(theta) - points[i][2] * sin(theta);
    new_point[2] = points[i][1] * sin(theta) + points[i][2] * cos(theta);
    for (int j = 0; j < 3; j++)
    {
        new_point[j] += p[j];
        points[i][j] = new_point[j];
    }
}
void turn_point_y(double points[n][3], int i, double theta, double p[3])
{
    double new_point[3] = {0, 0, 0};
    for (int j = 0; j < 3; j++)
    {
        points[i][j] -= p[j];
    }
    new_point[0] = points[i][0] * cos(theta) + points[i][2] * sin(theta);
    new_point[1] = points[i][1];
    new_point[2] = -points[i][0] * sin(theta) + points[i][2] * cos(theta);

    for (int j = 0; j < 3; j++)
    {
        new_point[j] += p[j];
        points[i][j] = new_point[j];
    }
}

void turn_points(double points_1[n][3], double a[3], double p[3])
{
    double x[3] = {0, p[1], p[2]};
    double y[3] = {p[0], 0, p[2]};
    double z[3] = {p[0], p[1], 0};
    double dis_x;
    double dis_y;
    double dis_z;
    double ca;

    for (int i = 0; i < n; i++)
    {
        turn_point_x(points_1, i, a[0], p);
        turn_point_y(points_1, i, a[1], p);
        turn_point_z(points_1, i, a[2], p);
    }
}
void calc(int j, double points_1[n][3])
{
    double a[3] = {(M_PI / 25.0), (M_PI / 25.0), (M_PI / 25.0)};

    turn_points(points_1, a, p);

    for (int i = 0; i < n; i++)
    {
        double l = points_1[i][2] / (points_1[i][2] - eye[2]); // lambda
        pro_point[i][0] = points_1[i][0] + l * (eye[0] - points_1[i][0]);
        pro_point[i][1] = points_1[i][1] + l * (eye[1] - points_1[i][1]);
    }
}
float scale = 1.5;
void connect_points(int a, int b)
{

    draw_line(pro_point[a][0] * scale, pro_point[a][1] * scale, pro_point[b][0] * scale, pro_point[b][1] * scale);
}

#define number_lines 12
int pairs[number_lines][2] = {{1, 2},
                              {2, 3},
                              {3, 4},
                              {1, 4},
                              {1, 5},
                              {2, 6},
                              {3, 7},
                              {4, 8},
                              {5, 6},
                              {5, 8},
                              {6, 7},
                              {7, 8}};

int frame_number = 0;
void save_frame()
{
    char file_path[50];
    sprintf(file_path, "frames/frame_%d.svg", frame_number);
    save_sketch(file_path);
    ++frame_number;
}
void draw()
{
    clear_sketch();

    for (int lno = 0; lno < number_lines; lno++)
    {
        connect_points(pairs[lno][0] - 1, pairs[lno][1] - 1);
    }
    // save_frame();
    save_sketch("3d_line.svg");
}
int main(void)
{
    frame_number += int_frame;
    clear_sketch();
    print_points();
    double a[3] = {0, M_PI, 0};
    turn_points(points, a, p);
    for (int i = 0; i < 100; i++)
    {
                                        calc(i, points);
        draw();
        usleep(100000);
    }
    return 0;
}