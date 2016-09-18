//
// Created by greendeer on 16. 9. 13.
//
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "std_msgs/Int16.h"

// #T3,B2,S2; (t- temp. b- light, s-thirst) M1 light. M2 Temp. M3 Thirst.
// watering #M4;

double LIGHT_MIN =1000;
double LIGHT_MAX =3000;
double TEMP_MIN =25;
double TEMP_MAX =27;
double THRST_MIN =65;
double THRST_MAX =80;


int calLightLv(double light){
    if(light < LIGHT_MIN/4){
        return 0;
    }else if(light < (LIGHT_MIN/4)*2){
        return 1;
    }else if(light < (LIGHT_MIN/4)*3){
        return 2;
    }else if(light < LIGHT_MIN){
        return 3;
    }else if(light < LIGHT_MAX){
        return 4;
    }else if(light < LIGHT_MAX + (5000-LIGHT_MAX)/4){
        return 5;
    }else if(light < LIGHT_MAX + ((5000-LIGHT_MAX)/4)*2){
        return 6;
    }else if(light < LIGHT_MAX + ((5000-LIGHT_MAX)/4)*3){
        return 7;
    }else if(light < 5000){
        return 8;
    }else{
        return 9;
    }
}

int calTempLv(double temp){
    if(temp < TEMP_MIN/4){
        return 0;
    }else if(temp < (TEMP_MIN/4)*2){
        return 1;
    }else if(temp < (TEMP_MIN/4)*3){
        return 2;
    }else if(temp < TEMP_MIN){
        return 3;
    }else if(temp < TEMP_MAX){
        return 4;
    }else if(temp < TEMP_MAX + (50-TEMP_MAX)/4){
        return 5;
    }else if(temp < TEMP_MAX + ((50-TEMP_MAX)/4)*2){
        return 6;
    }else if(temp < TEMP_MAX + ((50-TEMP_MAX)/4)*3){
        return 7;
    }else if(temp < 50){
        return 8;
    }else{
        return 9;
    }
}

int calThirstLv(double temp){
    if(temp < THRST_MIN/4){
        return 0;
    }else if(temp < (THRST_MIN/4)*2){
        return 1;
    }else if(temp < (THRST_MIN/4)*3){
        return 2;
    }else if(temp < THRST_MIN){
        return 3;
    }else if(temp < THRST_MAX){
        return 4;
    }else if(temp < THRST_MAX + (100-THRST_MAX)/4){
        return 5;
    }else if(temp < THRST_MAX + ((100-THRST_MAX)/4)*2){
        return 6;
    }else if(temp < THRST_MAX + ((100-THRST_MAX)/4)*3){
        return 7;
    }else if(temp < 100){
        return 8;
    }else{
        return 9;
    }
}

int calWaterLv(double t){
    return (int) t/410;
}

int main(int argc, char** argv){

    ros::init(argc,argv,"herb_calcul_node");
    ros::NodeHandle n;

    auto rate = ros::Rate(10);

    double val = -1;

    while(ros::ok()) {

        if (n.getParam("/light", val)) {
            n.setParam("/lightLV", calLightLv(val));
        }
        if (n.getParam("/air_t", val)) {
            n.setParam("/tempLV", calTempLv(val));
        }
        if (n.getParam("/soil_h", val)) {
            n.setParam("/thirstLV", calThirstLv(val));
        }
        if (n.getParam("/water", val)) {
            n.setParam("/waterLV",calWaterLv(val));
        }

        rate.sleep();
        ros::spinOnce();
    }
}
