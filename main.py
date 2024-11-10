import requests
import json
import uuid
import random

# 定义 URL
url = "http://127.0.0.1:8442/auth/license"


instance_id = str(uuid.uuid4())

# 定义请求体
auth_req = {
    "UUID": instance_id,
    "CPU ID": "1F8BFBFF000906EA",
    "MAC": ["00:0c:29:93:59:dc", "00:0c:29:e4:6f:6c"]
}


def main():
    response = requests.post(url, json=auth_req)
    device_id = ""

    if response.status_code == 200:
        print("Request successful")
        print(response)
        rsp = response.json()
        print("Response:",rsp)
        if rsp:
            device_id = rsp.get("deviceid", "")
    else:
        print(f"Request failed with status code {response.status_code}")
        print("Response:", response.text)


# if device_id != "":
#     url = "http://127.0.0.1:8442/inst/rel"
#     rel_msg = {
#         "deviceid": device_id
#     }
#     response = requests.post(url, json=rel_msg)
#     print(response, response.json())


