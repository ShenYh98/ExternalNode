#pragma once

#include <iostream>

/*
 *   webserver通讯层的数据结构
 */
typedef enum {
    GetPassWord,
    DevTableSave,
    ChannelTableSave,
    GetDevTable,
    GetChannelTable,
    AddDev,
    EditDev,
    DelDev,
    AddChannel,
    EditChannel,
    DelChannel,
    GetRTData
} taskId;

typedef enum {
    Yc,         // 遥测
    Yx,         // 遥信
    Yk,         // 遥控
    Yt          // 遥调
} RTtaskId;

typedef struct {
    int isOn;
    std::string dev_sn;
    RTtaskId id;
} s_RTtask;

typedef struct {
    std::string sourceName;     // 源名
    int value;                  // 值
    std::string unit;           // 单位
} s_RTSrcInfo;

/*
 *   串口的数据结构
 */

typedef struct {
    int baudrate;           // 波特率
    int databit;            // 数据为
    std::string stopbit;    // 停止位
    std::string parity;     // 奇偶性
    int vtime;              // 最小字节
    int vmin;               // 最迟响应时间
} SerialParamInfo;

typedef struct {
    int SerialId;               // 创建的id,通过打开文件来获取
    int statu;                  // 0未使能 1使能
    std::string SerialName;     // 驱动文件名
    std::string name;           // 串口名
    SerialParamInfo serialParamInfo;
} SerialIdInfo;

typedef struct {
    std::string devName;        // 设备名
    SerialIdInfo serialInfo;    // 串口
    std::string protocol;       // 规约
    int addr;                   // 地址
    std::string category;       // 类型
    std::string model;          // 型号
    std::string sn;             // sn号
    int deviceStatus;           // 设备状态
} DevInfo;

/*
 *   Setting数据结构
 */

typedef enum {
    Add,
    Edit,
    Del
} Action;

typedef struct {
    Action act;
    DevInfo devInfo;
    DevInfo oldDevInfo;
} srv_DevInfo;

typedef struct {
    Action act;
    SerialIdInfo serialInfo;
    SerialIdInfo oldSerialInfo;
} srv_SerialInfo;

/*
 *   task类数据结构
 */

typedef enum {
    ready,
    start,
    stop,
    cancel
} taskState;

typedef struct {
    std::string taskname;
    taskState taskstate;
} taskInfo;

/*
 *   modubs的数据结构
 */

typedef struct {
    uint8_t address;            // 报文头（地址）
    uint8_t func;               // 功能码（保持/写入）
    uint8_t* reg;               // 寄存器地址
    uint8_t* len;               // 数据长度
} modbusPkg;

typedef struct {
    std::string name;                   // 测点名称
    std::string description;            // 测点描述
    std::string dataType;               // 数据类型
    int res;                            // 是否请求
    int invalid;                        // 有效性
    std::vector<modbusPkg> mpkg;        // 一个测点相当于一帧，一帧可以读一个包也可以多个包，也可以一个包的连续寄存器
} modbusPoint;

typedef struct {
    std::string dataType;
    int idata;
    uint8_t u8Data;
    uint16_t u16Data;
    std::string sData;
} modbusDataInfo;

