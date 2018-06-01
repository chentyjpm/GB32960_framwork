# 库用于国标32960 新能源车辆的客户端库和中间件


由我根据国标结构实现

主要的库包含如下结构

1. can_data CAN数据管理器 
2. GB32960 GB数据管理器
3. can_dec 车辆CAN数据解析单元（根据车辆不同进行定制）

# GB32960 数据管理器

这里定义了所有国标字段的数据结构和内容，提供如下接口

对所有字段使用KEY VALUE方式操作 便于序列化

设置字段值接口

    bool cloudmsg_fullreport_setval(struct cloudmsg_fullreport_t *report, uint32_t id, uint32_t value);
    
数据输出结构体

```
struct cloudmsg_fullreport_t
{
	uint8_t vincode[VINCODE_SIZE];
	uint8_t batterycnt;
	uint8_t batterycodesize;
	uint8_t batterycode[GB_BATCODEMAX];

	struct cloudmsg_fullvehicleinfo_t fullvehicleinfo;
	struct cloudmsg_motorinfo_t motorinfo;
	struct cloudmsg_fuelcellinfo_t fuelcellinfo;
	struct cloudmsg_engineinfo_t engineinfo;
	struct cloudmsg_locationinfo_t locationinfo;
	struct cloudmsg_minmaxcellinfo_t minmaxcellinfo;
	struct cloudmsg_alarminfo_t alarminfo;
};
```

所有国标字段会被存储于这个结构体内

# CAN_DAT CAN数据管理器

这里定义了对CAN数据的解析和处理过程 后期优化用于对接DBC文件

CAN数据解析为CANID 和CANDAT的格式并进行缓存

CAN数据透过CANDEC接口解析并填写到GBreport中

初始化

    bool can_data_manager_init(struct can_data_manager_t *manager, struct can_dec_t *dec);
    
CAN数据输入    
    
    bool can_data_manager_canmsg_input(struct can_data_manager_t *manager, struct can_msg_t *msg);
    
CAN数据获取
    
    struct can_msg_t * can_data_manager_canmsg_get(struct can_data_manager_t *manager, uint32_t canid);

# CANDEC接口

CANDEC接口实现对CAN数据到 GB32960数据的转换

可加载模块定义

```
struct can_dec_t
{
	void *param;bool
	(*init)(void *param, struct cloudmsg_fullreport_t *report);bool
	(*fini)(void *param, struct cloudmsg_fullreport_t *report);bool
	(*deccanmsg)(void *param, struct cloudmsg_fullreport_t *report,
	        struct can_msg_t *msg);
};

```
