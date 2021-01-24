import { GetterTree, MutationTree } from "vuex";
import { CommonGetters, CommonMutations, CommonMutationTypes, CommonState } from "./common-types";

const state = () => ({ loadingJobs: [] } as CommonState);

const getters: GetterTree<CommonState, CommonState> & CommonGetters = {
  currentLoadingJobs: state =>
    state.loadingJobs.map(job => `${job.text}... `).join(),
  isLoading: state => state.loadingJobs.length > 0
};

const mutations: MutationTree<CommonState> & CommonMutations = {
  [CommonMutationTypes.START_JOB](
    state: CommonState,
    payload: { id: string, text: string }
  ) {
    state.loadingJobs.push(payload);
  },
  [CommonMutationTypes.FINISH_JOB](
    state: CommonState,
    payload: { id: string }
  ) {
    state.loadingJobs = state.loadingJobs.filter(job => job.id !== payload.id)
  }
};

export default {
  state,
  getters,
  mutations
};
