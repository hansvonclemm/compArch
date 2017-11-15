//
//  asteroids.cpp
//  Created by Hans von Clemm on 10/26/17.


#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace std;

//global variables
float dt = 0.1; //step
int dmin = 2;
int width = 200;
int height = 200;
int ray_width = 4;
double gravC = pow(6.674, -5);
int m = 1000;
int sdm = 50;

struct Asteroids{
    
    double xpos;
    double ypos;
    double mass;
    
    double xvel;
    double yvel;
    
    double fx;
    double fy;
    
    bool blownUp;
    
    
};

struct Planets{
    double xpos;
    double ypos;
    double mass;
    
};

void forceCalcA(int temp, int oppose, Asteroids *astArray){
    //calculate attractive force between the two and how much it'll cause it to move --> using formulas
    //distance between
    
    //Grab values
    double tempX = astArray[temp].xpos;
    double tempY = astArray[temp].ypos;
    double opposeX = astArray[oppose].xpos;
    double opposeY = astArray[oppose].ypos;
    double tempM = astArray[temp].mass;
    double opposeM = astArray[oppose].mass;
    
    double dist = sqrt(pow((tempX - opposeX), 2) + pow((tempY - opposeY), 2));
    double angle;
    double slope;
    
    //cout << "Distance between " << (temp + 1) << " and " << (oppose + 1) << " = " <<  dist << endl;
    
    if (dist > 2){
        slope = (opposeY - tempY) / (opposeX - tempX);
        //cout << "slope: " << slope << endl;
        //cout << " ANGLE BEFORE TRUNCATION: " << atan(slope) << " radians " << endl;
        
        if (slope > 1 || slope < -1){
            
            //cout << "SLOPE TRUNC # is " << trunc(slope) << endl;
            
            slope = slope - trunc(slope);  //UNCOMMENT THIS TO HAVE THE SLOPE TRUNCATED
            
            //cout << "truncated -> new slope: " << slope << endl;
        }
    }
    else{
        astArray[temp].fx = 0;
        astArray[temp].fy = 0;
        return;
    }
    angle = atan(slope);
    //cout << "angle calculated is " << angle << " radians." << endl;

    
    //double fx =
    double fx = ((gravC * tempM * opposeM) / dist) * cos(angle);
    
    //cout << fixed << setprecision(12) << fx << endl;
    
    double fy = ((gravC * tempM * opposeM) / dist) * sin(angle);
    
    if (tempX < opposeX){// then the force should be positive
        if (fx < 0){
            fx = fx * -1;
            //cout << "flag1" << endl;
        }
    }
    if (tempX > opposeX){ // force should be negative
        if (fx > 0){
            fx = fx * -1;
            //cout << "flag2" << endl;
        }
        
    }
    if (tempY < opposeY){ //force should be positive
        if (fy < 0){
            fy = fy * -1;
        }
    }
    
    if (tempY > opposeY){ // force should be negative
        if (fy > 0){
            fy = fy * -1;
        }
    }
    
    astArray[temp].fx = fx;
    astArray[temp].fy = fy;
}

void forceCalcP(int a, int p, Asteroids *astArray, Planets *planetArray){ //force calculator for force between planet and asteroid
    
    double astX = astArray[a].xpos;
    double astY = astArray[a].ypos;
    double planetX = planetArray[p].xpos;
    double planetY = planetArray[p].ypos;
    double astM = astArray[a].mass;
    double planetM = planetArray[p].mass;
    double angle;
    double slope = 0;
    
    double dist = sqrt(pow((astX - planetX), 2) + pow((astY - planetY), 2));
    
    if (dist > 2){
        double slope = (astX - planetX) / (astY - planetY);
        
        if (slope > 1 || slope < -1){
            slope = slope - trunc(slope); //UNCOMMENT THIS TO HAVE THE SLOPE BE TRUNCATED
            
        }
        
    }
    else{
        astArray[a].fx = 0;
        astArray[a].fy = 0;
        return;
    }
    
    angle = atan(slope);
    double fx = ((gravC * planetM * astM) / dist) * cos(angle);
    double fy = ((gravC * planetM * astM) / dist) * sin(angle);
    
    if (astX < planetX){// then the force should be positive
        if (fx < 0){
            fx = fx * -1;
            //cout << "flag1" << endl;
        }
    }
    if (astX > planetX){ // force should be negative
        if (fx > 0){
            fx = fx * -1;
            //cout << "flag2" << endl;
        }
        
    }
    if (astY < planetY){ //force should be positive
        if (fy < 0){
            fy = fy * -1;
            //cout << "flag3" << endl;
        }
    }
    
    if (astY > planetY){ // force should be negative
        if (fy > 0){
            fy = fy * -1;
            //cout << "flag4" << endl;
        }
    }
    
    astArray[a].fx = fx;
    astArray[a].fy = fy;
}

void movement(int index, double sumFX, double sumFY, Asteroids *astArray){
    //this will be calculating the acceleration AT EACH POINT IN TIME
    double accelX = sumFX / astArray[index].mass;
    //cout << "accelX = " << accelX << endl;
    double accelY = sumFY / astArray[index].mass;
    
    astArray[index].xvel = astArray[index].xvel + accelX * dt;
    astArray[index].yvel = astArray[index].yvel + accelY * dt;
    
    astArray[index].xpos = astArray[index].xpos + astArray[index].xvel * dt;
    astArray[index].ypos = astArray[index].ypos + astArray[index].yvel * dt;
    
}

int main(int argc, char *argv[]){
    
    if (argc != 6){ //error argument for nasteroids-seq
        cout << "nasteroids-seq: Wrong arguments." << endl;
        cout << "Correct use:" << endl;
        cout << "nasteroids-seq num_asteroids num_iterations num_planets pos_ray seed" << endl;
        exit(-1);
    }
    
    else {
        cout << "Inputs Correct." << endl;
    }
    
    
    int num_asteroids = stoi(argv[1]);
    int num_iterations = stoi(argv[2]);
    int num_planets = stoi(argv[3]);
    double pos_ray = stod(argv[4]);
    int seed = stoi(argv[5]);
    cout << num_asteroids << " " << num_iterations << " " << num_planets << " " << fixed << setprecision(1) << pos_ray << " " << seed << endl;
    
    //cout initial conditions inputed by the user
    string initStr = to_string(num_asteroids) + " " + to_string(num_iterations) + " " + to_string(num_planets) + " " + to_string(pos_ray) + " " + to_string(seed);
    //cout << initStr << endl;
    
    ofstream initFile;
    initFile.open ("init_conf.txt");
    initFile << initStr << endl;
    initFile.close();
    
    Asteroids * astArray = new Asteroids[num_asteroids];
    Planets * planetArray = new Planets[num_planets];

    
    //random distributions
    default_random_engine re(seed);
    uniform_real_distribution<double> xdist(0.0, std::nextafter(width, std::numeric_limits<double>::max()));
    uniform_real_distribution<double> ydist(0.0, std::nextafter(height, std::numeric_limits<double>::max()));
    normal_distribution<double> mdist(m, sdm);
    cout << fixed << setprecision(4);
    
    //cout << xdist(re) << " " << ydist(re) << endl;
    //cout << "Seed is " << seed << endl;
    
    
    //assign values for pos, mass etc for all asteroids and planets
    for (int i = 0; i < num_asteroids; i++){
        astArray[i].xpos = xdist(re);
        astArray[i].ypos = ydist(re);
        astArray[i].mass = mdist(re);
        astArray[i].xvel = 0;
        astArray[i].yvel = 0;
        astArray[i].blownUp = false;
    }
    
    for (int i = 0; i < num_planets; i++){
        if (i == 0 || i == 4){
            //left axis, x = 0
            planetArray[i].ypos = ydist(re);
            planetArray[i].xpos = 0;
        }
        if (i == 1 || i == 5){
            //top border, y = 0
            planetArray[i].xpos = xdist(re);
            planetArray[i].ypos = 0;
            
        }
        if (i == 2 || i == 6){
            //right border, x = width
            planetArray[i].ypos = ydist(re);
            planetArray[i].xpos = width;
        }
        if (i == 3 || i == 7){
            //bottom border, y = height
            planetArray[i].xpos = xdist(re);
            planetArray[i].ypos = height;
        }
        //double planetMass =
        planetArray[i].mass = mdist(re) * 10;
    }
    
    
    //print statements for asteroids and planets
    for (int i = 0; i < num_asteroids; i++){
        cout << fixed << setprecision(3) << astArray[i].xpos << " " << astArray[i].ypos << " " << astArray[i].mass << endl;
    }
    
    for (int i = 0; i < num_planets; i++){
        cout << fixed << setprecision(3) << planetArray[i].xpos << " " << planetArray[i].ypos << " " << planetArray[i].mass << endl;
    }
    //print laser beam location
    cout << "0.000 " << pos_ray << endl;
    
    for (int t = 0; t < num_iterations; t++){
        
        //cout << endl << "time step #" << (t+1) << endl;
    
        //calculate in each time stamp
        for (int i = 0; i < num_asteroids; i++){
            //cout << endl << "Asteroid at " << (i+1) << endl;
            double fx = 0;
            double fy = 0;
            
            for (int j = 0; j < num_asteroids; j++){
                
                if (i != j){
                    forceCalcA(i, j, astArray);
                    
                    fx = fx + astArray[i].fx;
                    fy = fy + astArray[i].fy;
                    
                    //cout << fixed << setprecision(8) << "Force between asteroids " << (i+1) << " and " << (j+1) << " : fx = " << astArray[i].fx << " , fy = " << astArray[i].fy << endl;
                }
            }
            
            for (int k = 0; k < num_planets; k++){
                //calaculate attractive force between asteroid [i] and planet in question --> using formulas
                forceCalcP(i, k, astArray, planetArray);
                
                //cout << fixed << setprecision(8) << "Force between asteroid " << (i+1) << " and planet " << (k+1) << " : fx = " << astArray[i].fx << " , fy = " << astArray[i].fy << endl;
                
                fx = fx + astArray[i].fx;
                fy = fy + astArray[i].fy;
            }
            
            //cout << "Sum X forces: " << fx << " :: Sum Y forces: " << fy << endl;
            movement(i, fx, fy, astArray);
            
            //check if asteroid is within the Laser ray's path NOTE -----> it will make a difference when you delete the asteroid. since without its existence it won't act on the other ones.
            
            //rebound effect
            if (astArray[i].xpos <= 0){ // flip x velocity and set xpos = 2
                astArray[i].xpos = 2;
                astArray[i].xvel = astArray[i].xvel * -1;
            }
            
            if (astArray[i].xpos >= width){ // flip x velocity and set xpos = width - 2
                astArray[i].xpos = width - 2;
                astArray[i].xvel = astArray[i].xvel * -1;
            }
            
            if (astArray[i].ypos <= 0){ // flip y velocity and set ypos = 0
                astArray[i].ypos = 2;
                astArray[i].yvel = astArray[i].yvel * -1;
            }
            
            if (astArray[i].ypos >= height){
                astArray[i].ypos = height -2;
                astArray[i].yvel = astArray[i].yvel * -1;
            }
            
            
        }
        //CODE FOR DELETING ASTEROIDS
        /*
        for (int l = 0; l < num_asteroids; l++){
            if (astArray[l].ypos > (pos_ray - 2) && astArray[l].ypos < (pos_ray + 2)){
                cout << "BOOM THAT SHIT EXXPLODED   asteroid " << (l+1) << " deleted" << endl;
                astArray[l].blownUp = true;
                
                //code to delete asteroid from the array
                
                int newCount = num_asteroids - 1;
                Asteroids *tempArray = astArray;
                
                for (int z = l; z < newCount; z++){
                    tempArray[z] = astArray[z+1];
                }
                num_asteroids = newCount;
                astArray = tempArray;
                
            }
        }*/
        
        
    }
    
    for (int v = 0; v < num_asteroids; v++){
        cout << astArray[v].xpos << " " << astArray[v].ypos << " " << astArray[v].xvel << " " << astArray[v].yvel << " " << astArray[v].mass << endl;
        
        /*
        if (astArray[v].blownUp == true){
            cout << "Asteroid " << (v+1) << " was blown up" << endl;
        }
         */
    }
}



//Quick idea--> for the signs to be correct, if the x/y value of the opposing asteroid or planet is less than the other, then it should be negative and visa versa TOok care of this.




