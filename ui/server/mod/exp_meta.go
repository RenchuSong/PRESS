package mod

// ExperimentMeta is the experiment meta data.
type ExperimentMeta struct {
	ID           int    `json:"id"`
	Name         string `json:"name"`
	CreationTime int    `json:"creationTime"`
	Image        string `json:"image"`
}
