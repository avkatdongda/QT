#ifndef USB_FUN_H
#define USB_FUN_H
#ifdef __cplusplus
 extern "C" {
 #endif
extern unsigned char *h2c_align_mem_tmp;
extern unsigned char *c2h_align_mem_tmp;
extern unsigned int FRAME_CMD;

// 函数声明 - 注意：这里必须和定义一致！
// 你的 usb_init 函数定义为：int usb_init(unsigned char *buffer)
// 所以这里也要声明为有参数
int usb_init(unsigned char *buffer);

void usb_deinit(void);
unsigned int h2c_transfer(unsigned int size);
unsigned int c2h_transfer(unsigned int size);
int usb_read_frame(unsigned char size);
int send_custom_command(const char* hex_str);  // 新增的函数
#ifdef __cplusplus
}
#endif
#endif
