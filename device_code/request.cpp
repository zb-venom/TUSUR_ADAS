#include <stdlib.h>
#include "stdio.h"
#include "mstn_gpio.h"
#include "../Serial.h"
#include "../FSerial.h"
#include "mstn_usb.h"
#include <string>
#include "../w5100.h"
#include "../Ethernet.h"
#include "../Dhcp.h"
#include "request.h"
#define Zum D8
//отпрвавка данных на сервер
bool make_user_code_req(uint8_t *serv, byte code [16], byte user_code[16]) { 
        EthernetClient client;//создание клиента сервера
        if (client.connect(serv, 80)) {
            printf("conect\n");
            client.write("\r\n");
            //настройка запроса
            //client.write("GET /add.php?code=%c HTTP/1.1\r\nHost: 192.168.100.4\r\nConnection: close\r\n\r\n", user_code);
            char lol[18] = {'G','E','T',' ','/','a','d','d','.','p','h','p','?','c','o','d','e','='};            
            char lol2[11] = {'&','u','s','e','r','_','c','o','d','e','='};
            char kek[53] = { ' ', 'H','T','T','P','/','1','.','1','\r','\n','H','o','s','t',':',' ','1','9','2','.','1','6','8','.','1','0','0','.','4','\r','\n','C','o','n','n','e','c','t','i','o','n',':',' ','c','l','o','s','e','\r','\n','\r','\n'};
            char temp[114]; int i = 0;
            //заполнение массива данными о запросе и данными 
            while (i < 18) {temp[i]=lol[i]; i++;} i = 0;  
            while (i < 16) {temp[i+18]=code[i]; i++;} i = 0; 
            while (i < 11) {temp[i+34]=lol2[i]; i++;} i = 0;            
            while (i < 16) {temp[i+45]=user_code[i]; i++;} i = 0; 
            while (i < 53) {temp[i+61]=kek[i]; i++;}       
            client.write(temp);//запись
            
        }
        //если произошла ошибка с соединением сервером
        else {
        
             pinMode(Zum,OUTPUT);
             analogWrite(Zum,50);
             delay(1000);
             analogWrite(Zum,0);
             delay(1000);
            printf ("EROR\n");
            return false;
        }
        //слушает сервер
        while (1) {
            if (client.available() > 0){
                char read = client.read();            
                printf("%c", read);
            }
        }
        //разрыв соединение с сервером
        client.stop();  
        return true;
}
//подключение к серверу
bool Ethernet_request ::intialazion_server(byte user_device[16], byte user_login[16]){
        Delay (5000);
        uint8_t mac[] = { 0x90, 0xAD, 0xDA, 0x0D, 0x96, 0xFE };// Mac - адрес сервера
        uint8_t serv [] = { 192, 168, 100, 4 };//адрес сервера 
        uint8_t ip [] = { 192, 168, 100, 215 };//ip-adress
        Ethernet.begin(mac, ip)
        uint8_t * your_ip = Ethernet.localIP();    
        uint8_t * your_mac = Ethernet.localMAC();
        //printf ("ip: %hu.%hu.%hu.%hu \n",your_ip[0],your_ip[1],your_ip[2],your_ip[3]);    
       // printf ("mac: %hu.%hu.%hu.%hu.%hu.%hu \n",your_mac[0],your_mac[1],your_mac[2],your_mac[3],your_mac[4],your_mac[5]); 
        if(make_user_code_req(serv, user_device, user_login)==false){ // отрправка дынных на сервера
            return false;
        }
        
        return true;
    
}/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

