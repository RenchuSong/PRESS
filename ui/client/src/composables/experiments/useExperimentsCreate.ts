import { Store } from "@/store";
import { ActionTypes } from "@/store/store-types";
import { Ref, ref } from "vue";

export default function useExperimentsCreate(store: Store) {
  const newExperimentName = ref("");
  const newExperimentImage: Ref<string | undefined> = ref(undefined);

  const createExperiment = async () => {
    await store.dispatch(ActionTypes.CREATE_EXPERIMENTS, {
      name: newExperimentName.value,
      image: newExperimentImage.value
    });
    newExperimentName.value = "";
    newExperimentImage.value = undefined;
  };

  return {
    newExperimentName,
    newExperimentImage,
    createExperiment
  };
}
