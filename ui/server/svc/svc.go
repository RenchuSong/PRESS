package svc

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
	config Config
	http   *HTTP
	sse    *SSEHandler
}

// NewService creates a new service.
func NewService(c Config) *Service {
	s := NewSSEHandler()

	return &Service{
		http:   NewHTTP(s, &c),
		sse:    s,
		config: c,
	}
}

// Run spawns the service.
func (s *Service) Run() {
	setLogger(&s.config)

	go s.sse.Run()
	s.http.Run()
}
