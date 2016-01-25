/******************************************************************************

                  ��Ȩ���� (C), 2009-2015, ����С�ֵ�                          

 ******************************************************************************
  �� �� ��   : lcd_snake.c
  �� �� ��   : rev01_20150921
  ��    ��   : ����С�ֵ�
  ��������   : 2015��9��21��,����һ
  ����޸�   :
  ��������   : ����̰������Ϸ
  �ӿں���   :
              snake_game_init         (int32 scnt, SNAKE_DIR sdir, \
                                       int32 sstep, int32 speed);
              snake_move_control      (SNAKE_DIR KEY);
              snake_get_score         (void);
              snake_get_speed         (void);
              snake_set_speed         (int32 speed);
              snake_add_speed         (void);
              snake_sub_speed         (void);
              snake_get_life          (void);
              snake_set_scstep        (int32 scstep);
              snake_get_dir           (void);
              snake_set_dir           (SNAKE_DIR sdir);
              snake_set_crosswall     (int32 crosswall);
  �޸���ʷ   :
  1.��    ��   : 2015��9��22��,���ڶ�
    ��    ��   : ����С�ֵ�
    �޸�����   : ȡ��ȫ�ֱ�������Ϊ�����ṩ�ӿں������鿴�����ñ���ֵ

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "lcd128x64.h"
#include "lcd_snake.h"

/*----------------------------------------------*
 * �Զ�����������                               *
 *----------------------------------------------*/
typedef enum snake_propy//����
{
    PR_FOOD = 0,        //0ʳ��
    PR_HEAD,            //1��ͷ
    PR_BODY,            //2����
    PR_WALL,            //3ǽ��
    PR_NULL             //�հ�
} SNAKE_PROPY;

typedef enum snake_col//��ɫ
{
    COL_BLACK = 0,      //0��ɫ��ʾ����ʾ
    COL_WHITE,          //1��ɫ��ʾ��ʾ
} SNAKE_COL;

typedef struct snake_point//λ��
{
    int32               PT_LOCX;//X����
    int32               PT_LOCY;//Y����
    SNAKE_COL           PT_COLOR;
} SNAKE_POINT;

typedef struct snake//��
{
    SNAKE_POINT         S_POINT;//����
    SNAKE_PROPY         S_PROPERTY;//����
} SNAKE;

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define SnakePointX     SnakePtSize//������С����λ:���أ������Ϊ������
#define SnakePointY     SnakePointX//������С
#define SnakeStepX      (SnakeMaxX / SnakePointX)//�ƶ�����
#define SnakeStepY      (SnakeMaxY / SnakePointY)//�ƶ�����

/*----------------------------------------------*
 * �ڲ���̬����                                 *
 *----------------------------------------------*/
static SNAKE        SNAKE_FOOD;//ʳ��
static SNAKE        GAME_SNAKE[SnakeMaxLen];//��
//static SNAKE        GAME_MAP[SnakeMaxLen];//��ͼ
static int32        SnakeCount = 3;//������
static SNAKE_DIR    SnakeDir = DR_RIGHT;//��ͷ�ƶ�����
static SNAKE_LIFE   SnakeLife = LF_LIVE;//��Ϸ����
static int32        SnakeScore = 0;//�÷�
static int32        SnakeSpeed = 10;//�ٶ�
static int32        SCORE_STEP = 10;//ÿ�Ե�10��ʳ���ٶȼӿ�һ��
static int32        SCROSS_WALL = 1;//����Խ��

/*****************************************************************************
 �� �� ��  : snake_draw_point
 ��������  : ʹ�����Ź����DPI����һ�������
 �������  : SNAKE_POINT* s_point  
 �������  : ��
 �� �� ֵ  : �ɹ�����0��ʧ�ܷ���1
 ����˵��  : 
*****************************************************************************/
int32 snake_draw_point(SNAKE_POINT* s_point)
{
    int32 x = 0, y = 0, col = 0;
    
    if(s_point == RTN_NULL)
        return RTN_ERR;
    
    x = s_point->PT_LOCX;
    y = s_point->PT_LOCY;
    col = s_point->PT_COLOR;
    
    if((x < 0) || (x >= SnakeStepX) || (y < 0) || (y >= SnakeStepY))
        return RTN_ERR;
    
    x = x * SnakePointX;
    y = y * SnakePointY;
    
    if(SnakePointX != 1)//��һ�����δ������Ź���ĵ�
        lcd128x64rectangle(x, y, x+SnakePointX-1, y+SnakePointY-1, col, 1);
    else//Ϊ�˼ӿ��ٶȣ����Ϊ1�����ţ�ֱ�ӻ���
        lcd128x64point(x, y, col);
    return RTN_OK;
}

/*****************************************************************************
 �� �� ��  : snake_get_point
 ��������  : ��ȡ��Ļ��ĳ�������ɫ
 �������  : SNAKE_POINT* s_point  
 �������  : ��
 �� �� ֵ  : ����ָ���ĵ����ɫ(1��0)
 ����˵��  : 
*****************************************************************************/
int32 snake_get_point(SNAKE_POINT* s_point)
{
    int32 x = 0, y = 0;
    
    if(s_point == RTN_NULL)
        return RTN_ERR;
    
    x = s_point->PT_LOCX;
    y = s_point->PT_LOCY;
    
    if((x < 0) || (x >= SnakeStepX) || (y < 0) || (y >= SnakeStepY))
        return RTN_ERR;
    
    x = x * SnakePointX;
    y = y * SnakePointY;
    
    /*if(lcd128x64getpoint(x, y))
        s_point->PT_COLOR = COL_WHITE;
    else
        s_point->PT_COLOR = COL_BLACK;
    
    return s_point->PT_COLOR;*/
    
    return(lcd128x64getpoint(x, y));
}

/*****************************************************************************
 �� �� ��  : snake_get_randxy
 ��������  : ����һ������������
 �������  : SNAKE_POINT* s_point  
 �������  : ��
 �� �� ֵ  : �ɹ����ظĵ㣬ʧ�ܷ���NULL
 ����˵��  : 
*****************************************************************************/
void* snake_get_randxy(SNAKE_POINT* s_point)
{
    struct timeval tpstart;
    uint32 sseed = 0;
    
    if(s_point == RTN_NULL)
        return RTN_NULL;
    
    gettimeofday(&tpstart,RTN_NULL);
    sseed = (uint32)tpstart.tv_usec;
    
    srand(sseed);
    s_point->PT_LOCX = rand() % SnakeStepX;
    s_point->PT_LOCY = rand() % SnakeStepY;
    
    return s_point;
}

/*****************************************************************************
 �� �� ��  : snake_create_food
 ��������  : ����һ���㣬���λ��������ģ����Ҹ�λ��δ��ռ��
 �������  : SNAKE* food:Ҫ���ɵĵ�
             SNAKE_PROPY spropy:ָ��Ҫ�����ĵ������(ʳ��?��ͷ?����?) 
 �������  : ��
 �� �� ֵ  : 
 ����˵��  : 
*****************************************************************************/
void* snake_create_food(SNAKE* food, SNAKE_PROPY spropy)
{
    SNAKE_POINT foodxy;
    
    int32 n = 1;
    
    if(food == RTN_NULL)
        return RTN_NULL;
    
    while(n++)//Ѱ�ҿ��Է���ʳ��������
    {
        snake_get_randxy(&foodxy);//����һ�������
        if(!snake_get_point(&foodxy))
        {
            break;//ʳ�ﲻ�����������Χǽ��
        }
        if(n > SnakeStepX*SnakeStepY)
        {
            n = 0;
            return RTN_NULL;//���������еĵ�֮�����Ȼ���ɹ����˳�
        }
    }
    
    foodxy.PT_COLOR = COL_WHITE;
    food->S_POINT.PT_LOCX = foodxy.PT_LOCX;
    food->S_POINT.PT_LOCY = foodxy.PT_LOCY;
    food->S_POINT.PT_COLOR = foodxy.PT_COLOR;
    food->S_PROPERTY = spropy;
    
    snake_draw_point(&foodxy);
 
    return food;
}

/*****************************************************************************
 �� �� ��  : snake_game_init
 ��������  : ̰������Ϸ��ʼ��
 �������  : int32 scnt:��ʼ��������
             SNAKE_DIR sdir:��ʼ����ͷ����
             int32 sstep:�÷��ٶ�
             int32 speed:�ٶ�
 �������  : ��
 �� �� ֵ  : 
 ����˵��  : 
*****************************************************************************/
int32 snake_game_init(int32 scnt, SNAKE_DIR sdir, \
                        int32 sstep, int32 speed)
{
    int32 dx = 0, dy = 0, i = 0;
    SNAKE_POINT snake_head;
    
    if((scnt <= 0) || (scnt >= SnakeMaxLen) || (sstep < 0) || (speed < 0))
        return RTN_ERR;
    
    SnakeLife = LF_LIVE;//��Ϸ����
    SnakeCount = scnt;//������
    SnakeDir = sdir;//��ͷ�ƶ�����
    SCORE_STEP = sstep;//������λ
    SnakeSpeed = speed;//�ٶȻ�ȼ�
    
    memset(GAME_SNAKE, 0, sizeof(GAME_SNAKE));
    
    //�������һ����ͷ
    if(snake_create_food(&(GAME_SNAKE[0]), PR_HEAD) == RTN_NULL)
        return RTN_ERR;//������ͷʧ��
    
    switch(sdir)
    {
        case DR_UP://��
            {dx = 0; dy = 1; break;}
        case DR_DOWN://��
            {dx = 0; dy = -1; break;}
        case DR_LEFT://��
            {dx = 1; dy = 0; break;}
        default://��
            {dx = -1; dy = 0; break;}
    }
    
    //��������
    for(i=1; i< scnt; i++)
    {
        GAME_SNAKE[i].S_POINT.PT_LOCX = \
            GAME_SNAKE[i-1].S_POINT.PT_LOCX + dx;
        GAME_SNAKE[i].S_POINT.PT_LOCY = \
            GAME_SNAKE[i-1].S_POINT.PT_LOCY + dy;
        GAME_SNAKE[i].S_POINT.PT_COLOR = COL_WHITE;
        GAME_SNAKE[i].S_PROPERTY = PR_BODY;
        snake_draw_point(&(GAME_SNAKE[i].S_POINT));
    }

    //����һ��ʳ��
    if(snake_create_food(&SNAKE_FOOD, PR_FOOD) == RTN_NULL)
        return RTN_ERR;

    return RTN_OK;
}

/*****************************************************************************
 �� �� ��  : snake_move_step
 ��������  : ��������ǰ�ƶ�һ�����������ƶ���������Ҫ������¼�
 �������  : SNAKE_DIR Dir  
 �������  : ��
 �� �� ֵ  : ���ص�ǰ������״̬
 ����˵��  : 
*****************************************************************************/
SNAKE_LIFE snake_move_step(SNAKE_DIR Dir)
{
    int32 dx = 0,dy = 0;
    uint32 n = 0, i = 0;
    SNAKE_POINT s_tail;
    
    //�ж��ƶ�����
    if(Dir == DR_UP)//��
        {dx = 0; dy = -1;}
    else if(Dir == DR_DOWN)//��
        {dx = 0; dy = 1;}
    else if(Dir == DR_LEFT)//��
        {dx = -1; dy = 0;}
    else//��
        {dx = 1; dy = 0;}
    
    //������������
    for(n=SnakeCount-1; n>0; n--)
    {
        memcpy(&(GAME_SNAKE[n].S_POINT), &(GAME_SNAKE[n-1].S_POINT), \
            sizeof(SNAKE_POINT));
    }
    //������ͷ����
    GAME_SNAKE[0].S_POINT.PT_LOCX += dx;
    GAME_SNAKE[0].S_POINT.PT_LOCY += dy;
    
    //�ж��ߵ�״̬:�Ե�ʳ��?ҧ���Լ�?ײ��ǽ?
    if(snake_get_point(&(GAME_SNAKE[0].S_POINT)))
    {
        //if(memcmp(&(GAME_SNAKE[n].S_POINT), &(SNAKE_FOOD.S_POINT), \
        //        sizeof(SNAKE_POINT) == 0)//���ڴ�ȽϷ�ʽ�ж������Ƿ����
        if((GAME_SNAKE[0].S_POINT.PT_LOCX == SNAKE_FOOD.S_POINT.PT_LOCX) || \
           (GAME_SNAKE[0].S_POINT.PT_LOCY == SNAKE_FOOD.S_POINT.PT_LOCY))
        {//�Ե�ʳ��
            if(snake_create_food(&SNAKE_FOOD, PR_FOOD) != RTN_NULL)
            {//����ʳ��ɹ�
                SnakeCount++;//����������
                if(SnakeCount >= SnakeMaxLen)
                    SnakeLife = LF_WIN;//�ﵽ��󳤶���ͨ��
                SnakeScore++;//��������
                if(SnakeScore % SCORE_STEP == 0)
                {
                    if(SnakeSpeed > SCORE_STEP)
                    SnakeSpeed -= SCORE_STEP;//�ٶȡ��ȼ�����
                }
            }
            else//����ʳ�ﲻ�ɹ�,˵����ʱ��Ļ�ϼ���û�п��ÿռ���
            {
                SnakeLife = LF_WIN;//��������϶�Ϊͨ��
            }
        }
        else
        {
            SnakeLife = LF_DIE;//ҧ���Լ���ײ��ǽ������
        }
    }

    //Խ�紦��
    if(SCROSS_WALL)
    {//����Խ��
        if(GAME_SNAKE[0].S_POINT.PT_LOCX >= SnakeStepX) 
            GAME_SNAKE[0].S_POINT.PT_LOCX = 0;
        if(GAME_SNAKE[0].S_POINT.PT_LOCX < 0) 
            GAME_SNAKE[0].S_POINT.PT_LOCX = SnakeStepX - 1;
        if(GAME_SNAKE[0].S_POINT.PT_LOCY >= SnakeStepY) 
            GAME_SNAKE[0].S_POINT.PT_LOCY = 0;
        if(GAME_SNAKE[0].S_POINT.PT_LOCY < 0) 
            GAME_SNAKE[0].S_POINT.PT_LOCY = SnakeStepY - 1;
    }
    else
    {//������Խ��
        if((GAME_SNAKE[0].S_POINT.PT_LOCX >= SnakeStepX) || \
            (GAME_SNAKE[0].S_POINT.PT_LOCX < 0) || \
            (GAME_SNAKE[0].S_POINT.PT_LOCY >= SnakeStepY) || \
            (GAME_SNAKE[0].S_POINT.PT_LOCY < 0))
        {
            if(SnakeLife == LF_LIVE)
                SnakeLife = LF_DIE;//�����߽�������
        }
    }
    #endif
    
    //������ʾ��    
    for(i=0; i< SnakeCount; i++)
        {snake_draw_point(&(GAME_SNAKE[i].S_POINT));}
    
    //������β�ĵ�
    memcpy(&s_tail, &(GAME_SNAKE[SnakeCount-1].S_POINT), \
            sizeof(SNAKE_POINT));//��ȡ��β�������
    s_tail.PT_COLOR = COL_BLACK;
    snake_draw_point(&s_tail);//������β�ĵ�
    
    return SnakeLife;
}

/*****************************************************************************
 �� �� ��  : snake_move_control
 ��������  : ���ݰ���ֵ�������ƶ�
 �������  : SNAKE_DIR KEY
 �������  : ��
 �� �� ֵ  : ������ҵ�ǰ������״̬
 ����˵��  : û�е�������Ϸ״̬��������ʹ���߱������Լ����������и���
             �ú������ص�����״̬��������Ϸ����(����˵��SnakeLifeΪLF_DIE��
             ʱ�����ʲô�£���Щ��Ҫ��ʹ�ñ����������Լ����)
*****************************************************************************/
SNAKE_LIFE snake_move_control(SNAKE_DIR KEY)
{
    //ֻ�а����������Ҽ����з�Ӧ
    if(((KEY == DR_UP)&&(SnakeDir != DR_DOWN)) || \
       ((KEY == DR_DOWN)&&(SnakeDir != DR_UP)) || \
       ((KEY == DR_LEFT)&&(SnakeDir != DR_RIGHT)) || \
       ((KEY == DR_RIGHT)&&(SnakeDir != DR_LEFT)))//���ܷ����ƶ�
    {
        SnakeDir = KEY;//�����ƶ�����
    }
    return(snake_move_step(SnakeDir));//���հ��������ƶ�
}

/*****************************************************************************
 �� �� ��  : snake_get_score
 ��������  : ��ȡ��ǰ��Ϸ�÷�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��Ϸ�÷�
 ����˵��  : 
*****************************************************************************/
int32 snake_get_score(void)
{
    return SnakeScore;
}

/*****************************************************************************
 �� �� ��  : snake_get_speed
 ��������  : ��ȡ��ǰ�ٶ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : �ٶ�
 ����˵��  : 
*****************************************************************************/
int32 snake_get_speed(void)
{
    return SnakeSpeed;
}

/*****************************************************************************
 �� �� ��  : snake_set_speed
 ��������  : ���õ�ǰ�ٶ�
 �������  : int32 speed
 �������  : ��
 �� �� ֵ  : �ٶ�
 ����˵��  : 
*****************************************************************************/
int32 snake_set_speed(int32 speed)
{
    if(speed > 0)
        SnakeSpeed = speed;
    
    return SnakeSpeed;
}

/*****************************************************************************
 �� �� ��  : snake_sub_speed
 ��������  : ���ٶȼ�С
 �������  : ��
 �������  : ��
 �� �� ֵ  : �ٶ�
 ����˵��  : 
*****************************************************************************/
int32 snake_sub_speed(void)
{
    if(SnakeSpeed < 500)
        SnakeSpeed += SCORE_STEP;
    return SnakeSpeed;
}

/*****************************************************************************
 �� �� ��  : snake_add_speed
 ��������  : ���ٶ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  : �ٶ�
 ����˵��  : 
*****************************************************************************/
int32 snake_add_speed(void)
{
    if(SnakeSpeed > SCORE_STEP)
        SnakeSpeed -= SCORE_STEP;
    return SnakeSpeed;
}

/*****************************************************************************
 �� �� ��  : snake_get_life
 ��������  : ��ȡ��ǰ��Ϸ����ֵ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��ǰ��Ϸ����ֵ
 ����˵��  : 
*****************************************************************************/
SNAKE_LIFE snake_get_life(void)
{
    return SnakeLife;
}

/*****************************************************************************
 �� �� ��  : snake_set_scstep
 ��������  : ������Ϸ�÷ֲ���
 �������  : int32 scstep
 �������  : ��
 �� �� ֵ  : ��Ϸ�÷ֲ���
 ����˵��  : 
*****************************************************************************/
int32 snake_set_scstep(int32 scstep)
{
    if(scstep > 0)
        SCORE_STEP = scstep;

    return SCORE_STEP;
}

/*****************************************************************************
 �� �� ��  : snake_get_dir
 ��������  : ��ȡ��ǰ��ͷ����
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��ǰ��ͷ����
 ����˵��  : 
*****************************************************************************/
SNAKE_DIR snake_get_dir(void)
{
    return SnakeDir;
}

/*****************************************************************************
 �� �� ��  : snake_set_dir
 ��������  : ���õ�ǰ��ͷ����
 �������  : SNAKE_DIR sdir
 �������  : ��
 �� �� ֵ  : ��ǰ��ͷ����
 ����˵��  : 
*****************************************************************************/
SNAKE_DIR snake_set_dir(SNAKE_DIR sdir)
{
    if((sdir == DR_UP) || (sdir == DR_DOWN) || \
        (sdir == DR_LEFT) ||(sdir == DR_RIGHT))
        SnakeDir = sdir;
    return SnakeDir;
}

/*****************************************************************************
 �� �� ��  : snake_set_crosswall
 ��������  : �����Ƿ�����Խ��
 �������  : int32 crosswall
 �������  : ��
 �� �� ֵ  : ��
 ����˵��  : �����ʾ����Խ��
*****************************************************************************/
void snake_set_crosswall(int32 crosswall)
{
    SCROSS_WALL = crosswall;
}