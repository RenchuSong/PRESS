package svc

import (
	"os"

	log "github.com/sirupsen/logrus"
)

func setLogger(c *Config) {
	// Timestamp format.
	log.SetFormatter(&log.TextFormatter{
		FullTimestamp: true,
	})
	// Log level.
	switch c.LogLevel {
	case "ERROR":
		log.SetLevel(log.ErrorLevel)
	case "WARN":
		log.SetLevel(log.WarnLevel)
	case "INFO":
		log.SetLevel(log.InfoLevel)
	case "DEBUG":
		log.SetLevel(log.DebugLevel)
	default:
		log.SetLevel(log.InfoLevel)
	}
	// Log path.
	f, err := os.OpenFile(
		c.Logs+"access.log",
		os.O_APPEND|os.O_CREATE|os.O_WRONLY,
		0644,
	)
	if err != nil {
		log.Fatal("Failed to open log file, ", err)
	}
	log.SetOutput(f)
}
