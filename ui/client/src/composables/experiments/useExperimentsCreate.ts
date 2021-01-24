import { Store } from "@/store";
import { ActionTypes, MutationTypes } from "@/store/store-types";
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
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.CREATE_EXPERIMENTS,
      text: "Creating experiment"
    });
    const newExperimentMeta = await store.dispatch(
      ActionTypes.CREATE_EXPERIMENTS,
      {
        name: newExperimentForm.name,
        image: newExperimentForm.image
      }
    );
    newExperimentForm.name = "";
    newExperimentForm.image = undefined;
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.CREATE_EXPERIMENTS
    });
    return newExperimentMeta;
  };

  return {
    newExperimentForm,
    createExperiment
  };
}
