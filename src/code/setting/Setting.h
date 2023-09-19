#pragma once

#include <iostream>
#include <ctime>
#include <cstdlib>


#include "MessageQueue.hpp"
#include "CommonLog.h"

#include "../../DataStructure.h"

#include "nlohmann/json.hpp"

using namespace CommonLib;
using json = nlohmann::json;

namespace SettingMiddleware {

class Setting
{
public:
    Setting();
    ~Setting();

private:
    void srvDevActionTask(const std::vector<srv_DevInfo>& msg, std::function<void(const srv_GetInfo &)> responder);

    void srvSerialActionTask(const std::vector<srv_SerialInfo>& msg, std::function<void(const srv_GetInfo &)> responder);

    void saveDevJson(const std::string& fileName);

    void saveSerialJson(const std::string& fileName);

    int loadJsonToDevList(const std::string& fileName);

    int loadJsonToSerList(const std::string& fileName);

private:
    std::vector<DevInfo> devList;

    std::string addDevPath;

    std::vector<SerialIdInfo> serialList;

    std::string addSerPath;

};

}



