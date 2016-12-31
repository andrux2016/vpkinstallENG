package main

import (
	"fmt"
	"net/http"
	"io/ioutil"
	"strings"
)

var(
	port string
	ip string
)

func init(){
	port = "8080" //Sets port
	ip = "192.168.2.2" //Sets ip
}

func handler(w http.ResponseWriter, r *http.Request) {

	files, _ := ioutil.ReadDir("./vpk/")//Reads out file in vpk dir
	for _, f := range files {//for each in array made above
	    fmt.Fprintf(w, "<a href='vdi:install?"+ip+":"+port+"/download?file="+strings.Replace(f.Name(),".vpk","",-1)+"'>Download "+strings.Replace(f.Name(),".vpk","",-1)+"</a><br/>")//Where f.Name is a file name it creates links for the vita and also removes vpk extension
	}

}

func downloadhandler(w http.ResponseWriter, r *http.Request) {
	file := r.URL.Query().Get("file")//gets url get of "file"
	http.ServeFile(w,r,"./vpk/"+strings.Replace(file,".","",-1)+".vpk")//Very basic protection 
}

func main() {
	http.HandleFunc("/", handler)
	http.HandleFunc("/download", downloadhandler)
	http.ListenAndServe(ip+":"+port, nil)
}
