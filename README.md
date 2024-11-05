# License Manager

使用C++实现licence管理
license文件已经生成了
1. 获取设备信息CPU ID 和 MAC地址组成下面格式的json字符串: {"CPU ID": cpuid, "MAC Address": mac_addr}，对字符串使用sha256 hash，得到设备的hash值: devce_hash
2. lincense文件名为：license.dat， 文件内容格式如下：header+data
  header格式：固定32字节， 1~4字节：公钥长度；5~8字节：加密信息的长度；9~12字节：加密信息签名值的长度；13~16字节，confused_aes_key长度; 后面16个字节保留字段
  data格式：跟在header后面，依次是公钥数据public_der、加密信息en_data，签名值sig、confused_aes_key内容
  public_der: 公钥的der编码格式（私钥已经有了ras256格式）
  confused_aes_key：8字节随机数据 + aes_key（32字节） + 8字节随机数据
  en_data：license_info：为下面的json格式，使用aes算法进行加密得到en_data，aes_key保存在confused_aes_key中
```json
{
    "id:": license_id,
    "app_info": app_hash,
    "max_instance": max_inst,
    "is_server": is_server,
    "device_info": device_hash,
    "ether_name": ether_name,
    "issue_date": issue_date,
    "expiration": expire_date
}
```
  sig：用私钥对加密前的原始信息license_info进行签名得到，使用SHA256方式hash

已有的C++代码：
获取设备信息的接口 string get_device_info(bool is_server, const std::string& ether_name)，你需要实现sha256 hash

需求：
1. 先帮忙实现license.dat文件的解析， 获取所有内容
2. 实现license校验的校验流程
    2.1 使用解出来ras_pub_key公钥，对签名进行验签，失败即退出程序
    2.2 获取解密后的license_info，从中获取是否为服务器版本（is_server字段），网卡名（ether_name字段），获取设备device_hash（device_info字段）
    2.3 计算设备actual_hash，比对device_hash是否一致，失败即退出程序
    2.4 判断license_info中发布时间issue_date和expiration到期时间有没有到期，到期则输出license超期，退出程序
