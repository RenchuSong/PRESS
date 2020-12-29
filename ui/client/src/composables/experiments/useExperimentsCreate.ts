import { Store } from "@/store";
import { ActionTypes } from "@/store/store-types";
import { reactive, Ref } from "vue";

export default function useExperimentsCreate(store: Store) {
  const newExperimentForm = reactive<{
    name: string,
    image: string | undefined
  }>({
    name: "",
    image: undefined,
  });

  const createExperiment = async () => {
    const newExperimentMeta = await store.dispatch(
      ActionTypes.CREATE_EXPERIMENTS,
      {
        name: newExperimentForm.name,
        image: newExperimentForm.image
      }
    );
    newExperimentForm.name = "";
    newExperimentForm.image = undefined;
    return newExperimentMeta;
  };

  return {
    newExperimentForm,
    createExperiment
  };
}
