import atexit
import requests
import time
import signal
import uuid
from cpuid import cpuid
import json
import ssl
from requests.packages.urllib3.poolmanager import PoolManager

class Ssl1Adapter(requests.adapters.HTTPAdapter):
    def init_poolmanager(self, connections, maxsize, block=False):
        self.poolmanager = PoolManager(
            num_pools=connections,
            maxsize=maxsize,
            block=block,
            ssl_version=ssl.PROTOCOL_TLSv1_2
        )


# 定义 URL
req_url = "https://127.0.0.1:8445/auth/license"
rel_url = "https://127.0.0.1:8445/inst/rel"
echo_url = "https://127.0.0.1:8445/inst/echo"


def get_cpu_id():
    eax, ebx, ecx, edx = cpuid(1)
    return f"{edx:08X}{eax:08X}"


device_id = ""
instance_id = str(uuid.uuid4())
cpu_id = get_cpu_id()


# curl -X POST https://127.0.0.1:8445/auth/license -H "Content-Type: application/json"\
#      -d '{"uuid": "adfde574-ed2f-42d6-9d79-a09a98c67932", "cpuid": "1F8BFBFF00090675", "mac": ["00:0c:29:93:59:dc", "00:0c:29:e4:6f:6c"]}'

# 定义请求体
auth_req = {
    "uuid": instance_id,
    "cpuid": cpu_id,
    "mac": ["00:0c:29:93:59:dc", "00:0c:29:e4:6f:6c"],
}


# 创建会话并安装自定义适配器
session = requests.Session()
session.mount('https://', Ssl1Adapter())


def send_auth_req():
    req = json.dumps(auth_req)
    print(f"auth_req: {auth_req}\n")

    headers_ = {"Content-Type": "application/json"}

    # # 发送请求，验证服务器证书
    # response = session.post(url, data=json_data, headers=headers, verify="server.crt")
    # response.raise_for_status()  # 检查请求是否成功

    response = session.post(req_url, data=req, headers=headers_, verify="./config/server.crt")
    print(response)
    response.raise_for_status()  # 检查请求是否成功
    response_data = response.json()
    print("Response:", response_data)
    
    # response = requests.post(req_url, json=auth_req)
    global device_id

    if response.status_code == 200:
        print("Request successful")
        print(response)
        rsp = response.json()
        print("Response: ", rsp)
        if rsp:
            device_id = rsp.get("deviceid", "")
    else:
        print(f"Request failed with status code {response.status_code}")
        print("Response:", response.text)


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
        response = requests.post(echo_url, json=msg)
        print(response)


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"error {e}")
