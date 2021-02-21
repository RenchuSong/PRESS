import { Roadnet } from "@/api/roadnet";
import { createRoadnet, RoadnetWithBound } from "@/model/roadnet";
import { Store } from "@/store";
import { ActionTypes, MutationTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useRoadnet(store: Store) {
  const roadnetReaderTypes = computed(() => store.getters.roadnetReaderTypes);
  const roadnetDataSources = computed(() => store.getters.roadnetDataSources);
  const currentRoadnetBinaries = computed(
    () => store.getters.currentExperimentRoadnetBinaries
  );

  const initRoadnet = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.INIT_ROADNET_OPTIONS,
      text: "Initializing roadnet options",
    });
    await store.dispatch(ActionTypes.INIT_ROADNET_OPTIONS, {});
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.INIT_ROADNET_OPTIONS,
    });
  }

  const loadRoadnetFromFile = async (filename: string, graphReaderType: string) => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.LOAD_ROADNET_FROM_FILE,
      text: "Loading roadnet from file"
    });
    await store.dispatch(
      ActionTypes.LOAD_ROADNET_FROM_FILE,
      { folder: filename.split('/')[0], graphReaderType: graphReaderType }
    );
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.LOAD_ROADNET_FROM_FILE
    });
  };

  const dumpRoadnetToBinary = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.DUMP_ROADNET_TO_BINARY,
      text: "Dumping roadnet to binary"
    });
    await store.dispatch(ActionTypes.DUMP_ROADNET_TO_BINARY);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.DUMP_ROADNET_TO_BINARY
    });
  }

  const loadRoadnetFromBinary = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.LOAD_ROADNET_FROM_BINARY,
      text: "Loading roadnet from binary"
    });
    await store.dispatch(ActionTypes.LOAD_ROADNET_FROM_BINARY);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.LOAD_ROADNET_FROM_BINARY
    });
  };

  const getRoadnet = async (): Promise<RoadnetWithBound> => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.UPDATE_ROADNET,
      text: "Loading roadnet data to UI portal"
    });
    const roadnet = await Roadnet.genRoadnet();
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.UPDATE_ROADNET
    });
    return createRoadnet(roadnet);
  };

  return {
    initRoadnet,
    loadRoadnetFromFile,
    roadnetReaderTypes,
    roadnetDataSources,
    currentRoadnetBinaries,
    dumpRoadnetToBinary,
    loadRoadnetFromBinary,
    getRoadnet,
  };
}
