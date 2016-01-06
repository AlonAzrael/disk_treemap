
package main

import (
    "encoding/json"
    "fmt"
    "log"
    "os/exec"
    "bytes"
    "strings"
    "strconv"
    "path"
    "io/ioutil"
    "container/list"
    "github.com/julienschmidt/httprouter"
    "net/http"
)


type DirTree struct {
    NodeMap map[string]*DirTreeNode
}
func (dir_tree *DirTree) GetNode(node_path string) (new_node *DirTreeNode){
    return dir_tree.NodeMap[node_path]
}
func (dir_tree *DirTree) SetNode(node_path string, node *DirTreeNode) {
    dir_tree.NodeMap[node_path] = node
}
func gen_dirtree(root_path string, root_node *DirTreeNode) *DirTree {
    dir_tree := new(DirTree)
    dir_tree.NodeMap = make(map[string]*DirTreeNode)

    node_list := list.New()
    node_list.PushBack(root_node)

    path_list := list.New()
    path_list.PushBack(root_path)
    
    dir_tree.SetNode(root_path, root_node)

    for node_list.Len()>0 {
        cur_node := node_list.Remove(node_list.Front()).(*DirTreeNode)
        cur_path := path_list.Remove(path_list.Front()).(string)

        for _,child := range cur_node.Children {

            child_path := path.Join(cur_path, child.Name)
            dir_tree.SetNode(child_path, child)

            if child.Children != nil { // is dir
                node_list.PushBack(child)
                path_list.PushBack(child_path)
            } else { // is file
                
            }

        }
    }

    return dir_tree
}


type DirTreeNode struct {
    Name string `json:"name"`
    // Path string
    Depth uint `json:"depth"`
    Size uint64 `json:"size"`
    // Flag uint8 // dir or file
    
    Children []*DirTreeNode `json:"children"`
}

func if_error(e error) {
    if e != nil {
        panic(e)
    }
}

func loads_dirtree_json(filepath string) *DirTreeNode {
    data, err := ioutil.ReadFile(filepath)
    if_error(err)
    // dir_tree_str := string(data)

    root_node := new(DirTreeNode)
    err = json.Unmarshal(data, root_node)
    if_error(err)

    return root_node
}

func dumps_dirtree_json(root_node *DirTreeNode) (s string){
    dir_tree_bytes, dir_tree_bytes_err := json.MarshalIndent(root_node, "", "  ")
    if_error(dir_tree_bytes_err)

    dir_tree_str := string(dir_tree_bytes)
    
    // for debug
    fmt.Println(dir_tree_str)

    return dir_tree_str
}

func copy_dirtree_node(node *DirTreeNode) *DirTreeNode{
    new_node := new(DirTreeNode)
    new_node.Name = node.Name
    new_node.Size = node.Size
    new_node.Children = make([]*DirTreeNode,0)
    return new_node
}

func get_part_dirtree_by_depth(root_node *DirTreeNode, max_depth uint) *DirTreeNode {
    
    node_list := list.New()
    node_list.PushBack(root_node)

    new_node_list := list.New()
    new_root_node := copy_dirtree_node(root_node)
    new_root_node.Depth = 0
    new_node_list.PushBack(new_root_node)

    // path_list := list.New()
    // path_list.PushBack(root_path)

    for node_list.Len()>0 {
        cur_node := node_list.Remove(node_list.Front()).(*DirTreeNode)
        cur_new_node := new_node_list.Remove(new_node_list.Front()).(*DirTreeNode)

        for _,child := range cur_node.Children {
            
            child.Depth = cur_node.Depth + 1
            var new_child *DirTreeNode
            if child.Depth < max_depth {
                new_child = copy_dirtree_node(child)
                cur_new_node.Children = append(cur_new_node.Children, new_child)
            } else { // reach max depth
                return new_root_node
            }

            if child.Children != nil { // is dir
                node_list.PushBack(child)
                new_node_list.PushBack(new_child)
            } else { // is file
                // file has no Children
                new_child.Children = nil
            }

        }
    }

    return new_root_node
}

func call_disk_spider_dirtree_c(root_path string) string{
    // filepath := "./dir_tree/ds_c.json_"
    
    cmd := exec.Command("./disk_spider_dirtree_c",root_path)
    cmd.Stdin = strings.NewReader("")
    out := new(bytes.Buffer)
    cmd.Stdout = out

    err := cmd.Run()
    if_error(err)
    filepath := out.String()
    return filepath
}

func start_server() {
    router := httprouter.New()
    router.GET("/", Index)
    router.POST("/refresh_dirtree", refresh_dirtree)
    router.POST("/req_dirtree", req_dirtree)

    log.Fatal(http.ListenAndServe("0.0.0.0:10087", router))
}


func Index(w http.ResponseWriter, r *http.Request, _ httprouter.Params) {
    fmt.Fprintf(w, "Hello, World!\n")
}

var DIRTREE_FILE_PATH string
var DIR_TREE *DirTree
var ROOT_NODE *DirTreeNode
var ROOT_PATH = "/Users/aaronyin/AaronYinProject/DiskSpider_Demo/test_dir_good"

func refresh_dirtree(w http.ResponseWriter, r *http.Request, _ httprouter.Params) {
    root_path := ROOT_PATH
    filepath := call_disk_spider_dirtree_c(root_path)
    fmt.Println(filepath)
    DIRTREE_FILE_PATH = filepath

    ROOT_NODE = loads_dirtree_json(filepath)
    // dumps_dirtree_json(root_node)
    DIR_TREE = gen_dirtree(root_path, ROOT_NODE)
    
    // for key, val := range dir_tree.NodeMap {
    //     fmt.Println(key,val)
    // }
}

func req_dirtree(w http.ResponseWriter, r *http.Request, _ httprouter.Params) {
    max_depth,_ := strconv.ParseUint(r.FormValue("max_depth"), 10, 32)
    root_node := DIR_TREE.GetNode(ROOT_PATH)
    new_root_node := get_part_dirtree_by_depth(root_node, uint(max_depth))
    s := dumps_dirtree_json(new_root_node)
    fmt.Fprintf(w, s)
}


func main() {

    start_server()

}





