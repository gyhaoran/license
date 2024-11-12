import atexit
import requests
import time
import signal
import uuid
from cpuid import cpuid


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


# curl -X POST http://127.0.0.1:8442/auth/license -H "Content-Type: application/json"\
#      -d "{\"uuid\": \"adfde574-ed2f-42d6-9d79-a09a98c67932\", \"cpuid\": \"1F8BFBFF00090675\", \"mac\": [\"00:0c:29:93:59:dc\", \"00:0c:29:e4:6f:6c\"]}"

# 定义请求体
auth_req = {
    "uuid": instance_id,
    "cpuid": cpu_id,
    "mac": ["00:0c:29:93:59:dc", "00:0c:29:e4:6f:6c"],
}

print(f"auth_req: \n{auth_req}")


def send_auth_req():
    response = requests.post(req_url, json=auth_req)
    global device_id

    if response.status_code == 200:
        print("Request successful")
        print(response)
        rsp = response.json()
        print("Response:", rsp)
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
        print("error\n")
