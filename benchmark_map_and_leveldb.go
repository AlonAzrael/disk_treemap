package main


import (
    // "fmt"
    "time"
    "log"
    "math/rand"
    "math"
    "github.com/syndtr/goleveldb/leveldb/memdb"
    "encoding/binary"
    "github.com/syndtr/goleveldb/leveldb/comparer"
    // "bytes"
)

var max_records = 100000

func if_error(e error) {
    if e != nil {
        panic(e)
    }
}

func SB(s string) []byte{
    return []byte(s)
}
func BS(b []byte) string{
    return string(b)
}


const letterBytes = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
const (
    letterIdxBits = 6                    // 6 bits to represent a letter index
    letterIdxMask = 1<<letterIdxBits - 1 // All 1-bits, as many as letterIdxBits
    letterIdxMax  = 63 / letterIdxBits   // # of letter indices fitting in 63 bits
)

func RandStringBytesMaskImpr(n int) []byte {
    b := make([]byte, n)
    // A rand.Int63() generates 63 random bits, enough for letterIdxMax letters!
    for i, cache, remain := n-1, rand.Int63(), letterIdxMax; i >= 0; {
        if remain == 0 {
            cache, remain = rand.Int63(), letterIdxMax
        }
        if idx := int(cache & letterIdxMask); idx < len(letterBytes) {
            b[i] = letterBytes[idx]
            i--
        }
        cache >>= letterIdxBits
        remain--
    }

    return b
}

func rand_bytes_arr(n_element int, bytes_length int) [][]byte{
    bytes_arr := make([][]byte, n_element)
    for i := 0; i < n_element; i++ {
        bytes_arr[i] = RandStringBytesMaskImpr(bytes_length)
    }

    return bytes_arr
}


func test_leveldb(keys_arr [][]byte, vals_arr [][]byte) {
    var default_comparer = comparer.DefaultComparer
    db := memdb.New(default_comparer, max_records)

    // batch := new(leveldb.Batch)
    for i := 0; i < len(keys_arr); i++ {
        db.Put(keys_arr[i], vals_arr[i])
    }

    // read
    l := len(keys_arr)
    var temp []byte
    for i := 0; i < l; i++ {
        index := rand.Intn(l)
        key := keys_arr[index]
        temp, _ = db.Get(key)
        _ = temp
    }
}

func test_gomap(keys_arr []string, vals_arr []float64) {
    var gomap = make(map[string]float64)
    for i := 0; i < len(keys_arr); i++ {
        gomap[keys_arr[i]] = vals_arr[i]
    }

    // read
    l := len(keys_arr)
    var temp float64
    for i := 0; i < l; i++ {
        index := rand.Intn(l)
        key := keys_arr[index]
        temp = gomap[key]
        _ = temp
    }
}


func Float64bytes(float float64) []byte {
    bits := math.Float64bits(float)
    bytes := make([]byte, 8)
    binary.LittleEndian.PutUint64(bytes, bits)
    return bytes
}

func main() {

    keys_arr := rand_bytes_arr(100000*1, 50)
    vals_arr := make([]float64, len(keys_arr))
    vals_bytes_arr := make([][]byte, len(keys_arr))
    keys_str_arr := make([]string, len(keys_arr))
    for i := 0; i < len(vals_arr); i++ {
        val := rand.Float64()
        vals_arr[i] = val
        val_bytes := Float64bytes(val)
        vals_bytes_arr[i] = val_bytes
        keys_str_arr[i] = string(val_bytes)
    }

    start := time.Now()
    // code block

    // test_leveldb(keys_arr, vals_bytes_arr)
    test_gomap(keys_str_arr, vals_arr)

    // code block
    elapsed := time.Since(start)
    log.Printf("elapsed time: %.16f", elapsed.Seconds())
}



