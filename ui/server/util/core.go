package util

import (
	"encoding/json"
	"io/ioutil"
	"path"
)

// CoreResponse is the response from core.
type CoreResponse struct {
	Success bool   `binding:"required"`
	Message string `binding:"required"`
	Data    interface{}
}

// CoreCommunicator talks with core.
type CoreCommunicator struct {
	req  string
	resp string
}

// Core communicator.
var Core CoreCommunicator

// NewCore setup the core communicator.
func NewCore(pipe string) {
	Core = CoreCommunicator{
		req:  path.Join(pipe, "PRESS_CORE_IN"),
		resp: path.Join(pipe, "PRESS_CORE_OUT"),
	}
}

// SendRequest to core.
func (c *CoreCommunicator) SendRequest(b interface{}) error {
	p, err := json.Marshal(b)
	if err != nil {
		return err
	}
	return ioutil.WriteFile(c.req, p, 0755)
}

// GetResponse from core.
func (c *CoreCommunicator) GetResponse() (*CoreResponse, error) {
	s, err := ioutil.ReadFile(c.resp)
	if err != nil {
		return nil, err
	}
	var d CoreResponse
	if err := json.Unmarshal(s, &d); err != nil {
		return nil, err
	}
	return &d, nil
}
