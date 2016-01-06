
from snakefoot import Server


class DiskSpriderServer_C():

    def __init__(self):
        self.index_page = "index.html"


if __name__ == '__main__':
    s = Server(DiskSpriderServer_C())
    s.bind(host="0.0.0.0", port=10083, debug=True)
    s.run()

