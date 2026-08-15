#include"opalpch.h"
#include"UI.h"

static int point_in_rect(vec2 point,Rect rect)
{
    return point.x >= rect.x && point.x <= rect.x + rect.w &&
		point.y >= rect.y && point.y <= rect.y + rect.h;
}

void ui_push_rect_cmd(context* ctx, Rect rect, unsigned int color) {
    if (ctx->command_count < MAX_COMMANDS) {
        command* cmd = &ctx->commands[ctx->command_count++];
        cmd->type = 0; // UI_CMD_RECT
        cmd->rect = rect;
        cmd->color = color;
    }
}
#define HASH_INITIAL 2166136261
static void hash(id* hash, const void* data, int size)
{
    const unsigned char* p = (const unsigned char*)data;
    while (size--)
    {
        *hash = (*hash ^ *p++) * 16777619;
    }
}
static id get_id(context* ctx, const char* str) {
    id id = HASH_INITIAL;
    hash(&id, str, (int)strlen(str));
    return id;
}

void ui_begin(context* ctx) 
{
    ctx->hover_id = 0;
    ctx->command_count = 0;
}
void ui_end(context* ctx)
{

}
int ui_button(context* ctx,int x, int y, int w, int h, const char* text)
{
    id id = get_id(ctx, text);
	Rect rect = { x, y, w, h };
    int mouse_over = point_in_rect(ctx->mousepos,rect);
    int clicked = 0;

    if (mouse_over && !ctx->mousedown) {
        ctx->hover_id = id;
    }

    // 2. 更新 Focus 状态 (按下)
    if (mouse_over && ctx->mousepress) {
        ctx->focus_id = id;
    }

    // 3. 判断有效点击 (拥有焦点 且 鼠标松开)
    if (ctx->focus_id == id && !ctx->mousedown) {
        clicked = 1;
        ctx->focus_id = 0; // 释放焦点
    }

    // 4. 生成绘制命令 (为渲染模块准备)
    unsigned int color = 0x333333FF; // 默认深灰色
    if (ctx->focus_id == id) {
        color = 0x555555FF; // 按下状态：亮一点
    }
    else if (ctx->hover_id == id) {
        color = 0x444444FF; // 悬停状态：稍微亮一点
    }
    ui_push_rect_cmd(ctx,rect, color);

    return clicked;
}
