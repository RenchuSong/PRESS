// Based on work by A. Svensson
// Source: https://github.com/lmas/gin-sse/blob/master/sse_handler.go

package svc

import (
	"encoding/json"
	"fmt"
	"net/http"

	"github.com/RenchuSong/PRESS/tree/v3/ui/server/util"
	"github.com/gin-gonic/gin"
	log "github.com/sirupsen/logrus"
)

type sseClient struct {
	id      string
	channel chan string
}

// SSEHandler is the server-sent-events handler.
type SSEHandler struct {
	clients        map[string]chan string
	newClients     chan sseClient
	defunctClients chan string
	messages       chan string
}

// NewSSEHandler creates a new SSE handler.
func NewSSEHandler() *SSEHandler {
	return &SSEHandler{
		clients:        make(map[string]chan string),
		newClients:     make(chan sseClient),
		defunctClients: make(chan string),
		messages:       make(chan string),
	}
}

// Send out a JSON string object to all clients.
func (b *SSEHandler) Send(obj *util.TaskResult) {
	tmp, err := json.Marshal(obj)
	if err != nil {
		log.Info("Error while sending JSON object:", err)
	} else {
		b.messages <- string(tmp)
	}
}

// Subscribe a new client.
func (b *SSEHandler) Subscribe(c *gin.Context) {
	w := c.Writer
	f, ok := w.(http.Flusher)
	if !ok {
		c.AbortWithError(http.StatusBadRequest, fmt.Errorf("Streaming unsupported"))
		return
	}

	cli := sseClient{
		id:      util.NewUUID(),
		channel: make(chan string),
	}
	b.newClients <- cli

	notify := w.(http.CloseNotifier).CloseNotify()
	go func() {
		<-notify
		b.defunctClients <- cli.id
	}()

	w.Header().Set("Content-Type", "text/event-stream")
	w.Header().Set("Cache-Control", "no-cache")
	w.Header().Set("Connection", "keep-alive")
	// TODO: Remove for production.
	w.Header().Set("access-control-allow-origin", "*")

	for {
		msg, ok := <-cli.channel
		if !ok {
			break
		}
		fmt.Fprintf(w, "event: response\ndata: %s\n\n", msg)
		f.Flush()
		log.Debugf("Server event sent to client %s: %s", cli.id, msg)
	}
}

// Run spawns SSE handler.
func (b *SSEHandler) Run() {
	for {
		select {
		// New clients added.
		case c := <-b.newClients:
			b.clients[c.id] = c.channel
			log.Debug("Client connected: ", c.id)
		// Disconnect clients.
		case s := <-b.defunctClients:
			if ch, ok := b.clients[s]; ok {
				close(ch)
				delete(b.clients, s)
				log.Debug("Client disconnected: ", s)
			} else {
				log.Warn("Failed to disconnect client:", s)
			}
		// Broadcast events.
		case msg := <-b.messages:
			for _, ch := range b.clients {
				ch <- msg
			}
		}
	}
}
