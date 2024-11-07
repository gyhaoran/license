# License Manager
> A simple license manager

## Feature
- License generator
- License activator


C++实现
License Server 模式：一个授权服务器管理多个客户端的授权，客户端需要通过网络请求服务器获取授权。
1. client每次启动都需要想server发送校验req， 内容json格式 {"CPU ID": "BFEBFBFF00050654", "MAC": ['ether_name', '00:62:0b:f2:ee:00']}
2. server校验client的req中的设备信息，计算sha256 hash，看看受否授权（server中有所有授权机器的信息）
  2.1 如果设备信息校验失败，返回失败响应
  2.2 如果成功，检查同一台机器支持的实例个数max_instance是否到达，如果到达最大实例个数，则返回失败响应
  2.3 都校验成功，返回成功响应。
3. client程序再退出连接的时候需要告诉server自己退出，server将对应机器的运行实例个数计数器减一
4. License Server需要设计心跳包服务，定时检测历史client是否还在，如果有异常退出的需要清理计数器个数
5. 使用libhv库实现C++异步通信