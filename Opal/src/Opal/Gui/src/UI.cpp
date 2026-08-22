#include"opalpch.h"
#include"UI.h"
#include"Renderer.h"
void Opal::VM_Execute(OIcode* code, float* stack, int* top, void * input_data)
{
    bool should_execute = false;
    if (code->trigger_len > 0) {
        int temp_top = *top; 

        uint8_t* bc = code->trigger_bc;
        for (uint8_t i = 0; i < code->trigger_len; ) {
            uint8_t op = bc[i++];
            switch (op) {
            case OP_GET_VAL_OFFSET: 
            {
                uint8_t offset = bc[i++]; 
                if (input_data) 
                {
                    float val = *(float*)((uint8_t*)input_data + offset);
                    stack[++(*top)] = val;
                }
                else
                {
                    stack[++(*top)] = 0.0f; 
                }
                break;
            }
            //所有操作均为a对b
            case OP_GT: 
            {
                float b = stack[(*top)--]; 
                float a = stack[(*top)--]; 
                stack[++(*top)] = (a > b) ? 1.0f : 0.0f;
                break;
            }
            case OP_EQ:
            {
                float b = stack[(*top)--];
                float a = stack[(*top)--];
                stack[++(*top)] = (a == b) ? 1.0f : 0.0f;
                break;
            }

            case OP_LT: 
            {
                float b = stack[(*top)--]; 
                float a = stack[(*top)--]; 
                stack[++(*top)] = (a < b) ? 1.0f : 0.0f;
                break;
            }
            case OP_AND: 
            {
                float b = stack[(*top)--];
                float a = stack[(*top)--];
                bool res = (a > BOOL_THRESHOLD) && (b > BOOL_THRESHOLD);
                stack[++(*top)] = res ? 1.0f : 0.0f;
                break;
            }
            case OP_OR: 
            {
                float b = stack[(*top)--];
                float a = stack[(*top)--];
                bool res = (a > BOOL_THRESHOLD) || (b > BOOL_THRESHOLD);
                stack[++(*top)] = res ? 1.0f : 0.0f;
                break;
            }
            }
        }
        should_execute = (stack[temp_top] > BOOL_THRESHOLD);
    }
    else {
        should_execute = true; 
    }
    if (should_execute) {
        uint8_t* bc = code->bytecode;
        for (uint8_t i = 0; i < code->bc_len; ) {
            uint8_t op = bc[i++];
            switch (op) {
            case OP_GET_VAL_OFFSET: {
                uint8_t offset = bc[i++];
                if (input_data) {
                    float val = *(float*)((uint8_t*)code->data + offset);
                    stack[++(*top)] = val;
                }
                else {
                    stack[++(*top)] = 0.0f;
                }
                break;
            }
            case OP_PUSH_IMM_F32: {
                float imm;
                memcpy(&imm, &bc[i], sizeof(float));
                i += sizeof(float);
                stack[++(*top)] = imm;
                break;
            }
            case OP_ADD: {
                float b = stack[(*top)--];
                float a = stack[(*top)--];
                stack[++(*top)] = a + b;
                break;
            }
            case OP_WRITE_OFFSET: {
                uint8_t offset = bc[i++];
                if (code->data && *top >= 0) {
                    float val = stack[(*top)--];
                    *(float*)((uint8_t*)code->data + offset) = val;
                }
                break;
            }
            case OP_SUB: {
                if (*top >= 1) {
                    float b = stack[(*top)--];
                    float a = stack[(*top)--];
                    stack[++(*top)] = a - b;
                }
                break;
            }
            case OP_JUMP_IF_FALSE: {
                uint8_t jump_offset = bc[i++]; // 读取要跳过的字节数
                if (*top >= 0 && stack[(*top)--] <= BOOL_THRESHOLD) {
                    i += jump_offset; // 如果条件为假，直接跳过指定长度的字节码
                }
                break;
            }
            }
        }
    }
}

void Opal::DeclareCard(Context* ctx, OIcode* logic, RenderCommand* visual,void* input_data)
{
    if (logic)
    {
        VM_Execute(logic, ctx->vm_stack, &ctx->vm_top, input_data);
    }

    if (ctx->cmd_count < MAX_RENDER_CMDS) 
    {
        ctx->render_pool[ctx->cmd_count++] = *visual;
    }
}

void Opal::ui_begin(Context* ctx)
{
	ctx->cmd_count = 0;
    ctx->vm_top = -1;
}
void Opal::ui_end(Context* ctx)
{
    int width = 960;
    int height = 540;
    Renderer::RenderCommands(ctx->render_pool, ctx->cmd_count, width, height);
    ctx->vm_top = -1;
}
