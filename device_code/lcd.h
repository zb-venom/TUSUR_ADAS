/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lcd_keybord.h
 * Author: касымхан
 *
 * Created on 18 мая 2019 г., 8:09
 */
#include <stdlib.h>
#include "stdio.h"
#include "mstn_gpio.h"
#include "../Serial.h"
#include "../FSerial.h"
#include "mstn_usb.h"
#include <string>
#include "mstn.h"
#include "../w5100.h"
#include "../Ethernet.h"
#include "../Dhcp.h"
#include "Arduino_w.h"
#ifndef LCD_H
#define LCD_H



#define	MT_A0	D0 //для экрана
#define	MT_E	D1
#define	MT_DB4	D2
#define MT_DB5	D3
#define MT_DB6	D4
#define MT_DB7	D5
class System{
private:
    int t =0;// счетчик
    int q = 0;// чтобы выбор сделать
    int k = 0;//для работы
    int r = 0;// для того чтобы фраза правильная высвечивалась
    byte mt16;
    byte sym;
    byte text[32] ;
    byte user_l[16];
    byte user_d[16]; 
    byte user [17];
    bool proverka;
    char value[4][4] = 
    {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };
public:
    int PinOut[4] {22, 23, 24, 25};
    int PinIn[4] {26, 27, 28, 29};
    void proverka_true();
    void proverka_false();
    bool proverka_answer();
    void mt1(byte m,byte n);
    void start_fraza();
    void mt16_s(byte mt);
    void mt16_d(byte mt);
    void mt16_k(byte mt);
    void on_start();
    void beg();
    void write_symbol_system();
    void write_symbol_user();
    void clear();
    void phrase();
    void enter();
    void enter_user(); 
    void keyboard();
    void choice_system();
    void choice_user();
    void choice();

};
#endif /* LCD_KEYBORD_H */

