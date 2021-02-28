import { Store } from "@/store";
import { MutationTypes, ActionTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useGridIndex(store: Store) {
  const currentGridIndexBinaries = computed(
    () => store.getters.currentExperimentGridIndexBinaries
  );

  const buildGridIndex = async (width: number, height: number) => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.BUILD_GRID_INDEX,
      text: "Building grid index"
    });
    await store.dispatch(
      ActionTypes.BUILD_GRID_INDEX,
      { width, height }
    );
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.BUILD_GRID_INDEX
    });
  };

  const dumpGridIndexToBinary = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.DUMP_GRID_INDEX_TO_BINARY,
      text: "Dumping grid index to binary"
    });
    await store.dispatch(ActionTypes.DUMP_GRID_INDEX_TO_BINARY);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.DUMP_GRID_INDEX_TO_BINARY
    });
  }

  const loadGridIndexFromBinary = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.LOAD_GRID_INDEX_FROM_BINARY,
      text: "Loading grid index from binary"
    });
    await store.dispatch(ActionTypes.LOAD_GRID_INDEX_FROM_BINARY);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.LOAD_GRID_INDEX_FROM_BINARY
    });
  };

  return {
    currentGridIndexBinaries,
    buildGridIndex,
    dumpGridIndexToBinary,
    loadGridIndexFromBinary,
  };
}
