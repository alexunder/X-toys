import requests
import json

r = requests.get("https://yunbi.com//api/v2/tickers/btccny.json")
retjson = json.loads(r.text.decode('utf-8'))
print(retjson.get("at"))
print(retjson.get("ticker").get("buy"))
