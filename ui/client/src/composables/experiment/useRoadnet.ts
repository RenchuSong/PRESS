import { Store } from "@/store";
import { ActionTypes, MutationTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useRoadnet(store: Store) {
  const loadRoadnetFromFile = async (folder: string, graphReaderType: string) => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.LOAD_ROADNET_FROM_FILE,
      text: "Loading roadnet from file"
    });
    await store.dispatch(ActionTypes.LOAD_ROADNET_FROM_FILE, { folder: folder, graphReaderType: graphReaderType });
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.LOAD_ROADNET_FROM_FILE
    });
  };

  return {
    loadRoadnetFromFile
  };
}
