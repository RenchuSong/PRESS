package main

import (
	"flag"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/svc"
)

func main() {
	var confPath string
	var pipePath string
	flag.StringVar(&confPath, "config", "./config.json", "the config file path")
	flag.StringVar(&pipePath, "pipe", "./", "the folder for pipe files")
	flag.Parse()

	c := readConfig(confPath)

	service := svc.NewService(svc.Config{
		Experiments:      c.Experiments,
		Data:             c.Data,
		Logs:             c.AppServer.Logs,
		LogLevel:         c.AppServer.LogLevel,
		Pipe:             pipePath,
		Port:             c.AppServer.Port,
		Static:           c.AppServer.Static,
		CoreAPIHandlers:  c.AppServer.CoreAPIHandlers,
		OtherAPIHandlers: c.AppServer.OtherAPIHandlers,
	})
	service.Run()
}
