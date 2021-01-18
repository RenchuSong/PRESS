package main

import (
	"flag"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/svc"
)

func main() {
	var confPath string
	flag.StringVar(&confPath, "config", "./config.json", "the config file path")
	flag.Parse()

	c := readConfig(confPath)

	service := svc.NewService(svc.Config{
		Experiments: c.Experiments,
		Data:        c.Data,
		Logs:        c.AppServer.Logs,
		LogLevel:    c.AppServer.LogLevel,
		Port:        c.AppServer.Port,
		Static:      c.AppServer.Static,
		APIHandlers: c.AppServer.APIHandlers,
	})
	service.Run()
}
