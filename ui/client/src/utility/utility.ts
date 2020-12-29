import * as moment from "moment";

export function formatDatetime(
  epoch: number,
  format = "YYYY-MM-DD HH:mm:ss"
) {
  return moment.unix(epoch).format(format);
}
