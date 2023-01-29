> go env -w GO111MODULE=auto
> go build -race -buildmode=plugin ../mrapps/wc.go
> go run mrsequential.go wc.so pg*.txt   (build with race .. run with race.. & vice versa)
> go run -race mrsequential.go wc.so pg*.txt

## Notes

### pg*.txt

pg*.txt fetches all files only in linux. This does not work out in windows.
```go
go run mrtest.go wc.so pg*.txt
```
## Go lang concepts



### Command line args

```go
> go run mrsequential.go wc.so pg*.txt

func main() {
	if len(os.Args) < 3 {
		fmt.Fprint(os.Stderr, "Usage: mrsequential xxx.so inputfiles ...\n")
		os.Exit(1)
	}
}
```

### Files

```go
file, err := os.Open(filename)
if err != nil {
    log.Fatalf("Can not open %v", filename)
}

content, err := ioutil.ReadAll(file)
if err != nil {
    log.Fatalf("Can not read %v", filename)
}
file.Close()


oname := "mr-out-0"
ofile, _ := os.Create(oname)
fmt.Fprintf(ofile, "%v %v\n", intermediate[i].Key, output)
ofile.Close()
```

### String split using custom function
```go
// function to dete t word separators.
ff := func(r rune) bool { return !unicode.IsLetter(r) }

// split contents in to an array of words
words := strings.FieldsFunc(contents, ff)
```

### Working with custom types array
```go
// In mr package
type KeyValue struct {
	Key   string
	Value string
}

// In main
kva := []mr.KeyValue{}
for _, w := range words {
    kv := mr.KeyValue{w, "1"}
    kva = append(kva, kv)
}
```

### String to Integer & vice versa

```go
val := 1
valstr := strconv.Itoa(val)
```

### Sorting 

```go
// In mr package
type KeyValue struct {
	Key   string
	Value string
}

// In main package
type ByKey []mr.KeyValue

func (a ByKey) Len() int           { return len(a) }
func (a ByKey) Swap(i, j int)      { a[i], a[j] = a[j], a[i] }
func (a ByKey) Less(i, j int) bool { return a[i].Key < a[j].Key }

intermediate := []mr.KeyValue{}
sort.Sort(ByKey(intermediate))
```

### Working with plugins

```go
import "plugin"
import "log"

mapf, reducef := loadPlugin("wc.so")

func loadPlugin(filename string) (func(string, string) []mr.KeyValue, func(string, []string) string) {
	p, err := plugin.Open(filename)
	if err != nil {
		log.Fatalf("cannot load plugin %v", filename)
	}
	xmapf, err := p.Lookup("Map")
	if err != nil {
		log.Fatalf("cannot find Map in %v", filename)
	}
	mapf := xmapf.(func(string, string) []mr.KeyValue)
	xreducef, err := p.Lookup("Reduce")
	if err != nil {
		log.Fatalf("cannot find Reduce in %v", filename)
	}
	reducef := xreducef.(func(string, []string) string)

	return mapf, reducef
}
```