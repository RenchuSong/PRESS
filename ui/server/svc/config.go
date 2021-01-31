package svc

// Config for service.
type Config struct {
	Experiments      string
	Data             string
	Logs             string
	LogLevel         string
	Pipe             string
	Port             int
	Static           string
	CoreAPIHandlers  int
	OtherAPIHandlers int
}
