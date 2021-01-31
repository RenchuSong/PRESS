import { Store } from "@/store";
import { ActionTypes, MutationTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useRoadnet(store: Store) {
  const loadRoadnetFromFile = async (folder: string, graphReaderType: string) => {
    console.log('haha');
    // store.commit(MutationTypes.START_JOB, {
    //   id: ActionTypes.,
    //   text: "Opening experiment"
    // });
    // await store.dispatch(ActionTypes.OPEN_EXPERIMENT, { id });
    // store.commit(MutationTypes.FINISH_JOB, {
    //   id: ActionTypes.OPEN_EXPERIMENT
    // });
  };

  return {
    loadRoadnetFromFile
  };
}
