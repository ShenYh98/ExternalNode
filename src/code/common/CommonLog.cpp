#include "CommonLog.h"

using namespace CommonLib;

CommonLog& CommonLog::getInstance() {
    static CommonLog& instance = createInstance();
    return instance;
}

CommonLog& CommonLog::createInstance() {
    static CommonLog instance;
    return instance;
}

CommonLog::CommonLog() {
}

CommonLog::~CommonLog() {
    spdlog::drop_all(); // 关闭并刷新所有日志记录器的缓冲区
}

void CommonLog::initLogParam(const std::string& path) {
    getLogParamByJson(path);
    setLogParam();
}

int CommonLog::getLogParamByJson(const std::string& path) {
    // 打开JSON文件
    std::ifstream file(path);
    if (!file.is_open()) {
        // 文件打开失败，进行相应的处理
        printf("file open failed\n");
        return 0;
    } else {
        // 文件成功打开，继续读取文件内容
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        printf("Json parsing error: %s\n", content.c_str());

        // 解析JSON数据
        json jsonData;
        try {
            jsonData = json::parse(content);

            // 验证JSON数据的有效性
            if (!jsonData.is_object()) {
                throw std::runtime_error("Invalid JSON data: not an object");
            }

            // 获取字段的值
            log_path = jsonData["log_path"].get<std::string>();
            log_level = jsonData["log_level"].get<int>();
            log_size = jsonData["log_size"].get<int>();
            log_num = jsonData["log_num"].get<int>();
            printf("log_level: %d, log_size: %d, log_num: %d, log_path: %s\n", log_level, log_size, log_num, log_path.c_str());

        } catch(const std::exception& e) {
            // Json解析错误
            printf("Json parsing error\n");
            return 0;
        }

        file.close();
    }

    return 0;
}

void CommonLog::setLogParam() {    
    // 设置文件路径大小参数
    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(this->log_path, this->log_size*1024*1024, this->log_num);
    fileLogger = std::make_shared<spdlog::logger>("fileLogger", fileSink);
    spdlog::register_logger(fileLogger);
    fileLogger->set_level((spdlog::level::level_enum)this->log_level); // 设置日志等级\
    // 设置日志格式为 "[YYYY-MM-DD HH:MM:SS] [log_level] log_message"
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
}

std::string CommonLog::getFileName(const std::string& fullPath) {
    // 查找最后一个路径分隔符的位置
    size_t lastSeparatorPos = fullPath.find_last_of("/\\");

    if (lastSeparatorPos != std::string::npos)
    {
        // 提取文件名部分
        std::string fileName = fullPath.substr(lastSeparatorPos + 1);
        return fileName;
    }

    // 如果找不到路径分隔符，返回完整路径
    return fullPath;
}
