import requests
import json

# 定义 URL
url = "http://192.168.174.130:8442/auth/license"

# 定义请求体
payload = {
    "CPU ID": "1F8BFBFF00090675",
    "MAC": ["00:0c:29:93:59:dc", "02:42:22:bb:96:60"]
}

# 发送 POST 请求
response = requests.post(url, json=payload)

device_id = ""

# 检查响应状态码
if response.status_code == 200:
    print("Request successful")
    print(response)
    rsp = response.json()
    print("Response:",rsp)
    device_id = rsp["deviceid"]
else:
    print(f"Request failed with status code {response.status_code}")
    print("Response:", response.text)


if device_id != "":
    url = "http://192.168.174.130:8442/inst/rel"
    rel_msg = {
        "deviceid": device_id
    }
    response = requests.post(url, json=rel_msg)
    print(response, response.json())

