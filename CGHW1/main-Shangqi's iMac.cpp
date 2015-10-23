//
//  main.cpp
//  CG_hw1
//
//  Created by Shangqi Wu on 14-10-03.
//  Copyright (c) 2014 Shangqi Wu. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>
#include <limits>

using namespace std;

#define pi acos((double)-1)

// Define global variant-------------------------------------------------
int a=0, b=0, c=499, d=499, m=0, n=0, r=0; // These vars are used to be attributes of the image and window.
float s = 1.0;
string input = "./hw1.ps", output = "./out.xpm";
vector<vector<char> > xpmpix;  // This vector represents status of all pixels in the world window.

// Define all basic functions--------------------------------------------
void help();
int rnd(float arg);
void optana (int argc, char * const argv[]);
string setheader();
string setend();

// Define classes needed-------------------------------------------------
class Point {
private:
    int x;
    int y;
public:
    void set(int argx, int argy) {
        x = argx;
        y = argy;
    }
    int getx() {
        return x;
    }
    int gety() {
        return y;
    }
    void trans(int m, int n) { //Tanslation
        int ptold[3] = {x, y, 1};
        int matrix[3][3] = {1, 0, m, 0, 1, n, 0, 0, 1};
        int pt[3] = {0, 0, 0};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                pt[i] = pt[i] + matrix[i][j] * ptold[j];
            }
        }
        x=pt[0]; y=pt[1];
    }
    void scale(float s) { //Scaling
        float ptold[3] = {(float)x, (float)y, 1};
        float matrix[3][3] = {s, 0, 0, 0, s, 0, 0, 0, 1};
        float pt[3] = {0, 0, 0};
        for (int i = 0; i <3  ; i++) {
            for (int j = 0; j < 3; j++) {
                pt[i] += matrix[i][j] * ptold[j];
            }
        }
        x=rnd(pt[0]); y=rnd(pt[1]);
    }
    void rot(int r) { //Rotation
        double ang = (double)r * pi / 180.0;
        float ptold[3] = {(float)x, (float)y, (float)1};
        float pt[3] = {0, 0, 0};
        float matrix[3][3] = {(float)cos(ang), (float)-sin(ang), 0, (float)sin(ang), (float)cos(ang), 0, 0, 0, 1};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                pt[i] += matrix[i][j] * ptold[j];
            }
        }
        x=rnd(pt[0]); y=rnd(pt[1]);
    }
};

class Line { // present the line by: y = slope*x + d
private:
    Point start;
    Point end;
    bool sd_exist;
    float slope;
    float d;
    bool in;
    void cal() {
        if (start.getx() != end.getx()) {
            if (start.getx() > end.getx()) { // ensure the line goes from left to right
                Point tmp = start;
                start = end;
                end = tmp;
            }
            slope = (float)(start.gety() - end.gety()) / (float)(start.getx() - end.getx());
            d = (float)start.gety() - (slope * (float)start.getx());
            sd_exist = true;
        }else {
            if (start.gety() > end.gety()) { // If te line is vertical, make sure the line goes upward.
                Point tmp;
                tmp = start;
                start = end;
                end = tmp;
            }
            slope = numeric_limits<float>::max();
            d = numeric_limits<float>::max();
            sd_exist = false;
        }
    }
public:
    void setall(int argx1, int argy1, int argx2, int argy2) {
        start.set(argx1, argy1);
        end.set(argx2, argy2);
        cal();
    }
    bool sd() {
        return sd_exist;
    }
    Point gets() {
        return start;
    }
    Point gete() {
        return end;
    }
    Point cal_inter_wlr(float x) { // return the intersaction point of x=x
        Point tmp;
        if (sd_exist) {
            float y = slope*x +d;
            tmp.set(rnd(x), rnd(y));
        }else {
            tmp.set(numeric_limits<int>::max(), numeric_limits<int>::max());
        }
        return tmp;
    }
    Point cal_inter_wtb(float y) { // return the intersaction point of y=y
        Point tmp;
        if (sd_exist) {
            float x = (y - d)/ slope;
            tmp.set(rnd(x), rnd(y));
        }else {
            tmp.set(start.getx(), (int)y);
        }
        return tmp;
    }
    void setin(bool ifin) {
        in = ifin;
    }
    bool getin() {
        return in;
    }
    void trans(int m,int n) { // Translation for both ends of the line.
        start.trans(m, n);
        end.trans(m, n);
        cal();
    }
    void scale(float s) { // Scaling for both ends of the line.
        start.scale(s);
        end.scale(s);
        cal();
    }
    void rot(int r) { //Rotating for both ends of the line.
        start.rot(r);
        end.rot(r);
        cal();
    }
    void showall() { // Calculate all the points of the line.
        if (in == true) {
            if (sd_exist == true) {
                // Fllowing codes are of Bresenham Algorithm, presented in L-02_Lines.pdf. Codes are odifiied for this cpp file.
                int dx, dy, D, x, y;
                dx = end.getx() - start.getx();
                dy = abs(end.gety() - start.gety());
                x = start.getx();
                y = start.gety();
                if (0<slope && slope<1) {
                    D = 2*dy - dx;
                    int k = 0;
                    for (x = start.getx(); x <= end.getx(); x++) {
                        xpmpix[y-b][x-a] = '+';
                        if (D <= 0) {
                            D += 2*dy;
                        }else {
                            D += 2*(dy - dx);
                            y++;
                        }
                        k++;
                    }
                }else if (slope > 1) {
                    D = 2*dx - dy;
                    int k = 0;
                    for (y = start.gety(); y <= end.gety(); y++) {
                        xpmpix[y-b][x-a] = '+';
                        if (D <= 0) {
                            D += 2*dx;
                        }else {
                            D += 2*(dx - dy);
                            x++;
                        }
                        k++;
                    }
                }else if (-1<slope && slope<0) {
                    D = 2*dy - dx;
                    int k = 0;
                    for (x = start.getx(); x <= end.getx(); x++) {
                        xpmpix[y-b][x-a] = '+';
                        if (D <= 0) {
                            D += 2*dy;
                        }else {
                            D += 2*(dy - dx);
                            y--;
                        }
                        k++;
                    }
                }else if (slope == 1) {
                    int k = 0;
                    for (x = start.getx(); x <= end.getx(); x++) {
                        xpmpix[y-b][x-a]='+';
                        y++;
                        k++;
                    }
                }else if (slope == -1) {
                    int k = 0;
                    for (x = start.getx(); x <= end.getx(); x++) {
                        xpmpix[y-b][x-a] = '+';
                        y--;
                        k++;
                    }
                }else if (slope == 0) {
                    int k = 0;
                    for (x = start.getx(); x <= end.getx(); x++) {
                        xpmpix[y-b][x-a] = '+';
                        k++;
                    }
                }
                else { // i.e., slope<-1
                    D = 2*dx - abs(dy);
                    int k = 0;
                    for (y = start.gety(); y >= end.gety(); y--) {
                        xpmpix[y-b][x-a] = '+';
                        if (D <= 0) {
                            D += 2*dx;
                        }else {
                            D += 2*(dx - dy);
                            x++;
                        }
                        k++;
                    }
                }
            }else if (sd_exist == false) { // for vertical lines
                int k = 0;
                int x = start.getx();
                for (int y = start.gety(); y <= end.gety(); y++) {
                    xpmpix[y-b][x-a] = '+';
                    k++;
                }
            }
        }else cout<<"Do not need to process this line."<<endl;
    }
};

//Define all functions for Cohen-Sutherland algorithm---------------------------
int cstest(Line argl);
Line csclip(Line argl);

//Here is main function---------------------------------------------------------
int main(int argc, char * argv[]) {
    // analyze all the input options
    optana(argc, argv);
    int width = c - a + 1; // height of the world window
    int height = d - b + 1; // width of the world window
    xpmpix.resize(height); // set vector as a 2-d array
    for (int i = 0; i < xpmpix.size(); i++) {
        xpmpix[i].resize(width);
    }
    for (int i = 0; i < height; i++) { // Initailize all char to be '-', which stands for white pixel
        for (int j = 0; j < width; j++) { // i.e., all pixels should be white at first
            xpmpix[i][j] = '-'; // stands for point(y,x)
        }
    }
    // read and buffer input *.ps file
    input = "/Users/wushangqi/hw1.ps"; // dir only for debug*********************************
    ifstream infile(input.c_str(),ios::in);
    if (!infile) {
        cout<<"File does not exist, please check your path."<<endl;
        abort();
    }
    string buff;
    bool store = false;
    int buff_pt[4];
    int buff_i = 0;
    vector<Line> vecline; // Since the number of lines is not sure, define a vecter for Line.
    while (infile) {
        infile>>buff;
        if (buff.compare("%%%END") == 0) {
            store = false;
        }
        if (store == true && buff.compare("Line") != 0) {
            buff_pt[buff_i] = atoi(buff.c_str());
            buff_i++;
            if (buff_i == 4) {
                Line buff_line;
                buff_line.setall(buff_pt[0],buff_pt[1],buff_pt[2],buff_pt[3]);
                vecline.push_back(buff_line);
            }
        }
        if (buff.compare("%%%BEGIN") == 0) {
            store = true;
        }
        if (buff.compare("Line") == 0) {
            if (buff_i == 4) {
                buff_i = 0;
            }else {
                cout<<"There must be 4 values for one line. Please check the validity of your input file."<<endl;
                abort();
            }
        }
    }
    infile.close();
    // Do transformations before clip a line.
    if (s != 1.0) { // Do the scaling first.
        for (int i = 0; i < vecline.size(); i++) {
            vecline[i].scale(s);
        }
    }
    if (r != 0) { //Do rotation second.
        for (int i = 0; i < vecline.size(); i++) {
            vecline[i].rot(r);
        }
    }
    if (m != 0 || n != 0) { // Do translation last.
        for (int i = 0; i < vecline.size(); i++) {
            vecline[i].trans(m, n);
        }
    }

    // Find out the lines are not in the world window and erase them.
    for (int i = 0; i < vecline.size(); i++) { // Use Cohen-Sutherland algorithm to clip lines.
        vecline[i] = csclip(vecline[i]);
    }
    for (int i = 0; i < vecline.size(); i++) { // Write pixels for all lines in the world window.
        if (vecline[i].getin() == true) {
            vecline[i].showall();
        }
    }
    vecline.clear();
    // Prepare to wirte output file.
    string xpmheader = setheader();
    string xpmend = setend();
    string line = "";
    output = "/Users/wushangqi/out.xpm"; // dir only for debug ******************************************
    ofstream out(output.c_str(), ios::out);
    if (!out) {
        cout<<"Cannot write an output file, please check the validity of output path."<<endl;
    }
    out<<xpmheader<<endl;
    cout<<xpmheader<<endl;
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j< width; j++) {
            line += xpmpix[i][j];
        }
        line = "\"" + line + "\"";
        if (i != 0) {
            line = line + ",";
        }
        out<<line<<endl;
        cout<<line<<endl;
        line.clear();
    }
    out<<xpmend<<endl;
    cout<<xpmend<<endl;
    out.close();
    xpmpix.clear();
    string shell = "display " + output;
    system(shell.c_str());
    cout<<shell<<endl;
    return 0;
}

//-------------------------------------------------
int cstest(Line argl) { // Return 1 or 2 or 3 if the line is completely visible. The simple test of C-S algorithm
    int xs = argl.gets().getx();
    int ys = argl.gets().gety();
    int xe = argl.gete().getx();
    int ye = argl.gete().gety();
    if (((b<=ys&&ys<=d) && (b<=ye&&ye<=d)) && (xs<a) && (xe>c)) {
        return 1; //Lines go from WL to WR.
    } else if ((a<=xs&&xs<=c) && (a<=xe&&xe<=c) && ((ys<b && ye>d) || (ys>d && ye<b))) {
        return 2; //Lines go from WB to WT.
    } else if ((a<=xs&&xs<=c) && (a<=xe&&xe<=c) && (b<=ys&&ys<=d) && (b<=ye&&ye<=d)) {
        return 3; //Lines begin and end within the world window.
    }else return 0; //Lines cannot pass the simple exam.
}

//--------------------------------------------------
Line csclip(Line argl) { //Futher test and clip the line by using Cohen-Sutherland algorithm.
    int xs = argl.gets().getx();
    int ys = argl.gets().gety();
    int xe = argl.gete().getx();
    int ye = argl.gete().gety();
    int ya = argl.cal_inter_wlr((float)a).gety(); // intersaction with WL(x=a)
    int yc = argl.cal_inter_wlr((float)c).gety(); // intersaction with WR(x=c)
    int xb = argl.cal_inter_wtb((float)b).getx(); // intersaction with WB(y=b)
    int xd = argl.cal_inter_wtb((float)d).getx(); // intersaction with WT(y=d)
    int flag = cstest(argl);
    if (flag == 0) { //If the line has not passed the simple test, take it to do the following complex test.
        if (xs < a && xe > c) {
            if ((b<=ya&&ya<=d) && (b<=yc&&yc<=d)) {
                argl.setall(a, ya, c, yc);
                argl.setin(true);
            } else if ((b<=ya&&ya<=d) && yc<b){
                argl.setall(a, ya, xb, b);
                argl.setin(true);
            } else if ((b<=ya&&ya<=d) && yc>d) {
                argl.setall(a, ya, xd, d);
                argl.setin(true);
            } else if (ya<b && (b<=yc&&yc<=d)) {
                argl.setall(xb, b, c, yc);
                argl.setin(true);
            } else if (ya>d && (b<=yc&yc<=d)) {
                argl.setall(xd, d, c, yc);
                argl.setin(true);
            } else if (ya<b && yc>d) {
                argl.setall(xb, b, xd, d);
                argl.setin(true);
            } else if (ya>d && yc<b) {
                argl.setall(xd, d, xb, b);
                argl.setin(true);
            }else {argl.setin(false);}
        }else if (a<=xs && xs<=c && xe>c) {
            if ((b<=ys&&ys<=d) && (b<=yc&&yc<=d)) {
                argl.setall(xs, ys, c, yc);
                argl.setin(true);
            }else if ((b<=ys&ys<=d) && yc<b) {
                argl.setall(xs, ys, xb, b);
                argl.setin(true);
            }else if ((b<=ys&&ys<=d) && yc>d) {
                argl.setall(xs, ys, xd, d);
                argl.setin(true);
            }else if (ys<b && (b<=yc&&yc<=d)) {
                argl.setall(xb, b, c, yc);
                argl.setin(true);
            }else if (ys<b && yc>d) {
                argl.setall(xb, b, xd, d);
                argl.setin(true);
            }else if (ys>d && (b<=yc&&yc<=d)) {
                argl.setall(xd, d, c, yc);
                argl.setin(true);
            }else if (ys>d && yc<b) {
                argl.setall(xd, d, xb, b);
                argl.setin(true);
            }else {argl.setin(false);}
        }else if (xs<a && a<=xe && xe<=c) {
            if ((b<=ya&&ya<=d) && (b<=ye&&ye<=d)) {
                argl.setall(a, ya, xe, ye);
                argl.setin(true);
            }else if (ya<b && (b<=ye&&ye<=d)) {
                argl.setall(xb, b, xe, ye);
                argl.setin(true);
            }else if (ya>d && (b<=ye&&ye<=d)) {
                argl.setall(xd, d, xe, ye);
                argl.setin(true);
            }else if ((b<=ya&&ya<=d) && ye<b) {
                argl.setall(a, ya, xb, b);
                argl.setin(true);
            }else if ((b<=ya&&ya<=d) && ye>d) {
                argl.setall(a, ya, xd, d);
                argl.setin(true);
            }else if (ya<b && ye>d) {
                argl.setall(xb, b, xd, d);
                argl.setin(true);
            }else if (ya>d &&ye<b) {
                argl.setall(xd, d, xb, b);
                argl.setin(true);
            }else {argl.setin(false); }
        }else if ((a<=xs&&xs<=c) && (a<=xe&&xe<=c)) {
            if (ys<b && (b<=ye&&ye<=d)) {
                argl.setall(xb, b, xe, ye);
                argl.setin(true);
            }else if (ys<b && ye>d) {
                argl.setall(xb, b, xd, d);
                argl.setin(true);
            }else if (ys>d && ye<b) {
                argl.setall(xd, d, xb, b);
                argl.setin(true);
            }else if (ys>d && (b<=ye&&ye<=d)) {
                argl.setall(xd, d, xe, ye);
                argl.setin(true);
            }else if ((b<=ys&&ys<=d) && ye<b) {
                argl.setall(xs, ys, xb, b);
                argl.setin(true);
            }else if ((b<=ys&&ys<=d) && ye>d) {
                argl.setall(xs, ys, xd, d);
                argl.setin(true);
            }else if ((b<=ys&&ys<=d) && (b<=ye&&ye<=d)) {
                argl.setin(true);
            }else argl.setin(false);
        }else {argl.setin(false);}
    }else if (flag == 1) {
        argl.setall(a, ya, c, yc);
        argl.setin(true);
    }else if (flag == 2) {
        argl.setall(xb, b, xd, d);
        argl.setin(true);
    }else if (flag == 3) { // Lines totally within the window do not need to be clipped.
        argl.setin(true);
    }
    else {argl.setin(false);} // Mark and ignore all lines out the world window.
    if (argl.gets().getx() == argl.gete().getx() && argl.gets().gety() == argl.gete().gety()) {
        argl.setin(false);
    }
    // for testing clipping result
    /*xs = argl.gets().getx();
     ys = argl.gets().gety();
     xe = argl.gete().getx();
     ye = argl.gete().gety();
     cout<<"xs "<<xs<<"  ys  "<<ys<<"  xe  "<<xe<<"  ye  "<<ye<<endl;*/
    return argl;
}

//----------------------------------------
void help(){
    cout<<"[-f] The next argument is the input \"Postscript\" file."<<endl;
    cout<<"[-s] This next argument is a float specifying the scaling factor in both dimensions about the world origin."<<endl;
    cout<<"[-r] This next argument is an integer specifying the number of degrees for a counter-clockwise rotation about the world origin."<<endl;
    cout<<"[-m] The next argument is an integer specifying a translation in the x dimension."<<endl;
    cout<<"[-n] The next argument is an integer specifying a translation in the y dimension."<<endl;
    cout<<"[-a] The next argument is an integer lower bound in the x dimension of the world window."<<endl;
    cout<<"[-b] The next argument is an integer lower bound in the y dimension of the world window."<<endl;
    cout<<"[-c] The next argument is an integer upper bound in the x dimension of the world window."<<endl;
    cout<<"[-d] The next argument is an integer upper bound in the y dimension of the world window."<<endl;
    cout<<"This program will generate ./out.xpm file automatically."<<endl;
}


//-----------------------------------------
int rnd(float arg){ //return a rounded vaule of a float
    if (arg > 0) {
        return (int)(arg + 0.5);
    }else {
        return (int)(arg - 0.5);
    }
}

//-------------------------------------------
void optana(int argc, char * const argv[]){
    // analyze input option and set defualt options
    float temp;
    int opt;
    while ((opt = getopt(argc, argv, "f:a:b:c:d:s:m:n:r:h"))!= -1) {
        switch (opt) {
            case 'f':{
                input = optarg;
                break;
            }
            case 'a':{
                string astr(optarg);
                temp = atof(astr.c_str());
                a = rnd(temp);
                break;
            }
            case 'b':{
                string bstr(optarg);
                temp = atof(bstr.c_str());
                b = rnd(temp);
                break;
            }
            case 'c':{
                string cstr(optarg);
                temp = atof(cstr.c_str());
                c = rnd(temp);
                break;
            }
            case 'd':{
                string dstr(optarg);
                temp = atof(dstr.c_str());
                d = rnd(temp);
                break;
            }
            case 'm':{
                string mstr(optarg);
                temp = atof(mstr.c_str());
                m = rnd(temp);
                break;
            }
            case 'n':{
                string nstr(optarg);
                temp = atof(nstr.c_str());
                n = rnd(temp);
                break;
            }
            case 'r':{
                string rstr(optarg);
                temp = atof(rstr.c_str());
                r = rnd(temp);
                break;
            }
            case 's':{
                string sstr(optarg);
                s = atof(sstr.c_str());
                break;
            }
            case 'h':help();abort();
                break;
            default:cout<<"please enter -h for help"<<endl;abort();
                break;
        }
    }
    if (a > c) { // switch if a is bigger than c
        int tmp = c;
        c = a;
        a = tmp;
    }
    if (b > d) {
        int tmp = d;
        d = b;
        b = tmp;
    }
    if (a == c || b == d) {
        cout<<"Lower bound and upper bound value cannot be the same."<<endl;
        abort();
    }
}

//----------------------------------------
string setheader() {
    stringstream tmp;
    int w = c - a + 1;
    int h = d - b + 1;
    tmp<<w;
    string intw;
    tmp>>intw;
    tmp.clear();
    tmp<<h;
    string inth;
    tmp>>inth;
    string str = "/* XPM file for Computer Graphics Assaignment 2 */\nstatic char *CG_hw2[] = {\n/* width height num_colors chars_per_pixel */\n\"" + intw + " " + inth +" 2 1\",\n/* colors */\n\"- c #ffffff\",\n\"+ c #000000\",\n/* pixels */";
    return str;
}

//----------------------------------------
string setend() {
    string str = "};";
    return str;
}