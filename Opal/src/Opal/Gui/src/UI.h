#pragma once

#define MAX_COMMANDS 1024

typedef unsigned id;

typedef struct vec2 { int x,y; };
typedef struct Rect { int x, y, w, h; };

typedef struct 
{
	int type;
	Rect rect;
	unsigned int color;
}command;
typedef struct 
{
	vec2 mousepos;
	int mousepress;
	int mousedown;

	int hover_id;
	int focus_id;

	command commands[MAX_COMMANDS];
	int command_count;
}context;
void ui_begin(context* ctx);
void ui_end(context* ctx);
int ui_button(context* ctx,int x, int y, int w, int h, const char* text);