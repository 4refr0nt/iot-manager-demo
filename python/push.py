# iotManager
# RPI
# @auhtor Alex Suslov <https://github.com/alexsuslov>
# @created 2016-09-06
# The MIT License (MIT)
# Copyright (c) 2016 AlexSuslov
#
# Push notification
#
ids =["your-phone-ids"]
msg = "Alert! Kitchen: water lake detected!"
import httplib2
import json

http = httplib2.Http()
url= "https://onesignal.com/api/v1/notifications"
headers = {'Content-type': 'application/json'}
data = {
  "app_id": "8871958c-5f52-11e5-8f7a-c36f5770ade9",
  "include_player_ids":ids,
  "android_group":"IoT Manager",
  "contents": {
    "en": msg
  }
}
response, content = http.request(url, 'POST', headers=headers, body=json.dumps(data))
print(response.status, response.reason)
