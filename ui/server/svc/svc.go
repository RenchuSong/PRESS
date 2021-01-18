package svc

import "github.com/RenchuSong/PRESS/tree/v3/ui/server/exe"

// Config for service.
type Config struct {
	Experiments string
	Data        string
	Logs        string
	LogLevel    string
	Port        int
	Static      string
	APIHandlers int
}

// Service for App.
type Service struct {
	config        Config
	http          *HTTP
	sse           *SSEHandler
	coreTskQueue  *exe.TaskQueue
	otherTskQueue *exe.TaskQueue
}

// NewService creates a new service.
func NewService(c Config) *Service {
	s := NewSSEHandler()
	cq := exe.NewTaskQueue("core", 50)
	oq := exe.NewTaskQueue("other", 1024)

	return &Service{
		http:          NewHTTP(s, cq, oq, &c),
		sse:           s,
		config:        c,
		coreTskQueue:  cq,
		otherTskQueue: oq,
	}
}

// Run spawns the service.
func (s *Service) Run() {
	setLogger(&s.config)

	go s.sse.Run()
	s.http.Run()
}
