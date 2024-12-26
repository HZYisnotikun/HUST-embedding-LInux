#include <stdio.h>
#include <unistd.h>
#include "../common/common.h"

#define COLOR_BACKGROUND FB_COLOR(0xff, 0xff, 0xff)
#define COLOR_TEXT FB_COLOR(0x0, 0x0, 0x0)
#define RED FB_COLOR(255, 0, 0)
#define ORANGE FB_COLOR(255, 165, 0)
#define YELLOW FB_COLOR(255, 255, 0)
#define GREEN FB_COLOR(0, 255, 0)
#define CYAN FB_COLOR(0, 127, 255)
#define BLUE FB_COLOR(0, 0, 255)
#define PURPLE FB_COLOR(139, 0, 255)
#define BLACK FB_COLOR(0, 0, 0)
#define TIME_X (SCREEN_WIDTH - 160)
#define TIME_Y 0
#define TIME_W 100
#define TIME_H 30

#define SEND_X (SCREEN_WIDTH - 60)
#define SEND_Y 0
#define SEND_W 60
#define SEND_H 60
// #define easy_AI
int IP = 0;
int color[8] = {RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, PURPLE, BLACK};
static int touch_fd;
static int bluetooth_fd;
int flag[2] = {1, -1};				 // 面对方向
int over_flag = 0;					 // 结束符号
int pos[2][2] = {50, 250, 850, 250}; // 初始位置
int blood[2] = {350, 350};			 // 初始血量
fb_image *img_move_left[2], *img_move_right[2], *img_attack_left[2], *img_attack_right[2], *img_dead_left[2], *img_dead_right[2], *img_skill_left[2], *img_skill_right[2];
fb_image *img_background; // 定义全局变量存储背景图片
fb_image *menu[6];		  // 菜单
fb_image *gg, *lp, *rp;
void init_img()
{

	time_t t;
	/* 初始化随机数发生器 */
	srand((unsigned)time(&t));
	// 加载背景图片
	img_background = fb_read_png_image("./picture/bg1.png");

	img_move_left[0] = fb_read_png_image("./picture/move_0p_left.png");
	img_move_right[0] = fb_read_png_image("./picture/move_0p_right.png");
	img_move_left[1] = fb_read_png_image("./picture/move_1p_left.png");
	img_move_right[1] = fb_read_png_image("./picture/move_1p_right.png");

	img_attack_left[0] = fb_read_png_image("./picture/attack_0p_left.png");
	img_attack_right[0] = fb_read_png_image("./picture/attack_0p_right.png");
	img_attack_left[1] = fb_read_png_image("./picture/attack_1p_left.png");
	img_attack_right[1] = fb_read_png_image("./picture/attack_1p_right.png");

	img_skill_left[0] = fb_read_png_image("./picture/skill_0p_left.png");
	img_skill_right[0] = fb_read_png_image("./picture/skill_0p_right.png");
	img_skill_left[1] = fb_read_png_image("./picture/skill_1p_left.png");
	img_skill_right[1] = fb_read_png_image("./picture/skill_1p_right.png");

	img_dead_left[0] = fb_read_png_image("./picture/dead_0p_left.png");
	img_dead_right[0] = fb_read_png_image("./picture/dead_0p_right.png");
	img_dead_left[1] = fb_read_png_image("./picture/dead_1p_left.png");
	img_dead_right[1] = fb_read_png_image("./picture/dead_1p_right.png");

	gg = fb_read_png_image("./picture/gg1.png");

	menu[0] = fb_read_png_image("./picture/l.png");
	menu[1] = fb_read_png_image("./picture/ud.png");
	menu[2] = fb_read_png_image("./picture/r.png");
	menu[3] = fb_read_png_image("./picture/f.png");
	menu[4] = fb_read_png_image("./picture/s1.png");
	menu[5] = fb_read_png_image("./picture/s2.png");

	rp = fb_read_png_image("./picture/rp.png");
	lp = fb_read_png_image("./picture/lp.png");
	// img_move_left[0] = fb_read_png_image("./picture/3333.png");
	// img_move_right[0] = fb_read_png_image("./picture/3333.png");
	// img_move_left[1] = fb_read_png_image("./picture/3333.png");
	// img_move_right[1] = fb_read_png_image("./picture/3333.png");

	// img_attack_left[0] = fb_read_png_image("./picture/3333.png");
	// img_attack_right[0] = fb_read_png_image("./picture/3333.png");
	// img_attack_left[1] = fb_read_png_image("./picture/3333.png");
	// img_attack_right[1] = fb_read_png_image("./picture/3333.png");

	// img_dead_left[0] = fb_read_png_image("./picture/3333.png");
	// img_dead_right[0] = fb_read_png_image("./picture/3333.png");
	// img_dead_left[1] = fb_read_png_image("./picture/3333.png");
	// img_dead_right[1] = fb_read_png_image("./picture/3333.png");
	return;
}
void dead(int people, int fromflag)
{
	printf("game over!\n");
	if (fromflag == 0)
	{
		if (people == 0)
			myWrite_nonblock(bluetooth_fd, "7\n", 2);
		if (people == 1)
			myWrite_nonblock(bluetooth_fd, "8\n", 2);
	}

	fb_draw_rect(0, 50, SCREEN_WIDTH, SCREEN_HEIGHT - 150, COLOR_BACKGROUND);
	fb_draw_rect(20, 235, SCREEN_WIDTH - 40, 10, color[7]); // 绘制楼层

	printf("死亡动作%d\nflag[people]=%d\n\n", people, flag[people]);
	if (flag[people] == -1)
		fb_draw_image(pos[people][0], pos[people][1], img_dead_left[people], 0);
	else if (flag[people] == 1)
		fb_draw_image(pos[people][0], pos[people][1], img_dead_right[people], 0);

	if (flag[1 - people] == -1)
		fb_draw_image(pos[1 - people][0], pos[1 - people][1], img_move_left[1 - people], 0);
	else if (flag[1 - people] == 1)
		fb_draw_image(pos[1 - people][0], pos[1 - people][1], img_move_right[1 - people], 0);
	over_flag = 1;
	fb_update();

	sleep(3);
	fb_draw_image(0, 0, gg, 0);
	fb_update();

	exit(0);
}
void draw_blood(int change_0, int change_1, int fromflag)
{
	fb_draw_rect(0, 0, SCREEN_WIDTH, 50, COLOR_BACKGROUND);
	blood[0] = blood[0] + change_0;
	blood[1] = blood[1] + change_1;
	fb_draw_rect(10, 25, 355, 15, color[7]);
	fb_draw_rect(12, 27, blood[0], 10, color[0]);

	fb_draw_rect(650, 25, 355, 15, color[7]);
	fb_draw_rect(1000 - blood[1], 27, blood[1], 10, color[0]);
	fb_draw_image(370, 2, lp, 0);
	fb_draw_image(607, 2, rp, 0);
	fb_update();
	if (blood[0] <= 0)
		dead(0, fromflag);
	if (blood[1] <= 0)
		dead(1, fromflag);
	return;
}
void draw_move(int people, int direction)
{
	if (over_flag == 0)
	{
		flag[people] = direction;
		fb_draw_rect(0, 50, SCREEN_WIDTH, SCREEN_HEIGHT - 150, COLOR_BACKGROUND);
		fb_draw_rect(20, 235, SCREEN_WIDTH - 40, 10, color[7]); // 绘制楼层
		pos[people][0] += direction * 15;
		pos[people][0] = 0 < pos[people][0] ? pos[people][0] : 0;
		pos[people][0] = pos[people][0] < (SCREEN_WIDTH - 75) ? pos[people][0] : (SCREEN_WIDTH - 75);

		for (int i = 0; i <= 1; i++) // 两个人物
		{
			if (flag[i] == -1)
				fb_draw_image(pos[i][0], pos[i][1], img_move_left[i], 0);
			if (flag[i] == 1)
				fb_draw_image(pos[i][0], pos[i][1], img_move_right[i], 0);
		}
		fb_update();
	}
	return;
}
void up_down(int people)
{
	fb_draw_rect(0, 50, SCREEN_WIDTH, SCREEN_HEIGHT - 150, COLOR_BACKGROUND);
	fb_draw_rect(20, 235, SCREEN_WIDTH - 40, 10, color[7]); // 绘制楼层
	pos[people][1] = 300 - pos[people][1];
	for (int i = 0; i <= 1; i++) // 两个人物
	{
		if (flag[i] == -1)
			fb_draw_image(pos[i][0], pos[i][1], img_move_left[i], 0);
		if (flag[i] == 1)
			fb_draw_image(pos[i][0], pos[i][1], img_move_right[i], 0);
	}
	fb_update();
	return;
}
void skill_1(int people)
{

	fb_draw_rect(0, 50, SCREEN_WIDTH, SCREEN_HEIGHT - 150, COLOR_BACKGROUND);
	fb_draw_rect(20, 235, SCREEN_WIDTH - 40, 10, color[7]); // 绘制楼层
	pos[people][0] += flag[people] * 250;
	pos[people][0] = 0 < pos[people][0] ? pos[people][0] : 0;
	pos[people][0] = pos[people][0] < (SCREEN_WIDTH - 75) ? pos[people][0] : (SCREEN_WIDTH - 75);

	for (int i = 0; i <= 1; i++) // 两个人物
	{
		if (flag[i] == -1)
			fb_draw_image(pos[i][0], pos[i][1], img_move_left[i], 0);
		if (flag[i] == 1)
			fb_draw_image(pos[i][0], pos[i][1], img_move_right[i], 0);
	}
	fb_update();
	return;
}
void draw_attack(int attack_people)
{
	fb_draw_rect(0, 50, SCREEN_WIDTH, SCREEN_HEIGHT - 150, COLOR_BACKGROUND);
	fb_draw_rect(20, 235, SCREEN_WIDTH - 40, 10, color[7]); // 绘制楼层
	// printf("攻击动作%d\nflag[attack_people]=%d\n\n",attack_people,flag[attack_people]);
	if (flag[attack_people] == -1)
		fb_draw_image(pos[attack_people][0], pos[attack_people][1], img_attack_left[attack_people], 0);
	else if (flag[attack_people] == 1)
		fb_draw_image(pos[attack_people][0], pos[attack_people][1], img_attack_right[attack_people], 0);

	if (flag[1 - attack_people] == -1)
		fb_draw_image(pos[1 - attack_people][0], pos[1 - attack_people][1], img_move_left[1 - attack_people], 0);
	else if (flag[1 - attack_people] == 1)
		fb_draw_image(pos[1 - attack_people][0], pos[1 - attack_people][1], img_move_right[1 - attack_people], 0);
	fb_update();
	return;
}
void attack(int from_people)
{
	draw_attack(from_people);
	if (pos[0][1] == pos[1][1])
	{
		if (from_people == 0) // 0对1发动攻击
		{
			if ((pos[0][0] - pos[1][0]) * (pos[0][0] + flag[0] * 200 - pos[1][0]) < 0)
				draw_blood(0, -15, 0);
		}
		else // 1对0发动攻击
		{
			if ((pos[1][0] - pos[0][0]) * (pos[1][0] + flag[1] * 200 - pos[0][0]) < 0)
				draw_blood(-15, 0, 0);
		}
	}
	return;
}
void draw_skill(int skill_people)
{
	fb_draw_rect(0, 50, SCREEN_WIDTH, SCREEN_HEIGHT - 150, COLOR_BACKGROUND);
	fb_draw_rect(20, 235, SCREEN_WIDTH - 40, 10, color[7]); // 绘制楼层
	// printf("攻击动作%d\nflag[skill_people]=%d\n\n",skill_people,flag[skill_people]);
	if (flag[skill_people] == -1)
		fb_draw_image(pos[skill_people][0], pos[skill_people][1], img_skill_left[skill_people], 0);
	else if (flag[skill_people] == 1)
		fb_draw_image(pos[skill_people][0], pos[skill_people][1], img_skill_right[skill_people], 0);

	if (flag[1 - skill_people] == -1)
		fb_draw_image(pos[1 - skill_people][0], pos[1 - skill_people][1], img_move_left[1 - skill_people], 0);
	else if (flag[1 - skill_people] == 1)
		fb_draw_image(pos[1 - skill_people][0], pos[1 - skill_people][1], img_move_right[1 - skill_people], 0);
	fb_update();
	return;
}
void skill_2(int from_people)
{
	draw_skill(from_people);
	if (pos[0][1] == pos[1][1])
	{
		if (from_people == 0) // 0对1发动攻击
		{
			if ((pos[0][0] - pos[1][0]) * (pos[0][0] + flag[0] * 200 - pos[1][0]) < 0)
				draw_blood(0, -30, 0);
		}
		else // 1对0发动攻击
		{
			if ((pos[1][0] - pos[0][0]) * (pos[1][0] + flag[1] * 200 - pos[0][0]) < 0)
				draw_blood(-30, 0, 0);
		}
	}
	return;
}
static void touch_event_cb(int fd)
{
	int type, x, y, finger;
	type = touch_read(fd, &x, &y, &finger);
	switch (type)
	{
	case TOUCH_PRESS:
	case TOUCH_MOVE:
		if (over_flag == 0)
		{
			if (50 < x && x < 125 && 500 < y && y < 575)
			{
				myWrite_nonblock(bluetooth_fd, "1\n", 2);
				printf("向左移动\n");
				draw_move(IP, -1);
			}
			else if (150 < x && x < 225 && 500 < y && y < 575)
			{
				myWrite_nonblock(bluetooth_fd, "2\n", 2);
				printf("向右移动\n");
				up_down(IP);
			}
			else if (250 < x && x < 325 && 500 < y && y < 575)
			{
				myWrite_nonblock(bluetooth_fd, "3\n", 2);
				printf("向右移动\n");
				draw_move(IP, 1);
			}
			else if (700 < x && x < 775 && 500 < y && y < 575)
			{
				myWrite_nonblock(bluetooth_fd, "4\n", 2);
				printf("攻击动作\n");
				attack(IP);
			}
			else if (800 < x && x < 875 && 500 < y && y < 575)
			{
				myWrite_nonblock(bluetooth_fd, "5\n", 2);
				printf("技能\n");
				skill_1(IP);
			}
			else if (900 < x && x < 975 && 500 < y && y < 575)
			{
				myWrite_nonblock(bluetooth_fd, "6\n", 2);
				printf("技能\n");
				skill_2(IP);
			}
		}
		else
		{
			if (blood[0] <= 0)
				dead(0, 1);
			if (blood[1] <= 0)
				dead(1, 1);
		}
		// usleep(50000);
		if (over_flag == 0)
		{
#ifdef easy_AI
			int AI = 1 - IP;
			int a = rand() % (21);
			if (a < 14)
			{
				if (pos[IP][0] < pos[AI][0])
				{
					draw_move(AI, -1);
					if ((pos[AI][0] - pos[IP][0]) < 200)
					{
						attack(AI);
						if (a < 5)
							draw_move(AI, 1);
					}
				}
				if (pos[AI][0] < pos[IP][0])
				{
					draw_move(AI, 1);
					if ((pos[AI][0] - pos[IP][0]) < 200)
					{
						attack(AI);
						if (a < 5)
							draw_move(AI, 1);
					}
				}
			}
			else if (a < 18)
			{
				attack(AI);
			}
			else
			{
				if (pos[IP][0] < pos[AI][0])
				{
					draw_move(AI, 1);
				}
				if (pos[AI][0] < pos[IP][0])
				{
					draw_move(AI, -1);
				}
			}
			usleep(50000);
#endif
		}
		break;
	case TOUCH_RELEASE:
		break;
	case TOUCH_ERROR:
		printf("close touch fd\n");
		close(fd);
		task_delete_file(fd);
		break;
	default:
		return;
	}
	fb_update();
	return;
}
// static void touch_event_cb(int fd)
// {
// 	int type,x,y,finger;
// 	type = touch_read(fd, &x,&y,&finger);
// 	switch(type){
// 	case TOUCH_PRESS:
// 		//printf("type=%d,x=%d,y=%d,finger=%d\n",type,x,y,finger);
// 		if((x>=SEND_X)&&(x<SEND_X+SEND_W)&&(y>=SEND_Y)&&(y<SEND_Y+SEND_H)) {
// 			printf("bluetooth tty send hello\n");
// 			myWrite_nonblock(bluetooth_fd, "hello\n", 6);
// 		}
// 		break;
// 	case TOUCH_ERROR:
// 		printf("close touch fd\n");
// 		task_delete_file(fd);
// 		close(fd);
// 		break;
// 	default:
// 		return;
// 	}
// 	fb_update();
// 	return;
// }

static int pen_y = 30;
static void bluetooth_tty_event_cb(int fd)
{
	char buf[128];
	int n;

	n = myRead_nonblock(fd, buf, sizeof(buf) - 1);
	if (n <= 0)
	{
		printf("close bluetooth tty fd\n");
		// task_delete_file(fd);
		// close(fd);
		exit(0);
		return;
	}
	else
	{
		printf("%d %c\n", n, buf[0]);
		if (buf[0] == '1')
		{
			printf("向左移动\n");
			draw_move(1 - IP, -1);
		}
		else if (buf[0] == '2')
		{
			printf("向右移动\n");
			up_down(1 - IP);
		}
		else if (buf[0] == '3')
		{
			printf("向右移动\n");
			draw_move(1 - IP, 1);
		}
		else if (buf[0] == '4')
		{
			printf("攻击动作\n");
			attack(1 - IP);
		}
		else if (buf[0] == '5')
		{
			printf("技能\n");
			skill_1(1 - IP);
		}
		else if (buf[0] == '6')
		{
			printf("技能\n");
			skill_2(1 - IP);
		}
		else if (buf[0] == '7')
		{
			printf("1P死亡\n");
			draw_blood(-500, 0, 1);
			skill_2(1 - IP);
		}
		else if (buf[0] == '8')
		{
			printf("2P死亡\n");
			draw_blood(0, -500, 1);
			skill_2(1 - IP);
		}
	}

	// buf[n] = '\0';
	// printf("bluetooth tty receive \"%s\"\n", buf);
	// fb_draw_text(2, pen_y, buf, 24, COLOR_TEXT); fb_update();
	// pen_y += 30;
	return;
}

static int bluetooth_tty_init(const char *dev)
{
	int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK); /*非阻塞模式*/
	if (fd < 0)
	{
		printf("bluetooth_tty_init open %s error(%d): %s\n", dev, errno, strerror(errno));
		return -1;
	}
	return fd;
}

// static int st=0;
// static void timer_cb(int period) /*该函数0.5秒执行一次*/
// {
// 	char buf[100];
// 	sprintf(buf, "%d", st++);
// 	fb_draw_rect(TIME_X, TIME_Y, TIME_W, TIME_H, COLOR_BACKGROUND);
// 	fb_draw_border(TIME_X, TIME_Y, TIME_W, TIME_H, COLOR_TEXT);
// 	fb_draw_text(TIME_X+2, TIME_Y+20, buf, 24, COLOR_TEXT);
// 	fb_update();
// 	return;
// }
int main(int argc, char *argv[])
{
	init_img();
	fb_init("/dev/fb0");
	font_init("./font.ttc");
	fb_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BACKGROUND);
	// fb_draw_rect(50, 500, 75, 75, color[1]);				// 画一个向左移动按钮
	fb_draw_image(50, 500, menu[0], 0);
	// fb_draw_rect(150, 500, 75, 75, color[1]);				// 画一个上下楼按钮
	fb_draw_image(150, 500, menu[1], 0);
	// fb_draw_rect(250, 500, 75, 75, color[1]);				// 画一个向右移动按钮
	fb_draw_image(250, 500, menu[2], 0);
	// fb_draw_rect(700, 500, 75, 75, color[1]);				// 画一个攻击按钮
	fb_draw_image(700, 500, menu[3], 0);
	// fb_draw_rect(800, 500, 75, 75, color[1]);				// 画一个技能按钮
	fb_draw_image(800, 500, menu[4], 0);
	// fb_draw_rect(900, 500, 75, 75, color[1]);				// 画一个技能按钮
	fb_draw_image(900, 500, menu[5], 0);
	draw_move(0, flag[0]);									// 绘制初始人物
	draw_blood(0, 0, 0);									// 画血条
	fb_draw_rect(20, 235, SCREEN_WIDTH - 40, 10, color[7]); // 绘制楼层

	fb_draw_image(370,2,lp,0);
	fb_draw_image(607,2,rp,0);
	fb_update();

	// 打开多点触摸设备文件, 返回文件fd
	touch_fd = touch_init("/dev/input/event2");
	// 添加任务, 当touch_fd文件可读时, 会自动调用touch_event_cb函数
	task_add_file(touch_fd, touch_event_cb);

	bluetooth_fd = bluetooth_tty_init("/dev/rfcomm0");
	if (bluetooth_fd == -1)
		return 0;
	task_add_file(bluetooth_fd, bluetooth_tty_event_cb);

	// task_add_timer(500, timer_cb); /*增加0.5秒的定时器*/
	task_loop(); // 进入任务循环
	return 0;
}