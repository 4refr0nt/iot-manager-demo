=======

Demo client for IoT Manager

=======

## Install

Edit demo1.js and set connection (mqtt host, port, user and pass), then run

```
npm i
node demo1.js
```
## Valid messages for manual testing

- publish to topic `/IoTmanager/mobile_id/request` payload `{ "command": "getPages", "param":"" }`
- publish to topic `/IoTmanager/mobile_id/request` payload `{ "command": "getPageById", "param":20 }` where `20` - pageId (may be `10`,`20`,`30`,`40`)

## Where results?

look at
- `/IoTmanager/dev-0001/response`
- `/IoTmanager/dev-0001/config`
- `/IoTmanager/dev-0001/+/status`

## License

MIT
