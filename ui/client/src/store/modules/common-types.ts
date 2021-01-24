// state
export interface CommonState {
  loadingJobs: Array<{ id: string, text: string }>;
}

// mutations
export enum CommonMutationTypes {
  START_JOB = "START_JOB",
  FINISH_JOB = "FINISH_JOB"
}

export type CommonMutations<S = CommonState> = {
  [CommonMutationTypes.START_JOB](
    state: S,
    payload: { id: string, text: string }
  ): void;
  [CommonMutationTypes.FINISH_JOB](
    state: S,
    payload: { id: string }
  ): void;
};

// getters
export type CommonGetters = {
  currentLoadingJobs(state: CommonState): string;
  isLoading(state: CommonState): boolean;
};
