import atexit
import requests
import time
import signal
import uuid
from cpuid import cpuid
import json
import licensepy

# 定义 URL
req_url = "http://127.0.0.1:8442/auth/license"
rel_url = "http://127.0.0.1:8442/inst/rel"
echo_url = "http://127.0.0.1:8442/inst/echo"


def get_cpu_id():
    eax, ebx, ecx, edx = cpuid(1)
    return f"{edx:08X}{eax:08X}"

device_id = ""
instance_id = str(uuid.uuid4())
cpu_id = get_cpu_id()

auth_req = {
    "uuid": instance_id,
    "cpuid": cpu_id,
    "mac": ["00:0c:29:93:59:dc", "00:0c:29:e4:6f:6c"],
}

def send_post_msg(url, en_data):
    headers = {"Content-Type": "application/octet-stream"}  # 设置内容类型为二进制数据
    response = requests.post(url, data=en_data, headers=headers)
    return response

def send_auth_req():
    data = json.dumps(auth_req)
    print(f"data type: {type(data)}, data: {data}")
    en_data = licensepy.encrypt_info(data)
    print(f"en_data: {en_data}")

    headers = {"Content-Type": "application/octet-stream"}  # 设置内容类型为二进制数据
    response = requests.post(req_url, data=en_data, headers=headers)

    print("Response:", response.content)

    de_data = licensepy.decrypt_info(response.content)
    print(f"de_data: {de_data}")


def send_rel_msg():
    if device_id:
        inst_rel_msg = {"deviceid": device_id, "uuid": instance_id}
        response = requests.post(rel_url, json=inst_rel_msg)
        print(response)


def init():
    atexit.register(send_rel_msg)


def main():
    # init()
    send_auth_req()
    while True:
        time.sleep(30)
        print(f'30s send echo req')
        msg = {"deviceid": device_id, "uuid": instance_id}
        response = send_post_msg(echo_url, encrypt_info(json.dumps(msg)))
        print(response)


def test():
    # 示例：AES 加解密
    plaintext = '{"action": "get_license", "user": "JohnDoe", "key": "123456"}'
    key = ""  # 密钥，必须与 C++ 端一致

    # 加密
    encrypted_data = licensepy.encrypt_info(plaintext)
    print(f"Encrypted: {encrypted_data}")


if __name__ == "__main__":
    try:
        main()
        # test()
    except Exception as e:
        print(f"error {e}")



# curl -X POST https://127.0.0.1:8445/auth/license -H "Content-Type: application/json"\
#      -d '{"uuid": "adfde574-ed2f-42d6-9d79-a09a98c67932", "cpuid": "1F8BFBFF00090675", "mac": ["00:0c:29:93:59:dc", "00:0c:29:e4:6f:6c"]}'

# 定义请求体