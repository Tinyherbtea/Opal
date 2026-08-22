#pragma once
#include<stdint.h>

namespace Opal
{
	typedef struct Rect { int x, y, w, h; };
	typedef struct vec2 { int x, y; };

#define OP_PUSH_IMM_F32		 0x02
#define OP_ADD				 0x03
#define OP_SUB				 0x06 
#define OP_CLAMP			 0x04
#define OP_WRITE_OFFSET		 0x05
#define OP_GET_VAL_OFFSET	 0x10 
#define OP_GT                0x09 //greater than
#define OP_EQ                0x0A //equal
#define OP_LT                0x0D //less than
#define OP_AND               0x0B 
#define OP_OR                0x0C  
#define OP_JUMP_IF_FALSE  0x07 

#define BOOL_THRESHOLD 0.0f
	typedef struct {
		void* data;
		uint8_t trigger_bc[32];
		uint8_t trigger_len;

		uint8_t bytecode[32];   
		uint8_t bc_len;         
	} OIcode;

	typedef struct {
		float x, y, w, h;
		uint32_t color; 
	} RenderCommand;

	typedef struct {
		float mouse_x;      // 偏移量 0  (占 4 字节)
		float mouse_y;      // 偏移量 4  (占 4 字节)
		uint8_t mousedown;  // 偏移量 8  (占 1 字节，但注意内存对齐)
	} InputState;

#define MAX_RENDER_CMDS 1024
	typedef struct 
	{
		RenderCommand render_pool[MAX_RENDER_CMDS];
		uint32_t cmd_count;

		float vm_stack[16];
		int   vm_top;

		InputState state;
	} Context;
	
	void VM_Execute(OIcode* code, float* stack, int* top,void* input_data);

	void DeclareCard(Context* ctx,OIcode* logic ,RenderCommand* visual, void* input_data);

	void ui_begin(Context* ctx);
	void ui_end(Context* ctx);
}
