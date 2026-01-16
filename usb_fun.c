#ifdef __cplusplus
 extern "C" {
 #endif
#include <Windows.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <strsafe.h>
#include <stdint.h>
#include <SetupAPI.h>
#include <INITGUID.H>
#include <WinIoCtl.h>
//#include <AtlBase.h>
#include <io.h>
#include "FTD3XX.h"
 #include "usb_fun.h"  // 包含自己的头文件

//#pragma comment(lib, "../pcie_speed/FTD3XX.lib")

DEFINE_GUID(GUID_DEVINTERFACE_FOR_FTDI_DEVICES,
     0x36fc9e60, 0xc465, 0x11cf, 0x80, 0x56, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);

unsigned char *h2c_align_mem_tmp;
unsigned char *c2h_align_mem_tmp;

static FT_HANDLE ftHandle;
static FT_STATUS ftStatus;

unsigned int FRAME_CMD = 0x813009c4;  // 原有的帧命令，用于触发数据传输 2500   32'f8130+2500

LARGE_INTEGER start;
LARGE_INTEGER stop;
LARGE_INTEGER freq;

// ============ 新增的32位指令发送功能 ============

/**
 * @brief 发送32位指令到FPGA
 * @param hex_str 十六进制字符串，如 "810002FF"
 * @return 成功返回1，失败返回0
 */
int send_custom_command(const char* hex_str)
{
    if (ftStatus != FT_OK || ftHandle == NULL)
    {
        printf("aERROR:USB device is not connected! \n"); //错误：USB设备未连接！
        return 0;
    }

    if (hex_str == NULL || strlen(hex_str) == 0)
    {
        printf("bERROR:Input is empty! \n");  //错误：输入为空！
        return 0;
    }
    // 1. 解析十六进制字符串为32位整数
    uint32_t command_data = 0;
    char* endptr;

    // 跳过可能的 "0x" 前缀
    const char* str = hex_str;
    if (strlen(hex_str) > 2 &&
        hex_str[0] == '0' &&
        (hex_str[1] == 'x' || hex_str[1] == 'X'))
    {
        str = hex_str + 2;
    }

    // 简单的十六进制解析
    command_data = (uint32_t)strtoul(str, &endptr, 16);

    // 检查解析是否成功
    if (endptr == str || *endptr != '\0')
    {
        printf("cERROR:The data '%s' cannot be parsed! \n", hex_str); //错误：无法解析十六进制数据
        printf("d请使用类似 '810002FF' 或 '0x810002FF' 的格式\n");
        return 0;
    }
    // 3. 发送数据到USB设备
    ULONG length = sizeof(uint32_t);  // 固定4字节
    unsigned long writtenLen;

    ftStatus = FT_WritePipe(ftHandle, 0x02, (UCHAR*)&command_data, length, &writtenLen, NULL);

    if (FT_FAILED(ftStatus))
    {
        printf("eERROR:Sending failed! Error code %d\n", ftStatus);  //错误: 发送失败！错误代码:
        return 0;
    }
    printf("Sending successful! %d\n", ftStatus);

    if (writtenLen != length)
    {
        printf("f警告: 数据发送不完整！期望4字节，实际发送%lu字节\n", writtenLen);
        return 0;
    }

    // 4. 显示确认信息
    printf("0x%08X has been sent.\n", command_data);

    return 1;
}


// ============ 原有的其他函数保持不变 ============
static int verbose_msg(const char* const fmt, ...) {

    int ret = 0;
    va_list args;
    if (1) {
        va_start(args, fmt);
        ret = vprintf(fmt, args);
        va_end(args);
    }
    return ret;

}
static BYTE* allocate_buffer(size_t size, size_t alignment) {

    if(size == 0) {
        size = 4;
    }

    if (alignment == 0) {
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        alignment = sys_info.dwPageSize;
        //printf("alignment = %d\n",alignment);
    }
    verbose_msg("Allocating host-side buffer of size %d, aligned to %d bytes\n", size, alignment);
    return (BYTE*)_aligned_malloc(size, alignment);

}

static void open_devices( int dev )
{
    //FT_INVALID_HANDLE
        USHORT uwVID = 0;
        USHORT uwPID = 0;

        GUID DeviceGUID[2] = { 0 };
        memcpy(&DeviceGUID[0], &GUID_DEVINTERFACE_FOR_FTDI_DEVICES, sizeof(GUID));
        ftStatus = FT_Create(&DeviceGUID[0], FT_OPEN_BY_GUID, &ftHandle);

        if (FT_FAILED(ftStatus))
        {
            verbose_msg("open fail");
            return ;
        }

        ftStatus = FT_GetVIDPID(ftHandle, &uwVID, &uwPID);
        if (FT_FAILED(ftStatus))
        {
            FT_Close(ftHandle);
        }
}

static BOOL read_device( UCHAR* buff, ULONG *length )
{
    if (ftStatus != FT_OK)
    {
        return FALSE;
    }
    unsigned long readLen;
    ftStatus = FT_ReadPipe(ftHandle, 0x82, buff, *length, &readLen, NULL);

    BOOL result = TRUE;
    if (FT_FAILED(ftStatus) || readLen != *length)
    {
        result = FALSE;
    }
    *length = readLen;
    return result;
}

static BOOL write_device( UCHAR *data, ULONG *length )
{
    if (ftStatus != FT_OK)
    {
        return FALSE;
    }
    unsigned long writenLen;

    ftStatus = FT_WritePipe(ftHandle, 0x02, data, *length, &writenLen, NULL);

    BOOL result = TRUE;
    if (FT_FAILED(ftStatus) || writenLen != *length)
    {
        result = FALSE;
    }
    return result;
}


int usb_init(unsigned char *buffer)
{
    printf("usb_init ok\n");
    int res = 1;
    open_devices(0);
    h2c_align_mem_tmp = allocate_buffer(0x80000000,8192);
    c2h_align_mem_tmp = allocate_buffer(0x80000000,8192);//开辟1MB
    if(NULL == h2c_align_mem_tmp || NULL == c2h_align_mem_tmp)
        return 0;

    return res;
}
void usb_deinit()
{
    FT_Close(ftHandle);

    ftHandle = NULL;
    ftStatus = (FT_STATUS)6;

    if (h2c_align_mem_tmp != NULL)
    {
        _aligned_free(h2c_align_mem_tmp);
        h2c_align_mem_tmp = NULL;
    }
    if (c2h_align_mem_tmp != NULL)
    {
        _aligned_free(c2h_align_mem_tmp);
        c2h_align_mem_tmp = NULL;
    }
}
unsigned int h2c_transfer(unsigned int size)//写入，上位机发向下位机
{
    double bd=0;
    double time_sec;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    write_device(h2c_align_mem_tmp,(ULONG *)&size);
    QueryPerformanceCounter(&stop);
    time_sec = (unsigned long long)(stop.QuadPart - start.QuadPart) / (double)freq.QuadPart;
    bd = ((double)size)/(double)(time_sec)/1024.0/1024.0;

    return (unsigned int)bd;
}
unsigned int c2h_transfer(unsigned int size)//读，size=image_h*image*4;
{
    double bd=0;
    double time_sec;
    int len = 4;

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&stop);
    time_sec = (unsigned long long)(stop.QuadPart - start.QuadPart) / (double)freq.QuadPart;
    bd = (int)(1.0/time_sec);;
    QueryPerformanceCounter(&start);

    write_device((UCHAR *)&FRAME_CMD,(ULONG *)&len); // fpga frame(帧) cmd
    read_device(c2h_align_mem_tmp,(ULONG *)&size);


    return (unsigned int)bd;
}


#ifdef __cplusplus
}
#endif
