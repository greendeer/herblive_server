//
// Created by greendeer on 16. 9. 13.
//
#include "stdio.h"
#include "stdlib.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "ros/ros.h"
#include "std_msgs/Float32.h"

#define BUF_LEN 128
#define PORT 2016


int main(int argc, char** argv){

    char buffer[BUF_LEN];
    struct sockaddr_in server_addr, client_addr;
    char temp[20];
    int server_fd, client_fd;
    socklen_t len;
    int msg_size;

    ros::init(argc, argv, "herb_server_node");
    ros::NodeHandle n;
    auto rate = ros::Rate(10);

    double val;

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

    memset(buffer, 0x00, sizeof(buffer));
    len = sizeof(client_addr);

    while(1){
        printf("Server : wating connection request.\n");
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
        if(client_fd < 0){
            printf("Server: accept failed.\n");
            exit(0);
        }
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp, sizeof(temp));
        printf("Server : %s client connected.\n", temp);
        while(1 && ros::ok()) {
            msg_size = read(client_fd, buffer, BUF_LEN);
            if(msg_size <= 0){
                printf("Server : client read error\n");
                close(client_fd);
                printf("Server : %s client closed.\n", temp);
                break;
            }else{
                printf("%s\n",buffer);
                val = atof(buffer+2);
                if(val>=0) {
                    if (buffer[0] == 'L' && buffer[1] == 'I') {
                        n.setParam("/light", val);
                    }else if (buffer[0] == 'S' && buffer[1] == 'T') {
                        n.setParam("/soil_t", val);
                    }else if (buffer[0] == 'S' && buffer[1] == 'H') {
                        n.setParam("/soil_h", val);
                    }else if (buffer[0] == 'A' && buffer[1] == 'T') {
                        n.setParam("/air_t", val);
                    }else if (buffer[0] == 'A' && buffer[1] == 'H') {
                        n.setParam("/air_h", val);
                    }else if (buffer[0] == 'W' && buffer[1] == 'A') {
                        n.setParam("/water", val);
                    }else{
                        printf("%s is error.\n", buffer);
                    }
                }
            }
            memset(buffer, 0x00, sizeof(buffer));
            rate.sleep();
        }
    }

    close(server_fd);
}