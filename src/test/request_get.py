import requests

url = '127.0.0.1:8000'
response = requests.get(url)
print(response.status_code)     # 打印状态码
print(response.url)             # 打印请求url
print(response.headers)         # 打印头信息
print(response.cookies)         # 打印cookie信息
print(response.text)            #以文本形式打印网页源码