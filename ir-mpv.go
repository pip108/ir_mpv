package main

import (
	"fmt"
	"log"
	"net"
)

const VOL_DOWN = 17
const VOL_UP = 16
const MUTE = 13
const SUBTITLES = 60
const PLAY_PAUSE = 92
const SCREENSHOT = 109
const SEEK_FORWARD = 91
const SEEK_BACKWARD = 90

const (
	proto            = "unix"
	addr             = "/tmp/mpv-socket"
	mpv_input_format = "{\"command\":[\"keypress\",\"%s\"]}\n"
)

var commands = map[int]string{
	92:  " ",
	109: "F5",
	90:  "left",
	91:  "right",
	13:  "m",
	60:  "s",
	17:  "9",
	16:  "0",
}

func ipc_command(cmd_json string) {
	conn, err := net.Dial(proto, addr)
	if err != nil {
		println(err.Error())
		return
	}
	conn.Write([]byte(cmd_json))
	conn.Close()
}

func make_cmd(code int) string {
	cmd := fmt.Sprintf(mpv_input_format, commands[code])
	println(cmd)
	return cmd
}

func main() {
	fd, err := net.ListenPacket("udp", ":30108")
	if err != nil {
		log.Fatal(err.Error())
	}
	defer fd.Close()
	for {
		buf := make([]byte, 1024)
		n, _, err := fd.ReadFrom(buf)
		if err != nil || n < 1 {
			continue
		}
		cmd := make_cmd(int(buf[0]))
		ipc_command(cmd)
	}
}
