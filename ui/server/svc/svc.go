package svc

import (
	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
)

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
	coreTskQueue  *util.TaskQueue
	otherTskQueue *util.TaskQueue
	coreExes      []*Exe
	otherExes     []*Exe
}

// NewService creates a new service.
func NewService(c Config) *Service {
	s := NewSSEHandler()
	cq := util.NewTaskQueue("core", 50)
	oq := util.NewTaskQueue("other", 1024)
	ce := make([]*Exe, 0, c.APIHandlers)
	for i := 0; i < c.APIHandlers; i++ {
		ce = append(ce, NewExe(cq, s))
	}
	oe := make([]*Exe, 0, 4)
	for i := 0; i < 4; i++ {
		oe = append(oe, NewExe(oq, s))
	}

	return &Service{
		http:          NewHTTP(s, cq, oq, &c),
		sse:           s,
		config:        c,
		coreTskQueue:  cq,
		otherTskQueue: oq,
		coreExes:      ce,
		otherExes:     oe,
	}
}

// Run spawns the service.
func (s *Service) Run() {
	setLogger(&s.config)

	go s.sse.Run()

	for _, e := range s.coreExes {
		go e.Run()
	}
	for _, e := range s.otherExes {
		go e.Run()
	}

	s.http.Run()
}
