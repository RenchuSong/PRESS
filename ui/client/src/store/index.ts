import { InjectionKey } from "vue";
import {
  createStore,
  useStore as baseUseStore,
  Store as VuexStore,
  createLogger,
  CommitOptions,
  DispatchOptions
} from "vuex";
import common from "./modules/common";
import experiment from "./modules/experiment";
import experiments from "./modules/experiments";
import {
  RootState,
  Mutations,
  Actions,
  Getters,
  RootStoreState
} from "./store-types";

const debug = process.env.NODE_ENV !== "production";

export type Store = Omit<
  VuexStore<RootState>,
  "getters" | "commit" | "dispatch"
> & {
  commit<K extends keyof Mutations, P extends Parameters<Mutations[K]>[1]>(
    key: K,
    payload: P,
    options?: CommitOptions
  ): ReturnType<Mutations[K]>;
} & {
  dispatch<K extends keyof Actions>(
    key: K,
    payload?: Parameters<Actions[K]>[1],
    options?: DispatchOptions
  ): ReturnType<Actions[K]>;
} & {
  getters: {
    [K in keyof Getters]: ReturnType<Getters[K]>;
  };
};

export const store = createStore<RootStoreState>({
  modules: {
    common,
    experiments,
    experiment
  },
  strict: debug,
  plugins: debug ? [createLogger()] : []
});

export const key: InjectionKey<VuexStore<RootState>> = Symbol();

export function useStore() {
  return baseUseStore(key) as Store;
}
