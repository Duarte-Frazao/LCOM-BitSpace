#include <minix/syslib.h>
#include "Score.h"

static Sprite **numbers;
static Score *score;


Sprite** loadNumbers(){
	numbers = (Sprite **) malloc(sizeof(Sprite *) * 12);
	char filename[] = "/home/lcom/svn/BitSpace/res/images/numbers/numeralX.bmp";

	unsigned int i;
	for (i = 0; i < 10; i++) {
		//Altera o valor X para a imagem correspondente
		filename[strlen(filename) - 5] = (char) (i + '0');
		numbers[i] = create_sprite_Bitmap(filename, 0, 0, 0, 0);
	}

	numbers[10] = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/numbers/clock.bmp", 0, 0, 0, 0);
	numbers[11] = create_sprite_Bitmap("/home/lcom/svn/BitSpace/res/images/numbers/bar.bmp", 0, 0, 0, 0);
	return numbers;
}

Sprite** getNumbers(){
	return numbers;
}


Score * initialize_score(){

	score = (Score *)malloc(sizeof(Score));
	score->actual_score = 0;
	score->numeral = numbers;
}

void update_score(){
	score->actual_score ++;
}

void reset_score(){
	score->actual_score = 0;
}

void draw_num(int num, unsigned int *posX, unsigned int *posY, unsigned int size){
	unsigned int i;
	int delta = score->numeral[0]->width + 3;

	for(i = 0; i < size; i++){
		int dig = num % 10;
		num/=10;
		score->numeral[dig]->x = *posX;
		score->numeral[dig]->y = *posY;

		draw_sprite(score->numeral[dig]);

		*posX -= delta;
	}
}

void draw_score(){

	int delta = score->numeral[0]->width + 3;
	int xn = 1024 - (2 * delta);
	int yn = 768 - (2*delta);
	draw_num(score->actual_score, &xn, &yn, 6);
}

void draw_date(char * date, unsigned int posX, unsigned int posY){
	int hrs, min, sec, day, mth, yrs;
	sscanf(date, "%d %d %d %d %d %d", &hrs, &min, &sec, &day, &mth, &yrs);

	int delta = (score->numeral[0]->width + 3)/2;
	int delta_2 = (score->numeral[0]->width + 3)/4;

	draw_num(yrs,&posX,&posY,2);

	score->numeral[11]->x = posX+ delta_2;
	score->numeral[11]->y = posY;
	draw_sprite(score->numeral[11]); posX -= delta;

	draw_num(mth,&posX,&posY,2);

	score->numeral[11]->x = posX + delta_2;
	score->numeral[11]->y = posY;
	draw_sprite(score->numeral[11]); posX -= delta;

	draw_num(day,&posX,&posY,2);

	posX -= 2*delta;

	draw_num(sec,&posX,&posY,2);

	score->numeral[10]->x = posX + delta_2;
	score->numeral[10]->y = posY;
	draw_sprite(score->numeral[10]); posX -= delta;

	draw_num(min,&posX,&posY,2);

	score->numeral[10]->x = posX + delta_2;
	score->numeral[10]->y = posY;
	draw_sprite(score->numeral[10]); posX -= delta;

	draw_num(hrs,&posX,&posY,2);
}


void delete_score(){
	if(numbers != NULL){
		unsigned int i;
		for(i = 0; i < 11; i++)
			destroy_sprite(numbers[i]);
		free(numbers);
	}
	if(score != NULL) free(score);
}
