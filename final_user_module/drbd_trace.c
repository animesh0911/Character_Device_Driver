#include "tracer.h"
int main(int argc, char *argv[]) {
	int fd = 0, i = 0, ret = 0;
	struct user_data *udata = NULL;
	char ts[16];
	memset(ts, 0, sizeof(ts));
retry:
	udata = malloc (sizeof (struct user_data));
	if (udata == NULL) {
		printf("udata allocation failed\n");
		goto retry;
	}
	memset (udata, 0, sizeof(struct user_data));
retry1:
	udata->u_data = malloc(sizeof(struct trace_data) * 8192);
	if (!udata->u_data) {
		printf("udata->u_data allocation failed\n");
		goto retry1;
	}
	for(i=0;i<8192;i++){
		udata->u_data[i].p_data = malloc(sizeof (struct p_data));
		if(!udata->u_data[i].p_data){
			printf("udata->u_data[i].p_data allocation failed\n");
			goto retry1;
		}
	}
retry2:
	fd = open("/dev/drbd_tracer", O_RDONLY);
	if (fd < 0) {
		printf("open failed /dev/drbd_tracer\n");
		goto retry2;
	}

	udata->u_size = 8192;

	pid_t process_id = 0;
	pid_t sid = 0;
	process_id = fork();
	if(process_id < 0){
		printf("fork failed");
		exit(1);
	}
	if(process_id > 0){
		printf("Daemon Process ID : %d\n",process_id);
		exit(0);
	}
	sid = setsid();
	if(sid < 0)
		exit(1);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	FILE *fp = fopen("logs.txt","w+");

	while(1) {
		if ((ret = ioctl(fd, TRACE_DRBD_DATA, udata) > 0)) {
			for (i = 0; i < ret; i++) {
				strftime(ts, 16, "%b %d %T", localtime(&(udata->u_data[i].time_insec)));
				fprintf(fp,"time=%-15s jiffies=%llu msg_type=%d cmd=%d bi_size=%llu ",
				ts,udata->u_data[i].jiffies, udata->u_data[i].msg_type, udata->u_data[i].cmd,
				udata->u_data[i].bi_size);
				
				fprintf(fp,"packet_seq_no=%u packet_dp_flag=%u sector=%llu block_id=%llu buf=%llx\n",
				udata->u_data[i].p_data->seq_num, udata->u_data[i].p_data->dp_flags,
				udata->u_data[i].p_data->sector, udata->u_data[i].p_data->block_id,
				udata->u_data[i].buf_ptr);
				fflush(fp);
				free(udata->u_data[i].p_data);
				udata->u_data[i].p_data = malloc(sizeof (struct p_data));
			}
			ret = 0;
		}
	}
	fclose(fp);
	free(udata->u_data);
	free(udata);
	return 0;
}
