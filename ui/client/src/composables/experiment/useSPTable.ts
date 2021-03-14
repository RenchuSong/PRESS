import { Store } from "@/store";
import { MutationTypes, ActionTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useSPTable(store: Store) {
  const currentSPTableBinaries = computed(
    () => store.getters.currentExperimentSPTableBinaries
  );

  const buildSPTable = async (distance: number) => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.BUILD_SP_TABLE,
      text: "Building shortest path table"
    });
    await store.dispatch(
      ActionTypes.BUILD_SP_TABLE,
      { distance }
    );
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.BUILD_SP_TABLE
    });
  };

  const dumpSPTableToBinary = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.DUMP_SP_TABLE_TO_BINARY,
      text: "Dumping shortest path table to binary"
    });
    await store.dispatch(ActionTypes.DUMP_SP_TABLE_TO_BINARY);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.DUMP_SP_TABLE_TO_BINARY
    });
  }

  const loadSPTableFromBinary = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.LOAD_SP_TABLE_FROM_BINARY,
      text: "Loading shortest path table from binary"
    });
    await store.dispatch(ActionTypes.LOAD_SP_TABLE_FROM_BINARY);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.LOAD_SP_TABLE_FROM_BINARY
    });
  };

  return {
    currentSPTableBinaries,
    buildSPTable,
    dumpSPTableToBinary,
    loadSPTableFromBinary,
  };
}
