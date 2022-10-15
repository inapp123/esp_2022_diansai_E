import tornado
import tornado.websocket
import tornado.ioloop
import tornado.web
import asyncio


class CamDetectStream(tornado.websocket.WebSocketHandler):
    live_web_sockets = set()

    def check_origin(self, origin):
        return True

    def open(self):
        ip = repr(self.request.remote_ip)
        print("Main(Tornado): Websocket open:",ip)
        self.set_nodelay(True)
        self.live_web_sockets.add(self)
        # self.write_message("")

    def on_message(self, message):
        print('Message incomming:', message)

    def on_close(self):
        ip = repr(self.request.remote_ip)
        self.live_web_sockets.remove(self)
        print("Main(Tornado): WebSocket closed:",ip)

    @classmethod
    def send_message(cls, message,binary=False):
        removable = set()
        for ws in cls.live_web_sockets:
            if not ws.ws_connection or not ws.ws_connection.stream.socket:
                removable.add(ws)
            else:
                try:
                    ws.write_message(message,binary=binary)
                except tornado.iostream.StreamClosedError as e:
                    cls.live_web_sockets.remove(ws)
        for ws in removable:
            cls.live_web_sockets.remove(ws)

async def sendsomeshit():
    i = 0
    while True:
        datagram = bytearray(2)
        datagram[0] = 0 #digital
        datagram[1] = i
        CamDetectStream.send_message(bytes(datagram),True)
        i = i + 1
        if i > 255:
            i = 0
        await asyncio.sleep(0.2)

if __name__ == "__main__":
    application = tornado.web.Application([
        (r"/ws_digital", CamDetectStream)
    ])
    print("Main:backend Service started at :8888")
    loop = asyncio.get_event_loop()
    loop.create_task(sendsomeshit())

    application.listen(80)
    tornado.ioloop.IOLoop.current().start()