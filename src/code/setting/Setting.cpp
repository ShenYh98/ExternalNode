#include "Setting.h"

using namespace SettingMiddleware;

Setting::Setting(/* args */) {
    addDevPath = "data/csi_devtable.json";
    addSerPath = "data/csi_channeltable.json";

    loadJsonToDevList(addDevPath);
    loadJsonToSerList(addSerPath);

    ServiceQueue< std::vector<srv_DevInfo>, std::string >::getInstance().service_subscribe("devTable/addDev", 
                    std::bind(&Setting::srvDevActionTask, this, std::placeholders::_1, std::placeholders::_2));

    ServiceQueue< std::vector<srv_SerialInfo>, std::string >::getInstance().service_subscribe("SerialTable/addSerial", 
                    std::bind(&Setting::srvSerialActionTask, this, std::placeholders::_1, std::placeholders::_2));

    MessageQueue<std::vector<DevInfo>>::getInstance().publish("Setting/DevList", devList);
    MessageQueue<std::vector<SerialIdInfo>>::getInstance().publish("Setting/serialList", serialList);

    printHello();
}

Setting::~Setting()
{
}

void Setting::srvDevActionTask(const std::vector<srv_DevInfo>& msg, std::function<void(const std::string&)> responder) {
    LOG_DEBUG("msg action: {}\n", msg[0].act);
    int res = 0;

    switch (msg[0].act)
    {
    case Action::Add : {
        LOG_DEBUG("msg add addr: {}\n", msg[0].devInfo.addr);
        LOG_DEBUG("msg add category: {}\n", msg[0].devInfo.category);
        LOG_DEBUG("msg add deviceStatus: {}\n", msg[0].devInfo.deviceStatus);
        LOG_DEBUG("msg add devName: {}\n", msg[0].devInfo.devName);
        LOG_DEBUG("msg add model: {}\n", msg[0].devInfo.model);
        LOG_DEBUG("msg add protocol: {}\n", msg[0].devInfo.protocol);
        LOG_DEBUG("msg add serialInfo: {}\n", msg[0].devInfo.serialInfo.name);
        LOG_DEBUG("msg add sn: {}\n", msg[0].devInfo.sn);

        auto device = msg[0].devInfo;
        devList.push_back(device);
        saveDevJson(addDevPath);

        res = 1;

        break;
    }
    case Action::Edit : {
        LOG_DEBUG("msg edit addr: {}\n", msg[0].devInfo.addr);
        LOG_DEBUG("msg edit category: {}\n", msg[0].devInfo.category);
        LOG_DEBUG("msg edit deviceStatus: {}\n", msg[0].devInfo.deviceStatus);
        LOG_DEBUG("msg edit devName: {}\n", msg[0].devInfo.devName);
        LOG_DEBUG("msg edit model: {}\n", msg[0].devInfo.model);
        LOG_DEBUG("msg edit protocol: {}\n", msg[0].devInfo.protocol);
        LOG_DEBUG("msg edit serialInfo: {}\n", msg[0].devInfo.serialInfo.name);
        LOG_DEBUG("msg edit sn: {}\n", msg[0].devInfo.sn);  
        
        LOG_DEBUG("old msg edit addr: {}\n", msg[0].oldDevInfo.addr);
        LOG_DEBUG("old msg edit category: {}\n", msg[0].oldDevInfo.category);
        LOG_DEBUG("old msg edit deviceStatus: {}\n", msg[0].oldDevInfo.deviceStatus);
        LOG_DEBUG("old msg edit devName: {}\n", msg[0].oldDevInfo.devName);
        LOG_DEBUG("old msg edit model: {}\n", msg[0].oldDevInfo.model);
        LOG_DEBUG("old msg edit protocol: {}\n", msg[0].oldDevInfo.protocol);
        LOG_DEBUG("old msg edit serialInfo: {}\n", msg[0].oldDevInfo.serialInfo.name);
        LOG_DEBUG("old msg edit sn: {}\n", msg[0].oldDevInfo.sn);

        auto device = msg[0].devInfo;
        auto olddevice = msg[0].oldDevInfo;
        auto it = std::find_if(devList.begin(), devList.end(),
        [&olddevice](const DevInfo& dev) {
            return dev.devName == olddevice.devName;
        });
        if (it != devList.end())
        {
            *it = device;
            res = 1;
        }
        else
        {
            res = 0;
        }
        saveDevJson(addDevPath);

        break;
    }
    case Action::Del : {
        for (auto it : msg) {
            auto device = it.devInfo; 
            auto findDel = std::find_if(devList.begin(), devList.end(),
                                        [&device](const DevInfo& dev) {
                                            return dev.devName == device.devName;
                                        });
            if (findDel != devList.end())
            {
                devList.erase(findDel);
                res = 1;
            }
            else
            {
                res = 0;
            }
        }

        saveDevJson(addDevPath); 

        break;
    }

    default:
        break;
    }
    
    if (1 == res) {
        responder("ok");

        MessageQueue<std::vector<DevInfo>>::getInstance().publish("Setting/DevList", devList);
    } else {
        responder("fail");
    }
}

void Setting::srvSerialActionTask(const std::vector<srv_SerialInfo>& msg, std::function<void(const std::string&)> responder) {
    LOG_DEBUG("msg action: {}\n", msg[0].act);
    int res = 0;

    switch (msg[0].act)
    {
    case Action::Add : {
        LOG_DEBUG("msg add name: {}\n", msg[0].serialInfo.name);
        LOG_DEBUG("msg add ceate id: {}\n", msg[0].serialInfo.SerialId);
        LOG_DEBUG("msg add file status (1,0): {}\n", msg[0].serialInfo.statu);
        LOG_DEBUG("msg add file name: {}\n", msg[0].serialInfo.SerialName);
        LOG_DEBUG("msg add serial baudrate: {}\n", msg[0].serialInfo.serialParamInfo.baudrate);
        LOG_DEBUG("msg add serial databit: {}\n", msg[0].serialInfo.serialParamInfo.databit);
        LOG_DEBUG("msg add serial parity: {}\n", msg[0].serialInfo.serialParamInfo.parity);
        LOG_DEBUG("msg add serial stopbit: {}\n", msg[0].serialInfo.serialParamInfo.stopbit);
        LOG_DEBUG("msg add serial vmin: {}\n", msg[0].serialInfo.serialParamInfo.vmin);
        LOG_DEBUG("msg add serial vtime: {}\n", msg[0].serialInfo.serialParamInfo.vtime);

        auto serial = msg[0].serialInfo;
        serialList.push_back(serial);
        //
        saveSerialJson(addSerPath);

        res = 1;

        break;
    }
    case Action::Edit : {
        LOG_DEBUG("msg add name: {}\n", msg[0].serialInfo.name);
        LOG_DEBUG("msg add file name: {}\n", msg[0].serialInfo.SerialName);
        LOG_DEBUG("msg add serial baudrate: {}\n", msg[0].serialInfo.serialParamInfo.baudrate);
        LOG_DEBUG("msg add serial databit: {}\n", msg[0].serialInfo.serialParamInfo.databit);
        LOG_DEBUG("msg add serial parity: {}\n", msg[0].serialInfo.serialParamInfo.parity);
        LOG_DEBUG("msg add serial stopbit: {}\n", msg[0].serialInfo.serialParamInfo.stopbit);
        LOG_DEBUG("msg add serial vmin: {}\n", msg[0].serialInfo.serialParamInfo.vmin);
        LOG_DEBUG("msg add serial vtime: {}\n", msg[0].serialInfo.serialParamInfo.vtime);
 
        
        LOG_DEBUG("msg add name: {}\n", msg[0].oldSerialInfo.name);
        LOG_DEBUG("msg add file name: {}\n", msg[0].oldSerialInfo.SerialName);
        LOG_DEBUG("msg add serial baudrate: {}\n", msg[0].oldSerialInfo.serialParamInfo.baudrate);
        LOG_DEBUG("msg add serial databit: {}\n", msg[0].oldSerialInfo.serialParamInfo.databit);
        LOG_DEBUG("msg add serial parity: {}\n", msg[0].oldSerialInfo.serialParamInfo.parity);
        LOG_DEBUG("msg add serial stopbit: {}\n", msg[0].oldSerialInfo.serialParamInfo.stopbit);
        LOG_DEBUG("msg add serial vmin: {}\n", msg[0].oldSerialInfo.serialParamInfo.vmin);
        LOG_DEBUG("msg add serial vtime: {}\n", msg[0].oldSerialInfo.serialParamInfo.vtime);


        auto serial = msg[0].serialInfo;
        auto oldSerial = msg[0].oldSerialInfo;
        auto it = std::find_if(serialList.begin(), serialList.end(),
            [&oldSerial](const SerialIdInfo& serial) {
                return serial.name == oldSerial.name;
            });
        if (it != serialList.end())
        {
            *it = serial;
            res = 1;
        }
        else
        {
            res = 0;
        }

        saveSerialJson(addSerPath);
        
        break;
    }
    case Action::Del : {
        for (auto it : msg) {
            auto serial = it.serialInfo; 
            auto findDel = std::find_if(serialList.begin(), serialList.end(),
                                        [&serial](const SerialIdInfo& ser) {
                                            return ser.name == serial.name;
                                        });
            if (findDel != serialList.end())
            {
                serialList.erase(findDel);
                res = 1;
            }
            else
            {
                res = 0;
            }
        }

        saveSerialJson(addSerPath); 

        break;
    }

    default:
        break;
    }
    
    if (1 == res) {
        responder("ok");

        MessageQueue<std::vector<SerialIdInfo>>::getInstance().publish("Setting/serialList", serialList);
    } else {
        responder("fail");
    }
}

void Setting::saveDevJson(const std::string& fileName) {
    // 创建一个json
    json j;

    // 创建一个json数组
    json devArray = json::array();

    for (auto dev : devList) {
        json j_dev;
        j_dev["name"] = dev.devName;
        j_dev["port"] = dev.serialInfo.name;
        j_dev["protocol"] = dev.protocol;
        j_dev["communicationAddress"] = std::to_string(dev.addr);
        j_dev["category"] = dev.category;
        j_dev["model"] = dev.model;
        j_dev["sn"] = dev.sn;
        j_dev["deviceStatus"] = std::to_string(dev.deviceStatus);

        devArray.push_back(j_dev);
    }

    j["data"]["table"] = devArray;

    // 打开文件并保存json数组
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }
    file << j.dump(4);  // 使用4个空格进行格式化
    file.close();
}

void Setting::saveSerialJson(const std::string& fileName) {
    // 创建一个json
    json j;

    // 创建一个json数组
    json serArray = json::array();

    for (auto ser : serialList) {
        json j_ser;
        j_ser["port"] = ser.SerialName;
        j_ser["portName"] = ser.name;
        j_ser["baudRate"] = std::to_string(ser.serialParamInfo.baudrate);
        j_ser["dataBits"] = std::to_string(ser.serialParamInfo.databit);
        j_ser["stopBits"] = ser.serialParamInfo.stopbit;
        j_ser["parity"] = ser.serialParamInfo.parity;
        j_ser["vtime"] = std::to_string(ser.serialParamInfo.vtime);
        j_ser["vmin"] = std::to_string(ser.serialParamInfo.vmin);
        serArray.push_back(j_ser);
    }

    j["data"]["table"] = serArray;

    // 打开文件并保存json数组
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }
    file << j.dump(4);  // 使用4个空格进行格式化
    file.close();
}

int Setting::loadJsonToDevList(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return 0;
    }

    std::string fileContent((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    file.close();

    try {
        json j = json::parse(fileContent);

        json devArray = j["data"]["table"];

        devList.clear();

        for (const auto& j_dev : devArray) {
            DevInfo dev;
            dev.devName = j_dev["name"].get<std::string>();
            dev.serialInfo.name = j_dev["port"].get<std::string>();
            dev.protocol = j_dev["protocol"].get<std::string>();
            dev.addr = std::stoi(j_dev["communicationAddress"].get<std::string>());
            dev.category = j_dev["category"].get<std::string>();
            dev.model = j_dev["model"].get<std::string>();
            dev.sn = j_dev["sn"].get<std::string>();
            dev.deviceStatus = std::stoi(j_dev["deviceStatus"].get<std::string>());
            devList.push_back(dev);
        }
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    return 1;
}

int Setting::loadJsonToSerList(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return 0;
    }
  
    std::string fileContent((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    file.close();

    try {
        json j = json::parse(fileContent);

        json serArray = j["data"]["table"];
        
        serialList.clear();

        for (const auto& j_ser : serArray) {
            SerialIdInfo ser;
            ser.SerialName = j_ser["port"].get<std::string>();
            ser.name = j_ser["portName"].get<std::string>();
            ser.serialParamInfo.baudrate = std::stoi(j_ser["baudRate"].get<std::string>());
            ser.serialParamInfo.databit = std::stoi(j_ser["dataBits"].get<std::string>());
            ser.serialParamInfo.stopbit = j_ser["stopBits"].get<std::string>();
            ser.serialParamInfo.parity = j_ser["parity"].get<std::string>();
            ser.serialParamInfo.vtime = std::stoi(j_ser["vtime"].get<std::string>());
            ser.serialParamInfo.vmin = std::stoi(j_ser["vmin"].get<std::string>());
            serialList.push_back(ser);
        }
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    return 1;
}
