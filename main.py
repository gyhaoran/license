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
    headers = {"Content-Type": "application/octet-stream"}
    response = requests.post(url, data=en_data, headers=headers)
    return response


def send_auth_req():
    data = json.dumps(auth_req)
    en_data = licensepy.encrypt_info(data)
    response = send_post_msg(req_url, en_data)

    if response.status_code == 200:
        rsp = licensepy.decrypt_info(response.content)
        print(f"rsp: {rsp}")
        global device_id
        if rsp:
            device_id = rsp.get("deviceid", "")
    else:
        print(f"Request failed with status code {response.status_code}")
        print("Response:", response.text)


def send_rel_msg():
    if device_id:
        inst_rel_msg = {"deviceid": device_id, "uuid": instance_id}
        response = requests.post(rel_url, json=inst_rel_msg)
        print(f"inst release ack: {response}")


def init():
    atexit.register(send_rel_msg)


def main():
    init()
    send_auth_req()
    while True:
        print(f"waitting 30s to send echo req")
        time.sleep(30)
        msg = {"deviceid": device_id, "uuid": instance_id}
        response = send_post_msg(echo_url, licensepy.encrypt_info(json.dumps(msg)))
        print(f"time echo ack: {response}")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"error {e}")
