#include "app/entry.h"

#include "json.hpp"
#include <iostream>

using nlohmann::json;

void json_test()
{
    json response;
    response["status"] = "success";
    response["message"] = "Operation completed successfully";

    // 创建 id 对象
    json id;
    id["id"] = "123456";

    // 将 id 对象的内容添加到 response 对象中
    response.update(id);

    // 输出合并后的 JSON 对象
    std::cout << response.dump(4) << std::endl;
}

int main(int argc, char** argv)
{
    // json_test();
    lic::main_entry(argc, argv);
    
    return 0;
}
