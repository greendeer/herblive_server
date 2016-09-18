//
// Created by greendeer on 16. 9. 13.
//
#include "stdio.h"
#include "stdlib.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "ros/ros.h"
#include "std_msgs/Int16.h"

#define BUF_LEN 128
#define PORT 2017

char buffer[BUF_LEN];
int val[4] = {-1};
bool light_update = false;
bool temp_update = false;
bool thirst_update = false;
bool water_update = false;

// #T3,B2,S2; (t- temp. b- light, s-thirst) M1 light. M2 Temp. M3 Thirst.
// watering #M4;

void val2msg(){
    memset(buffer, 0x00, BUF_LEN);
    buffer[0] = '#';
    int i=1;
    if(light_update){
        buffer[i] = 'B';
        i++;
        buffer[i] = val[0]+'0';
        i++;
        buffer[i] = ',';
        i++;
    }
    if(temp_update){
        buffer[i] = 'T';
        i++;
        buffer[i] = val[1]+'0';
        i++;
        buffer[i] = ',';
        i++;
    }
    if(thirst_update){
        buffer[i] = 'S';
        i++;
        buffer[i] = val[2]+'0';
        i++;
        buffer[i] = ',';
        i++;
    }
    if(water_update){
        buffer[i] = 'M';
        i++;
        buffer[i] = '4';
        i++;
        buffer[i] = ',';
        i++;
    }
    buffer[i-1] = ';';
    buffer[i] = '\n';

}

void startMsg(){
    memset(buffer, 0x00, BUF_LEN);
    buffer[0] = '#';
    buffer[1] = 'B';
    buffer[2] = val[0]+'0';
    buffer[3] = ',';
    buffer[4] = 'T';
    buffer[5] = val[1]+'0';
    buffer[6] = ',';
    buffer[7] = 'S';
    buffer[8] = val[2]+'0';
    buffer[9] = ';';
    buffer[10] = '\n';
}

int main(int argc, char** argv){

    struct sockaddr_in server_addr, client_addr;
    char temp[20];
    int server_fd, client_fd;
    socklen_t len;
    int msg_size;

    ros::init(argc, argv, "herb_push_node");
    ros::NodeHandle n;
    auto rate = ros::Rate(10);

    //socket make
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0))== -1){
        printf("Server : Can't open stream socket\n");
        exit(0);
    }
    memset(&server_addr, 0x00, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){
        printf("Server : Can't bind local address.\n");
        exit(0);
    }

    if(listen(server_fd, 5) <0){
        printf("Server : Can't listening connect.\n");
        exit(0);
    }

    len = sizeof(client_addr);

    while(ros::ok()){
        printf("Server : wating connection request.\n");
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
        if(client_fd < 0){
            printf("Server: accept failed.\n");
            exit(0);
        }
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp, sizeof(temp));
        printf("Server : %s client connected.\n", temp);
        int t = -1;
        if(n.getParam("/lightLV",t)){
                val[0] = t;
        }
        if(n.getParam("/tempLV",t)){
                val[1] = t;
        }
        if(n.getParam("/thirstLV",t)){
                val[2] = t;
        }
        startMsg();
        printf("%s\n",buffer);
        //write anything
        if((write(client_fd, buffer, strlen(buffer)))<=0){
            printf("write error");
            close(client_fd);
            printf("Server : %s client closed.\n", temp);
            continue;
        }
        while(1) {
//            msg_size = read(client_fd, buffer, BUF_LEN);
//            if(msg_size <= 0){
//                printf("Server : client read error\n");
//                close(client_fd);
//                printf("Server : %s client closed.\n", temp);
//                break;
////            }
//            int t = -1;
//            if(n.getParam("/lightLV",t)){
//                val[0] = t;
//            }
//            if(n.getParam("/tempLV",t)){
//                val[1] = t;
//            }
//            if(n.getParam("/thirstLV",t)){
//                val[2] = t;
//            }
//            startMsg();
//            printf("%s\n",buffer);
//            //write anything
//            if((write(client_fd, buffer, strlen(buffer)))<=0){
//                printf("write error");
//                close(client_fd);
//                printf("Server : %s client closed.\n", temp);
//                break;
//            }

            int t = -1;
            if(n.getParam("/lightLV",t)){
                if(val[0]!=t){
                    val[0] = t;
                    light_update = true;
                }
            }
            if(n.getParam("/tempLV",t)){
                if(val[1]!=t){
                    val[1] = t;
                    temp_update = true;
                }
            }
            if(n.getParam("/thirstLV",t)){
                if(val[2]!=t){
                    val[2] = t;
                    thirst_update = true;
                }
            }
            if(n.getParam("/waterLV",t)){
                if(val[3]!=t){
                    val[3] = t;
                    water_update = true;
                }
            }

            if(light_update||temp_update||thirst_update||water_update){

                val2msg();
                printf("%s\n",buffer);
                //write anything
                if((write(client_fd, buffer, strlen(buffer)))<=0){
                    printf("write error");
                    close(client_fd);
                    printf("Server : %s client closed.\n", temp);
                    break;
                }
                light_update = false;
                temp_update = false;
                thirst_update = false;
                water_update = false;
            }
            //memset(buffer, 0x00, sizeof(buffer));
            ros::spinOnce();
            rate.sleep();
        }
    }

    close(server_fd);
}