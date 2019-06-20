#include "main.h"
#include "../lcd.h"
#include "Arduino_w.h"


int main(int argc, char *argv[])
{
    
    System lcd;
    lcd.mt1(0x00,0x00);
    lcd.start_fraza();
    GPIO_ChangeDirection_Output(MT_E);	//установка режима работы выводов
    GPIO_ChangeDirection_Output(MT_A0); 
    GPIO_ChangeDirection_Output(MT_DB4);
    GPIO_ChangeDirection_Output(MT_DB5);
    GPIO_ChangeDirection_Output(MT_DB6);
    GPIO_ChangeDirection_Output(MT_DB7);
    GPIO_DigitalWrite(MT_E, 0);	//сброс выходов
    GPIO_DigitalWrite(MT_A0, 0);
    GPIO_DigitalWrite(MT_DB4, 0);
    GPIO_DigitalWrite(MT_DB5, 0);
    GPIO_DigitalWrite(MT_DB6, 0);
    GPIO_DigitalWrite(MT_DB7, 0);
    delay (20); //ожидание 20 мс по даташиту
    GPIO_DigitalWrite(MT_DB4, 1); //инициализация ЖК индикатора
    GPIO_DigitalWrite(MT_DB5, 1);
    GPIO_DigitalWrite(MT_E, 1);
    GPIO_DigitalWrite(MT_E, 0);
    delayMicroseconds (40); //ожидание 40 мкс по даташиту
    GPIO_DigitalWrite(MT_E, 1);
    GPIO_DigitalWrite(MT_E, 0);
    delayMicroseconds (40); //ожидание 40 мкс по даташиту
    GPIO_DigitalWrite(MT_E, 1);
    GPIO_DigitalWrite(MT_E, 0);
    delayMicroseconds (40); //ожидание 40 мкс по даташиту
    GPIO_DigitalWrite(MT_DB4, 0); //установка четырёхбитного режима
    GPIO_DigitalWrite(MT_E, 1);
    GPIO_DigitalWrite(MT_E, 0);
    delayMicroseconds (40); //ожидание 40 мкс по даташиту
    lcd.mt16_k (0x2A); //установка разрядности
    lcd.mt16_k (0x08); //выключение ЖК индикатора
    lcd.mt16_k (0x01); //очистка ЖК индикатора
    delay (2); //ожидание 1,5 мс для очистки ЖК индикатора
    lcd.mt16_k (0x06); //установка направления сдвига
    //mt16_k (0x2A); //установка 1-й страницы знакогенератора
    lcd.mt16_k (0x0C);
    
  pinMode (E2, OUTPUT);
  pinMode (E3, OUTPUT);
  pinMode (E4, OUTPUT);
  pinMode (E5, OUTPUT);
  
  pinMode (E6, INPUT_PULLUP); // инициализируем порты на вход с подтяжкой к плюсу (принимают нули на строках)
  pinMode (E7, INPUT_PULLUP);
  pinMode (E8, INPUT_PULLUP);
  pinMode (E9, INPUT_PULLUP);
  bool sleep = false;
  while(1)
    {
      //beg();
      lcd.on_start();
      delay(3000);
      lcd.phrase();     
            
      while(1)
      {
          while(sleep)
        {
          for(int i = 0; i < 3; i++)
          {     
              digitalWrite(lcd.PinOut[i], LOW);
              for (int j = 0; j < 4; j++)
              {
               if (digitalRead(lcd.PinIn[j]) == LOW)
                  {
                      sleep= false;
                      lcd.on_start();
                  }   
              }
              digitalWrite(lcd.PinOut[i], HIGH);
            }

        }
        lcd.keyboard(); 
        while (lcd.proverka_answer())
        {
            lcd.choice_system();//пошел выбор
            delay(3000);
            lcd.choice_user();
            lcd.choice();
        }
      }
  }
    return EXIT_SUCCESS;
}
