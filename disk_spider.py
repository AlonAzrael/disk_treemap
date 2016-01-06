
from py_utils.Utils_Base import Dir, timeit_func

import json
import ujson
from scandir import scandir as _scandir
import os
import subprocess



PATH_JOIN = os.path.join
PATH_EXISTS = os.path.exists
PATH_SPLIT = os.path.split
CUR_PATH = os.path.split(os.path.realpath(__file__))[0]
PATH_PARENT = lambda mypath: os.path.abspath(os.path.join(mypath, os.path.pardir))

def get_path_size(path):
    return subprocess.check_output(['du','-sh', path]).split()[0]


def get_entry_size(entry):
    return entry.stat().st_size

def gen_directory_node(path, node):
    children = {}

    for entry in scandir(path):
        name = entry.name
        new_path = entry.path
        new_node = {}
        if entry.is_dir():
            gen_directory_node(new_path, new_node)

        new_node["size"] = get_entry_size(entry)
        children[name] = new_node

    node["children"] = children

def gen_directory_tree(path, root):
    node_list = [root]
    path_list = [path]

    while len(node_list)>0:
        node = node_list.pop(0)
        path = path_list.pop(0)

        children = {}
        for entry in scandir(path):
            name = entry.name
            new_path = entry.path
            new_node = {}
            if entry.is_dir():
                node_list.append(new_node)
                path_list.append(new_path)

            new_node["size"] = get_entry_size(entry)
            children[name] = new_node

        node["children"] = children

def get_dir_size():
    pass


def scandir(path):
    # return _scandir(path)
    path += "/*"
    cmd = "du -sh "+path
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    for line in p.stdout.readlines():
        print line.replace("")


"""

directory_size_tree:


"""

class DirTree_C():

    def __init__(self, root_path):
        self.NODE_TYPE_DIR = 0
        self.NODE_TYPE_FILE = 1
        self.cur_depth = 0
        self.max_depth = 5

        self.next_node_list = []
        self.next_path_list = []

        self.node_count = 0
        self.node_list = []
        self.root = self.init_root(root_path)

    def add_node_child(self, node, child):
        node["children"][child["name"]] = child
        # child["parent"] = node["_node_index"]
        
        depth = node["depth"] + 1
        child["depth"] = depth
        if self.cur_depth < depth:
            self.cur_depth = depth

        if child["_node_type"] == self.NODE_TYPE_DIR:
            self.next_node_list.append(child)
            self.next_path_list.append(child["path"])

    def create_node(self, entry, node_type):
        new_node = {
            "size":get_entry_size(entry),
            "_node_type": node_type,
            "path":entry.path,
            "name":entry.name,
            "_node_index": self.node_count,
        }
        
        if new_node["_node_type"] == self.NODE_TYPE_DIR:
            new_node["children"] = {}

        self.node_list.append(new_node)
        self.node_count += 1
        return new_node

    def set_node_size(self, node, size):
        node["size"] = size

    def init_root(self, root_path):
        if root_path.endswith("/"):
            root_path = root_path[:-1]
        if os.path.isfile(root_path):
            raise Exception(root_path+" is not dir")
            
        parent_path = PATH_PARENT(root_path)
        for entry in scandir(parent_path):
            if entry.path == root_path:
                root_node = self.create_node(entry, self.NODE_TYPE_DIR)
                self.next_node_list.append(root_node)
                self.next_path_list.append(root_node["path"])
                break
        else:
            raise Exception(root_path+" is error")

        root_node["depth"] = self.cur_depth
        return root_node

    def get_root(self):
        return self.root

    def has_next_task(self):
        if self.cur_depth >= self.max_depth:
            return False
        elif len(self.next_node_list)==0:
            return False
        else:
            return True

    def get_next_task(self):
        next_path = self.next_path_list.pop(0)
        next_node = self.next_node_list.pop(0)
        return next_path, next_node


def gen_directory_tree_by_size(root_path):
    dirtree = DirTree_C(root_path)
    
    while dirtree.has_next_task():
        path, node = dirtree.get_next_task()
        
        try:
            entry_iter = scandir(path)
        except OSError as e:
            print e
            continue
        
        for entry in entry_iter:
            
            if entry.is_dir():
                new_node = dirtree.create_node(entry, dirtree.NODE_TYPE_DIR)
            elif entry.is_file():
                new_node = dirtree.create_node(entry, dirtree.NODE_TYPE_FILE)
            else:
                print entry.path, "is strange !!!"
                continue

            dirtree.add_node_child(node, new_node)

    return dirtree


def test_gen_dirtree():
    global NODE
    # path = "/Users/aaronyin/HiFiveProject"
    path = "/Users/aaronyin"
    dirtree = gen_directory_tree_by_size(path)
    NODE = dirtree.get_root()


def test_gen_directory_node():
    global NODE
    path = "/Users/aaronyin/HiFiveProject"
    # path = "/Users/aaronyin/AaronYinProject"
    node = {}
    # gen_directory_node(path, node)
    gen_directory_tree(path, node)
    NODE = node

def test():
    dir_tree = Dir("dir_tree")
    print timeit_func("test_gen_dirtree")
    # dir_tree.set("test.json", json.dumps(NODE,ensure_ascii=False, indent=2))

def test_du():
    path = "/Users/aaronyin/AaronYinProject/DiskSpider_Demo/*"
    cmd = "du -d 10 -h "+path
    cmd = "du -d 10 -h /Users/aaronyin/AaronYinProject/*"
    # cmd = cmd.split()
    # cmd = ['du','-d','10','-h',path]

    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    for line in p.stdout.readlines():
        print line,
    
    # result = subprocess.check_output(cmd)
    # print result

def test_path():
    path = "/Users/aaronyin/Library/Group Containers/"
    print os.stat(path)
    # for entry in scandir(path):
    #     print get_entry_size(entry)

def test_scandir():
    path = "/Users/aaronyin/AaronYinProject/DiskSpider_Demo"
    print scandir(path)

def test_():
    pass

def test_best_getsize():
    timeit_func()

def c_code():
    node_arr = []
    dir = root
    new_node = new_node(stat(dir))
    node_arr.append(new_node)

    while True:


    for child in dir.children():
        if is_dir(child):

        elif is_file(child):
            dir.




if __name__ == '__main__':
    # test()
    # test_path()
    test_scandir()


