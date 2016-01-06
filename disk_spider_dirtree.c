

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ftw.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>

#include <search.h>
#include <glib.h>

// #include <sparsehash/dense_hash_map>
// #include <sparsehash/dense_hash_set>

// using google::dense_hash_map;
// using google::dense_hash_set;


#define DIRENT struct dirent
#define DIRENT_P struct dirent *
#define DIR_P DIR *
#define STAT struct stat 
#define STAT_P struct stat * 
// #define TRUE 1
// #define FALSE 0
#define UINT unsigned int
#define ULONG unsigned long 
#define BOOL unsigned short

#define GHashTable_P GHashTable *
#define GArray_P GArray *

#define DT_NODE struct DirTreeNode
#define DT_NODE_P struct DirTreeNode * 
#define DT_NODE_ARR_P GArray_P
#define DT_NODE_SIZE ULONG
#define DT_HASH_P int *

#define DT_LINUX_HASH 0
#define DT_GLIB_HASH 1
#define DT_HASH_MODE DT_LINUX_HASH

#define DT_MAX_NODE 100000




struct DirTreeNode {
    char* node_name;
    char* node_path;
    DT_NODE_SIZE node_size;
    // UINT node_type;
    BOOL is_dir_flag;
    
    DT_NODE_ARR_P children;
    // UINT n_children;

    // DT_NODE_P parent;
    UINT node_index;
};

static DT_NODE_ARR_P dt_node_arr;
static GArray_P dt_tree_json;

void init_dt_node_arr(){
    dt_node_arr = g_array_new (FALSE, FALSE, sizeof (DT_NODE_P)) ;
}
void append_dt_node_arr_element(DT_NODE_P node){
    // printf("append ok\n");
    g_array_append_val(dt_node_arr, node);
}
void print_dt_node_arr(){
    DT_NODE_P node;
    for (UINT i = 0; i < dt_node_arr->len; ++i)
    {
        node = g_array_index (dt_node_arr, DT_NODE_P, i);
        printf("%s %ld\n", node->node_path, node->node_size);
    }
}
DT_NODE_P get_dt_node_arr_element(UINT index){
    return g_array_index(dt_node_arr, DT_NODE_P, index);
}

DT_NODE_P create_dt_node(char* path, char* name, UINT size, BOOL is_dir_flag){

    DT_NODE_P new_node = (DT_NODE_P) malloc(1*sizeof(DT_NODE));
    new_node->node_name = strdup(name);
    new_node->node_path = strdup(path);
    new_node->node_size = size;
    new_node->is_dir_flag = is_dir_flag;

    if (is_dir_flag)
    {
        new_node->children = g_array_new (FALSE, FALSE, sizeof (DT_NODE_P));
    }
    else 
    {
        new_node->children = NULL;
    }

    append_dt_node_arr_element(new_node);

    return new_node;
}
void append_dt_node_child(DT_NODE_P parent, DT_NODE_P child){
    g_array_append_val(parent->children, child);
}
DT_NODE_P get_dt_node_child(DT_NODE_ARR_P children, UINT index){
    return g_array_index(children, DT_NODE_P, index);
}

void init_dt_tree_json(){
    dt_tree_json = g_array_new (FALSE, FALSE, sizeof (char*)) ;
}
void append_dt_tree_json_str(char* new_str){

}
void print_dt_tree_json(){
    for (UINT i = 0; i < dt_tree_json->len; ++i){
        char* new_str = g_array_index( dt_tree_json, char* , i);
        printf("%s", new_str);
    }
}
void write_dt_tree_json(char* filepath){
    FILE *fp;
    fp = fopen(filepath, "w");
    if (fp)
    {
        for (UINT i = 0; i < dt_tree_json->len; ++i){
            char* new_str = g_array_index( dt_tree_json, char* , i);
            fprintf(fp, "%s", new_str);
        }
    }
    else 
    {
        printf("%s is fucked\n", filepath);
    }
    fclose(fp);
}


// GHashTable_P dt_hash;



static unsigned int total = 0;

char* path_join(char* first, char* second) {
    char * s = NULL;
    asprintf(&s, "%s/%s", first, second);
    return s;
}

void path_split( char *pf, char** p, char** f) {
    // char *slash = pf, *next;
    // while ((next = strpbrk(slash + 1, "\\/"))) slash = next;
    // if (pf != slash) slash++;
    // *p = strndup(pf, slash - pf);
    // *f = strdup(slash);

    char* dirc = strdup(pf);
    char* basec = strdup(pf);

    *p = dirname(dirc);
    *f = basename(basec);
    // printf("%s\n", *p);
}


void init_(char* dirpath){
    if (DT_HASH_MODE == DT_LINUX_HASH)
    {
        hcreate(DT_MAX_NODE);
        init_dt_node_arr();
        init_dt_tree_json();

        ENTRY new_entry;
        char* name, *parent_path;
        path_split(dirpath, &parent_path, &name);
        new_entry.key = parent_path;
        DT_NODE_P root_node = create_dt_node(parent_path, name, 2, TRUE);
        new_entry.data = (void *) root_node;
        hsearch(new_entry, ENTER);

        // printf("ENTER path: %s\n", parent_path);
        
        // append_dt_node_arr_element(root_node);
    } 
    else if (DT_HASH_MODE == DT_GLIB_HASH){
        // dt_hash =  g_hash_table_new(g_str_hash,g_str_equal);
    }

}

void callback_(const char* _fpath, UINT size, BOOL is_dir_flag){
    char* fpath = strdup(_fpath);
    // printf("%s\n", fpath);
    if (DT_HASH_MODE == DT_LINUX_HASH)
    {
        ENTRY new_entry, *ep;
        new_entry.key = fpath;
        char* name, *parent_path;
        path_split(fpath, &parent_path, &name);
        DT_NODE_P new_node = create_dt_node(fpath, name, size, is_dir_flag);
        new_entry.data = (void *) new_node;
        ep = hsearch(new_entry, ENTER);
        if (ep)
        {
            // printf("ENTER path: %s\n", fpath);
        }
        else 
        {
            printf("CANT ENTER path: %s\n", fpath);
        }

        // append to parent
        ENTRY parent_entry;
        parent_entry.key = parent_path;
        ep = hsearch(parent_entry, FIND);
        if (ep)
        {
            // printf("FIND path: %s\n", parent_path);
            DT_NODE_P parent = (DT_NODE_P)ep->data;
            append_dt_node_child(parent, new_node);
        }
        else 
        {
            printf("CANT FIND path: %s\n", parent_path);
        }
        
    }
    else if (DT_HASH_MODE == DT_GLIB_HASH){

    }
}


////////////////////////////////////////


int sum(const char *fpath, const struct stat *sb, int typeflag) {
    // printf("%s", fpath);
    if (typeflag != FTW_NS)
    {
        total += sb->st_size;
    }
    return 0;
}

int sum_nftw(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    // printf("%s\n", fpath);

    // if (typeflag == FTW_NS)
    // {
    //     return FTW_SKIP_SIBLINGS;
    // }
    // else {
    //     total += sb->st_size;
    //     return FTW_CONTINUE;
    // }

    UINT st_size = sb->st_size;
    callback_(fpath, st_size, S_ISDIR(sb->st_mode));

    if (typeflag==FTW_SL)
    {
        // printf("symbol link\n");
        // return 1;
    }
    else {
        total += st_size;
        return 0;
    }

    return 0;
}



DT_NODE_SIZE calc_dt_node_real_size(DT_NODE_P node){
    /*
    this is actually the recursive dt_tree-traverse-walk, so try to do all traverse work here.
    For example, dump dt_tree to json.
    */
    
    char* header_s = "{\"children\":[";
    // asprintf(&header_s, "{ \"name\":\"%s\", \"size\":%ld, \"children\":[ ", node->node_name, node->node_size);
    g_array_append_val(dt_tree_json, header_s);
    
    // printf("OK\n");


    DT_NODE_ARR_P children = node->children;
    DT_NODE_P child;
    for (int i = 0; i < children->len; ++i)
    {
        child = get_dt_node_child(children, i);
        if (child->is_dir_flag)
        {
            node->node_size += calc_dt_node_real_size(child);
        }
        else
        {
            node->node_size += child->node_size;
            
            char* file_s = NULL;
            asprintf(&file_s, "{\"name\":\"%s\",\"size\":%ld}", child->node_name, child->node_size);
            // ,\"Children\":[] ,\"Flag\":%d
            g_array_append_val(dt_tree_json, file_s);

        }
        
        // detect last one
        if (i!=children->len-1)
        {
            char* comma_s = ",";
            g_array_append_val(dt_tree_json, comma_s);
        }
            
    }
    
    char* footer_s = NULL;
    asprintf(&footer_s, "],\"name\":\"%s\",\"size\":%ld}", node->node_name, node->node_size);
    g_array_append_val(dt_tree_json, footer_s);

    return node->node_size;
}

void calc_dt_tree_real_size(){
    UINT index = 0;
    DT_NODE_P root_node = get_dt_node_arr_element(index);
    DT_NODE_P first_node = get_dt_node_child(root_node->children, index);
    first_node->node_size = calc_dt_node_real_size(first_node);
}

int breath_first_ftw(char* dirpath) {

    char* path_arr[200000];

    UINT path_arr_length = 0;
    UINT cur_index = 0;
    
    path_arr[cur_index] = dirpath;
    path_arr_length +=1 ;

    char* cur_path;
    ULONG total_size = 0;

    // printf("init ok\n");

    // calc dirpath size
    STAT temp_stat;
    if(stat(dirpath, &temp_stat) == 0){
        total_size += temp_stat.st_size;
        // printf("%s\n", dirpath);
    }

    DIR_P d;
    DIRENT_P entry;
    while (TRUE) {

        if (cur_index < path_arr_length)
        {
            cur_path = path_arr[cur_index];
            // printf("%s\n", cur_path);
            path_arr[cur_index] = NULL; // free 
            cur_index += 1;
        }
        else 
        {
            break;
        }

        d = opendir(cur_path);
        if (!d)
        {
            printf("%s error\n", cur_path);
            return 1;
        }

        while ((entry = readdir(d)) != NULL){

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            char* new_path = path_join(cur_path, entry->d_name);
            STAT new_stat;
            if(stat(new_path, &new_stat) == 0){
                total_size += new_stat.st_size;
                // printf("%s\n", new_path);
            }

            if (entry->d_type == DT_DIR) // this is a dir
            {
                // append new_path 
                path_arr[path_arr_length] = new_path;
                path_arr_length += 1;
            }

        }
        closedir(d);
    }

    return total_size;
}

int test_ftw_and_nftw(char* filepath) {

    if (nftw(filepath, &sum_nftw, 1, FTW_PHYS)) {
        perror("ftw");
        return 2;
    }

    // if (ftw(filepath, &sum, 1)) {
    //     perror("ftw");
    //     return 2;
    // }

    return total;
}


int main(int argc, char **argv) {
    char * filepath;
    if (argv[1])
    {
        filepath = argv[1];
    }
    else {
        // filepath = "/Users/aaronyin/anaconda";
        // filepath = "/Users/aaronyin/AaronYinProject";
        filepath = "/Users/aaronyin/AaronYinProject/DiskSpider_Demo/test_dir_good" ;
    }

    if (!filepath || access(filepath, R_OK)) {
        return 1;
    }

    init_(filepath);

    total = test_ftw_and_nftw(filepath);

    // total = breath_first_ftw(filepath);

    // char* dir_path , *base_path ;
    // path_split(filepath,&dir_path,&base_path);
    // printf("%s %s\n", dir_path, base_path);

    // printf("%s: %u\n", filepath, total);
    calc_dt_tree_real_size();
    // print_dt_node_arr();
    // print_dt_tree_json();
    char* dt_tree_json_filepath = "/Users/aaronyin/AaronYinProject/DiskSpider_Demo/dir_tree/ds_c.json_";
    write_dt_tree_json(dt_tree_json_filepath);
    printf("%s", dt_tree_json_filepath);
    return 0;
}

/* compile cmd
gcc disk_spider_dirtree.c -o disk_spider_dirtree_c `pkg-config --cflags --libs glib-2.0`
*/


