#include "drivers/view/hal.h"
#include "drivers/view/view.h"
#include "drivers/view/core.h"
#include "drivers/view/msg.h"
#include <xbook/task.h>
#include <xbook/schedule.h>
#include <assert.h>

static void view_thread(void *arg)
{
    view_core_loop();
}

int view_core_init()
{
    if (view_screen_init() < 0) {
        return -1;
    }
    if (view_section_init() < 0) {
        view_screen_exit();
        return -1;
    }
    if (view_init() < 0) {
        view_screen_exit();
        view_mouse_exit();        
        return -1;
    }
    if (view_mouse_init() < 0) {
        view_screen_exit();
        view_mouse_exit();
        view_keyboard_exit();
        return -1;
    }
    if (view_keyboard_init() < 0) {
        view_screen_exit();
        view_mouse_exit();
        view_keyboard_exit();
        view_section_exit();
        return -1;
    }
    if (view_global_msg_init() < 0) {
        view_screen_exit();
        view_mouse_exit();
        view_keyboard_exit();
        view_section_exit();
        view_keyboard_exit();
        return -1;
    }
    kern_thread_start("driver-view", TASK_PRIO_LEVEL_NORMAL, view_thread, NULL);
    return 0;
}

int view_core_exit()
{
    view_screen_exit();
    view_mouse_exit();
    view_keyboard_exit();
    view_section_exit();
    view_exit();
    return 0;
}

/* 输入的获取 */
int view_core_loop()
{
    int i = 0;
    int has_event;
    view_msg_t msg;
    while (1) {
        if (!view_mouse_poll()) {
            i = 0;
        }
        #if 0
        if (!view_keyboard_poll()) {
            i = 0;
        }
        #endif
        i++;
        view_msg_reset(&msg);
        if (view_get_global_msg(&msg) < 0) {
            if (i > 300)
                task_yeild();
            continue;
        }
        if (is_view_msg_valid(&msg)) {
            /* 处理消息 */
            switch (view_msg_get_type(&msg)) {
            case VIEW_MSG_KEY_DOWN:
            case VIEW_MSG_KEY_UP:
                /* 键盘消息发送到聚焦的图层 */
                view_dispatch_keycode_msg(&msg);
                break;
            case VIEW_MSG_MOUSE_MOTION:
            case VIEW_MSG_MOUSE_LBTN_DOWN:
            case VIEW_MSG_MOUSE_LBTN_UP:
            case VIEW_MSG_MOUSE_LBTN_DBLCLK:
            case VIEW_MSG_MOUSE_RBTN_DOWN:
            case VIEW_MSG_MOUSE_RBTN_UP:
            case VIEW_MSG_MOUSE_RBTN_DBLCLK:
            case VIEW_MSG_MOUSE_MBTN_DOWN:
            case VIEW_MSG_MOUSE_MBTN_UP:
            case VIEW_MSG_MOUSE_MBTN_DBLCLK:
            case VIEW_MSG_MOUSE_WHEEL_UP:
            case VIEW_MSG_MOUSE_WHEEL_DOWN:
            case VIEW_MSG_MOUSE_WHEEL_LEFT:
            case VIEW_MSG_MOUSE_WHEEL_RIGHT:
                /* 鼠标消息发送到鼠标指针所在的图层 */
                view_dispatch_mouse_msg(&msg);
                break;
            default:
                /* 默认派发方式，发送消息给指定的目标 */
                view_dispatch_target_msg(&msg);
                break;
            }
        }
    }
}