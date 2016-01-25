/******************************************************************************

                  ��Ȩ���� (C), 2009-2015, ����С�ֵ�                          

 ******************************************************************************
  �� �� ��   : main.c
  �� �� ��   : ����
  ��    ��   : ����С�ֵ�
  ��������   : 2015��9��21��,����һ
  ����޸�   :
  ��������   : ����̰������Ϸ������
  �����б�   :
              game_snake
              main
              reset_keypress
              set_keypress
  �޸���ʷ   :
  1.��    ��   : 2015��9��21��,����һ
    ��    ��   : ����С�ֵ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include "lcd128x64.h"
#include "lcd_snake.h"

static struct termios   OLD_SETTING;
static SNAKE_DIR        KEY_DIR = DR_RIGHT;

//���ÿ���̨ģʽ��ֱ�Ӷ����ַ�������Ҫ�س�
void set_keypress (void)
{
    struct termios new_settings;
    tcgetattr (0, &OLD_SETTING);
    new_settings = OLD_SETTING;
    /* Disable canonical mode, and set buffer size to 1 byte */
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr (0, TCSANOW, &new_settings);
    return;
}
    
void reset_keypress(void)
{
    tcsetattr (0, TCSANOW, &OLD_SETTING);
    return;
}

//̰�����߳�
void* game_snake(void)
{
    while(1)
    {
        snake_move_control(KEY_DIR);
        lcd128x64update();
        usleep(1000*snake_get_speed());
    }
}

int main(void)
{
    pthread_t snake_id;
    int32 ret = 0, ch = 0;
    
    lcd128x64setup();
    
    system("clear");//�������̨��ʾ
    printf("Welcome to Snake Game!---by whjwnavy@163.com\r\n");
    printf("Press \"WSAD\" or \"wsad\" to move Snake!\r\n");
    printf("Press \'1\' to add Speed and \'2\' to sub Speed!\r\n");
    printf("Press \'Q\' or \'q\' or \'ESC\' to Exit Game!\r\n\r\n");
    lcd128x64puts(0, 0, "Welcome to Snake Game!"
                        "\r\nby whjwnavy@163.com", 0, 1);
    lcd128x64update();//������ʾ
    sleep(3);//�ȴ�1��
    lcd128x64clear(0);//�����Ļ��ʾ
    //system("clear");//�������̨��ʾ
    
    if(snake_game_init(3, DR_RIGHT, 10, 100) == RTN_ERR)//̰���߳�ʼ��
        return 0;
    lcd128x64update();//������ʾ
    
    //����̰�����߳�
    ret = pthread_create(&snake_id, NULL, (void*)game_snake, NULL);
    if(ret)
    {
        printf("Create pthread error!\n");
        return 1;
    }
    /*
    �����(��)�� VK_UP (38)
    �����(��)�� VK_DOWN (40)
    �����(��)�� VK_LEFT (37)
    �����(��)�� VK_RIGHT (39)
    ESC�� VK_ESCAPE (27)
    */    
    set_keypress();
    while(ch != 'q')
    {
        ch = getchar();
        switch(ch)
        {
            case 'w':
            case 'W':
            //case 38:
            {
                KEY_DIR = DR_UP;
                break;
            }
            case 's':
            case 'S':
            //case 40:
            {
                KEY_DIR = DR_DOWN;
                break;
            }
            case 'a':
            case 'A':
            //case 37:
            {
                KEY_DIR = DR_LEFT;
                break;
            }
            case 'd':
            case 'D':
            //case 39:
            {
                KEY_DIR = DR_RIGHT;
                break;
            }
            case '1':
            {
                snake_add_speed();
                break;
            }
            case '2':
            {
                snake_sub_speed();
                break;
            }
            case 'q':
            case 'Q':
            //case 27:
            {
                ch = 'q';
                break;
            }
            default:
                break;
        }
        printf("\rSpeed=%3d | Life=%3d | Score=%3d | Dir=%3d | "
                "INPUT KEY=%3c", snake_get_speed(), snake_get_life(), \
            snake_get_score(), snake_get_dir(), ch);
    }
    reset_keypress();
    return 0;
}
