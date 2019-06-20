#include "lcd.h"
#include "request.h"
void System::mt1(byte m, byte n){
    mt16 = 0x00;
    sym = 0x00;
}
void System::start_fraza()
{
    text[0] = 0x20;
    text[1] = 0x20;
    text[2] = 0x20;
    text[3] = 0x20;
    text[4] = 0x57;
    text[5] = 0x61;
    text[6] = 0x72;
    text[7] = 0x64;
    text[8] = 0x72;
    text[9] = 0x6f;
    text[10] = 0x62;
    text[11] = 0x65;
    text[12] = 0x20;
    text[13] = 0x20;
    text[14] = 0x20;
    text[15] = 0x20;
    text[16] = 0x20;
    text[17] = 0x20;
    text[18] = 0x77;
    text[19] = 0x65;
    text[20] = 0x6c;
    text[21] = 0x63;
    text[22] = 0x6f;
    text[23] = 0x6d;
    text[24] = 0x65;
    text[25] = 0x73;
    text[26] = 0x20;
    text[27] = 0x79;
    text[28] = 0x6f;
    text[29] = 0x75;
    text[30] = 0x20;
    text[31] = 0x20;
}
void System::mt16_s(byte mt)
{
    GPIO_DigitalWrite(MT_DB4, mt&16); //вывод старшего полубайта
    GPIO_DigitalWrite(MT_DB5, mt&32);
    GPIO_DigitalWrite(MT_DB6, mt&64);
    GPIO_DigitalWrite(MT_DB7, mt&128);
    GPIO_DigitalWrite(MT_E, 1);
    GPIO_DigitalWrite(MT_E, 0);
    GPIO_DigitalWrite(MT_DB4, mt&1); //вывод младшего полубайта
    GPIO_DigitalWrite(MT_DB5, mt&2);
    GPIO_DigitalWrite(MT_DB6, mt&4);
    GPIO_DigitalWrite(MT_DB7, mt&8);
    GPIO_DigitalWrite(MT_E, 1);
    GPIO_DigitalWrite(MT_E, 0); 
}
void System::mt16_d(byte mt)
{
    GPIO_DigitalWrite(MT_A0, 1); //режим записи данных
    mt16_s(mt);
    GPIO_DigitalWrite(MT_A0, 0);
    delayMicroseconds (40); //ожидание 40 мкс по даташиту
}
void System::mt16_k(byte mt)
{
    GPIO_DigitalWrite(MT_A0, 0); //режим записи команд
    mt16_s(mt);
    delayMicroseconds (40); //ожидание 40 мкс по даташиту
}
void System::on_start()
{
    mt16_k(0x80); //вывод верхней строки
    for (sym=0; sym<=15; sym++)
    {
        mt16_d(text[sym]);
    }
    mt16_k(0xC0); //вывод нижней строки
    for (sym=16; sym<=31; sym++)
    {
        mt16_d(text[sym]);
    }
}
void System::beg()
{
    int count = sizeof(text);
    int i = 0;
    while(count - i >= 16){
        
    mt16_k(0x80);
    for (sym=i; sym<=15+i; sym++)
    {
        mt16_d(text[sym]);
    }
    i++;
    delay(300);
    }
    on_start();
}
void System::write_symbol_system()
{
   mt16_k(0x80); //вывод верхней строки
    for (sym=0; sym<=15; sym++)
    {
        mt16_d(text[sym]);
    }
}
void System::write_symbol_user()
{
    mt16_k(0xC0); //вывод нижней строки
    for (sym=0; sym<=15; sym++)
    {
         mt16_d(user[sym]);
    }
    sym=0;
}

void System::clear()
{
        for(int p = 16; p < 32; p++)
            text[p] = 0x20;
        on_start();
}

void System::phrase()//фраза Введите код 
{
    text[0] = 0x45;
    text[1] = 0x6e;
    text[2] = 0x74;
    text[3] = 0x65;
    text[4] = 0x72;
    text[5] = 0x20;
    text[6] = 0x79;
    text[7] = 0x6f;
    text[8] = 0x75;
    text[9] = 0x72;
    text[10] = 0x20;
    text[11] = 0x63;
    text[12] = 0x6f;
    text[13] = 0x64;
    text[14] = 0x65;
    write_symbol_system ();
    for(int p = 0; p < 16; p++)
       {
           user [p] = 0x20;
       }
    write_symbol_user ();
}
void System::enter()
{
   proverka_false();
   for (int i = 0;i < 16; i++)
      user_l[i] = user[i];
   if ( true)
        {
            text[0] = 0x20;
            text[1] = 0x20;
            text[2] = 0x20;
            text[3] = 0x20;
            text[4] = 0x20;
            text[5] = 0x52;
            text[6] = 0x69;
            text[7] = 0x67;
            text[8] = 0x68;
            text[9] = 0x74;
            for (int i = 10; i < 16; i++)
               text[i] = 0x20;
            proverka_true();
            write_symbol_system ();
            clear();
            delay(2000);
            
       }
       else 
       {
            text[0] = 0x20;
            text[1] = 0x20;
            text[2] = 0x20;
            text[3] = 0x20;
            text[4] = 0x20;
            text[5] = 0x46;
            text[6] = 0x61;
            text[7] = 0x6c;
            text[8] = 0x73;
            text[9] = 0x65;
            for (int i = 10; i < 16; i++)
               text[i] = 0x20;
            write_symbol_system ();
           
       }
}
void System::enter_user()
{
   proverka_false();
   for (int i = 0;i < 16; i++)
      user_d[i]=user[i];
    Ethernet_request eth;
   if ( eth.intialazion_server(user_d,user_l))
        {
            if(r == 1)
            {
                //take_device(user_login, user_device);
                text[0] = 0x20;
                text[1] = 0x20;
                text[2] = 0x20;
                text[3] = 0x20;
                text[4] = 0x59;
                text[5] = 0x6f;
                text[6] = 0x75;
                text[7] = 0x20;
                text[8] = 0x74;
                text[9] = 0x6f;
                text[10] = 0x6f;
                text[11] = 0x6b;
                text[12] = 0x20;
                text[13] = 0x20;
                text[14] = 0x20;
                text[15] = 0x20;
            }
            if (r ==2)
            {
                //give_device(user_login, user_device);
                text[0] = 0x20;
                text[1] = 0x20;
                text[2] = 0x20;
                text[3] = 0x59;
                text[4] = 0x6f;
                text[5] = 0x75;
                text[6] = 0x20;
                text[7] = 0x72;
                text[8] = 0x65;
                text[9] = 0x74;
                text[10] = 0x75;
                text[11] = 0x72;
                text[12] = 0x6e;
                text[13] = 0x20;
                text[14] = 0x20;
                text[15] = 0x20; 
            }
            user[0] = 0x20;
            user[1] = 0x20;
            user[2] = 0x74;
            user[3] = 0x68;
            user[4] = 0x65;
            user[5] = 0x20;
            user[6] = 0x65;
            user[7] = 0x71;
            user[8] = 0x75;
            user[9] = 0x69;
            user[10] = 0x70;
            user[11] = 0x6d;
            user[12] = 0x65;
            user[13] = 0x6e;
            user[14] = 0x74;
            user[15] = 0x20;
            proverka_true();
            write_symbol_system ();
            write_symbol_user ();
            //clear();
            delay(2000);
            
       }
       else 
       {
           text[0] = 0x20;
           text[1] = 0x20;
           text[2] = 0x20;
           text[3] = 0x20;
           text[4] = 0x20;
           text[5] = 0x46;
           text[6] = 0x61;
           text[7] = 0x6c;
           text[8] = 0x73;
           text[9] = 0x65;
           for (int i = 10; i < 16; i++)
               text[i] = 0x20;
           write_symbol_system ();
           clear();
           delay(2000);
           choice_user();
           choice();
       } 
}
void System::keyboard() 
{
    for(int i = 0; i < 4; i++)
        {     
            digitalWrite(PinOut[i], LOW);
            for (int j = 0; j < 4; j++)
            {
                if (digitalRead(PinIn[j]) == LOW)
               {
                    if (value[i][j]=='B')
                    {
                        if (k == 0)
                            enter();
                        if (k == 1)
                            enter_user();
                    }
                    if (value[i][j]=='C')
                    {
                        user [t-1] = 0x20;
                        t--;
                        if (t<0) t=0;
                        write_symbol_user ();
                    }
                    if (value[i][j]=='0') 
                    {
                        user [t] = 0x30;
                        write_symbol_user ();
                        t++;
                    }
                    if (value[i][j]=='1') 
                    {
                            user [t] = 0x31;
                            write_symbol_user ();
                            t++;
                    }
                    if (value[i][j]=='2') 
                    {
                           user [t] = 0x32;
                            write_symbol_user ();
                            t++;
                    }
                    if (value[i][j]=='3') 
                    {
                        user [t] = 0x33;
                        write_symbol_user ();
                        t++;
                    }
                    if (value[i][j]=='4') 
                    {
                        user [t] = 0x34;
                        write_symbol_user ();
                        t++;
                    }
                    if (value[i][j]=='5') 
                    {
                        user [t] = 0x35;
                        write_symbol_user ();
                        t++;
                    }
                    if (value[i][j]=='6') 
                    {
                        user [t] = 0x36;
                        write_symbol_user ();
                        t++;
                    }
                    if (value[i][j]=='7') 
                    {
                        user [t] = 0x37;
                        write_symbol_user ();
                        t++;
                    }
                    if (value[i][j]=='8') 
                    {
                        user [t] = 0x38;
                        write_symbol_user ();
                        t++;
                    }
                    if (value[i][j]=='9') 
                    {
                        user [t] = 0x39;
                        write_symbol_user ();
                        t++;
                    }
                    if (value[i][j]=='*') 
                    {
                        if (q == 1)
                        {
                            phrase();
                            q = 0;
                            t = 0;
                            r = 1;
                        }
                    }
                    if (value[i][j]=='#') 
                    {
                        if (q == 1)
                        {
                            phrase();
                            q = 0;
                            t = 0;
                            r = 2;
                        }
                    }
                    if (value[i][j]=='A') 
                    {
                        start_fraza();
                        on_start();
                        delay(3000);
                        phrase();
                        keyboard();
                    }
                    if (value[i][j]=='D') 
                    {
                        user [t] = 0x20;
                        write_symbol_user ();
                        t++;
                    }
                    delay(314);
                    if(t>16){
                         pinMode(D8,OUTPUT);
                         analogWrite(D8,50);
                        delay(1000);
                         analogWrite(D8,0);
                        delay(1000);
                    }
                   
                }
            }  
            digitalWrite(PinOut[i], HIGH);
        }
}
void System::choice_system()
{
    text[0] = 0x20;
    text[1] = 0x4d;
    text[2] = 0x61;
    text[3] = 0x6b;
    text[4] = 0x65;
    text[5] = 0x20;
    text[6] = 0x61;
    text[7] = 0x20;
    text[8] = 0x63;
    text[9] = 0x68;
    text[10] = 0x6f;
    text[11] = 0x69;
    text[12] = 0x63;
    text[13] = 0x65;
    text[14] = 0x20;
    text[15] = 0x20;
    write_symbol_system ();
    for(int p = 0; p < 16; p++)
       {
           user [p] = 0x20;
       }
    write_symbol_user ();
    proverka_false();
}
void System::choice_user()
{
    text[0] = 0x2a;
    text[1] = 0x54;
    text[2] = 0x61;
    text[3] = 0x6b;
    text[4] = 0x65;
    text[5] = 0x20;
    text[6] = 0x74;
    text[7] = 0x68;
    text[8] = 0x65;
    text[9] = 0x20;
    text[10] = 0x64;
    text[11] = 0x65;
    text[12] = 0x76;
    text[13] = 0x69;
    text[14] = 0x63;
    text[15] = 0x65;
    write_symbol_system ();
    user[0] = 0x23;
    user[1] = 0x52;
    user[2] = 0x65;
    user[3] = 0x74;
    user[4] = 0x75;
    user[5] = 0x72;
    user[6] = 0x6e;
    user[7] = 0x20;
    user[8] = 0x64;
    user[9] = 0x65;
    user[10] = 0x76;
    user[11] = 0x69;
    user[12] = 0x63;
    user[13] = 0x65;
    write_symbol_user();
}
void System::choice()
{
    q = 1;
    k = 1;
    keyboard();
};
void System::proverka_true(){
     proverka = true;
};
void System::proverka_false(){
     proverka = false;
}
bool System::proverka_answer(){
    return proverka;
};