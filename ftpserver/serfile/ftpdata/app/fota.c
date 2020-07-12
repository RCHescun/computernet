#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>

#define FOTA_IP "xxx.xxx.xxx.xxx"
#define PORT xxxx
#define FOTA_MAIN_PATH                             "/cache/zte_fota/"
#define FOTA_DUA_MAIN_PATH                         FOTA_MAIN_PATH"dua/"
#define FOTA_DUA_UPDATE_STATUS_FILE                FOTA_DUA_MAIN_PATH"update_status"

typedef enum{
	FOTA_VERIFY_SUCCESS,
	FOTA_VERIFY_FAIL,
	FOTA_SYSTEM_UPDATE_SUCCESS,
	FOTA_SYSTEM_UPDATE_FAIL,
	FOTA_RECOVERY_UPDATE_SUCCESS,
	FOTA_RECOVERY_UPDATE_FAIL,
	FOTA_NO_NEED_UPDATE,
	FOTA_NEED_UPDATE
}E_PCS_FOTA_STATE;

static int32_t iFnGetFotaResult()
{
	E_PCS_FOTA_STATE eFotaState = FOTA_VERIFY_FAIL;
	FILE * fd = NULL;
	int32_t iResult = -1;

	//PC_SERVER_LOG();

	fd = fopen(FOTA_DUA_UPDATE_STATUS_FILE, "rb+");
	if(NULL == fd)
	{
		printf("Call fopen() error.");
		return -1;
	}

	iResult = fscanf(fd, "%d", (int32_t *)&eFotaState);
	if(iResult < 0)
	{
		printf("Call fscanf() error.");
		fclose(fd);
		fd = NULL;
		return -1;
	}

	fclose(fd);
	fd = NULL;
	return eFotaState;
}

int main(int argc, char *argv[])
{
	int iResult;
	if (argc < 2) return 1;
	char cmd[256];
	printf("Call argv %s", argv[1]);
	sprintf(cmd, "wget -O /cache/delta.package %s", argv[1]);
	system(cmd);
	system("zte_fota_reco verify");
	iResult = iFnGetFotaResult();
	if (iResult) {
		return 1;
	}
	system("zte_fota_reco set_update_flag");
	system("sync");
	sleep(3);
	system("zte_fota_reco update_recoveryfs &");

	return 0;
}
