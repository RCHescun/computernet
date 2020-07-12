/**************************************************************************
 *
 *                  Copyright (c) 2013 ZTE Corporation.
 *
 ***************************************************************************
 * ģ �� �� : Atserver
 * �� �� �� : at_com.h
 * ����ļ� : 
 * ʵ�ֹ��� : ATģ�鹫���ꡢ���Ͷ���
 * ��    �� : syx00130574
 * ��    �� : V1.0
 * ������� : 2013-04-16 
 * ����˵�� : 
 **************************************************************************/

/**************************************************************************
 * �޸ļ�¼ : 
 ***************************************************************************/
/**************************************************************************
 * �޸ı�� : 
 * �� �� �� : 
 * �޸����� : 
 * �޸����� : 
 **************************************************************************/
/**************************************************************************
 * �޸ı�� : �¹���
 * �� �� �� : chenjie10092374
 * �޸����� : 2014.08.15
 * �޸����� : �����С������
 **************************************************************************/
/**************************************************************************
 * �޸ı�� : ��
 * �� �� �� : chenjie10092374
 * �޸����� : 2014.08.19
 * �޸����� : �޸���С��ʵ������,��Ϊ����ԭ�ȵĶ�ʱ������
 **************************************************************************/
#ifndef _at_com_h_
#define _at_com_h_


/**************************************************************************
 * #include��
 **************************************************************************/
#include "ufi_def.h"
//#include "cfg.h"
#ifndef WIN32
#include "zte_log.h"
#endif
/**************************************************************************
 * ����������
 **************************************************************************/
#define ZAT_CMD_NAME_MAXLEN         32
#define ZAT_CFG_VALUE_MAXLEN        256
#define ZAT_MAX_LEN                 7168 //1024
/* AT��Ӧ��� */
#define ZAT_RESULT_OK               0x01    /* ����ִ�гɹ� */
#define ZAT_RESULT_ERROR            0x02    /* ����ִ��ʧ�� */
#define ZAT_RESULT_AUTOREPORT       0x03    /* �����ϱ�*/
/*add by zpr*/
#define ZAT_RESULT_SMS              0x04    /* SMSר��*/

/* ��ʱ����ʱ��Ϣ */
#define ZAT_PDPACT_TIMEOUT          260
//for lock cell
#define ZAT_CHECKLOCKCELL_CMD    261
#define ZAT_TIMERLOCKCELL_CMD    262
//for checkoutdatesms
#define ZAT_CHECKOUTDATESMS_TIMEOUT     263
#define ZAT_QUERYCSQ_TIMEOUT     264
#define ZAT_PPPD_PDPACT_TIMEOUT  265


#define ZAT_INVALID_CMD             0xFFFFFFFF
#define ZAT_INVALID_ID              -1
#define ZAT_ATRES_PDU_FLAG          1

#define ZAT_MSG_TYPE_DEFAULT        1    /*lMsgTypeĬ�ϵ���Ϣ����*/
#define ZAT_MSGTYPE_WEBCGI          2
#define ZAT_MSGTYPE_ATRESP          3
#define ZAT_MSGTYPE_ATMAIN          4

#define ZAT_CGATT_ATTACH            "1"  
#define ZAT_CGATT_DETACH            "0"  
#define ZAT_ERRCODE_MEMORY_FULL     "20"
/**************************************************************************
 * ���ݽṹ������
 **************************************************************************/
/* ĿǰUFI֧�ֵ�AT����ID��Ҫ���0��ʼ������ʹ�������ຯ��ָ�� */
typedef enum
{
	ZAT_AT_CMD=0,
	ZAT_CGSN_CMD=1,
	ZAT_CGMR_CMD,
	ZAT_ZMDMVER_CMD,
	ZAT_HVER_CMD,
	ZAT_ZPCUIVER_CMD,
	ZAT_ZVERSION_CMD,
	ZAT_ZSET_CMD,
	ZAT_CPINQUERY_CMD,
	ZAT_CLCK_CMD,

	ZAT_CLCKQUERY_CMD,
	ZAT_CPWD_CMD,
	ZAT_CPINACT_CMD,
	/*web����AT��PUK�������PIN+newpin*/
	ZAT_CPINPUKACT_CMD,
	ZAT_SYSCONFIGREAD_CMD,
	ZAT_SYSCONFIG_CMD,
	ZAT_ZRAP_CMD,
	ZAT_CGDCONT_CMD,
	ZAT_CGDCONTREAD_CMD,
	ZAT_ZGAAT_CMD,

	ZAT_CFUN_CMD,
	ZAT_CGREGSET_CMD,
	ZAT_CEREGSET_CMD,
	ZAT_ZSQR_CMD,
	ZAT_CSQ_CMD,
	ZAT_ZLTELCREAD_CMD,
	ZAT_CIMI_CMD,
	ZAT_CGACT_CMD,
	ZAT_ZGACT_PDPACTIVE_CMD,
	ZAT_CGACT_PDPDEACTIVE_CMD,

	ZAT_CGATT_CMD,
	ZAT_ZLTELCSET_CMD,
	ZAT_ZEACT_CMD,
	ZAT_COPSSET_CMD,
	ZAT_COPSAUTO_CMD,
	ZAT_COPSREAD_CMD,
	ZAT_COPSTEST_CMD,
	ZAT_COPSFORMAT_CMD,
	ZAT_ZSOFTRESET_CMD,
	/*�����ϱ�AT����*/
	//ZAT_COPS_CMD,
	ZAT_CREG_CMD,
	ZAT_CGREG_CMD,

	ZAT_CEREG_CMD,
	ZAT_ZGIPDNS_CMD,
	ZAT_ZCONSTAT_CMD,
	ZAT_ZPBIC_CMD,
	ZAT_CGEV_CMD,
	ZAT_CPMS_CMD,
	ZAT_CNMI_CMD,
	ZAT_CSCA_CMD,
	ZAT_CSCA_SET_CMD, //add by shisheng 2013-11-1
	ZAT_SYSINFO_CMD,

	/*add by zpr start*/   
	ZAT_CMGL_CMD,
	ZAT_CMGR_CMD,
	ZAT_CMGD_CMD,
	ZAT_CMGS_CMD,
	ZAT_CMTI_CMD,
	/*add by zpr end*/
	ZAT_CPBS_SET_CMD,
	ZAT_CPBS_READ_CMD,
	ZAT_SCPBR_TEST_CMD,
	ZAT_SCPBR_SET_CMD,
	ZAT_SCPBW_DEL_CMD,

	ZAT_SCPBW_ADD_MODIFY_CMD,
	ZAT_GETMAC_CMD,
	ZAT_SETSSID_CMD,
	ZAT_ZICCID_CMD,
	ZAT_ZMSRI_CMD,
	ZAT_ZCDCAT_CMD,
	ZAT_ZCDCLOG_CMD,
	ZAT_ZCDCMODEM_CMD,
	ZAT_ZMGR_CMD,
	ZAT_ZMGL_CMD,

	ZAT_TCARDSTAT_CMD,
	ZAT_ZSETTCARD_READ_CMD,
	ZAT_ZSETTCARD_CMD,
	ZAT_CDSI_CMD,
	ZAT_ZLOGDIR_SET_CMD,
	ZAT_ZLOGDIR_GET_CMD,
	ZAT_ZMODE_CMD,
	ZAT_SETWIFIKEY_CMD,
	ZAT_CNUM_CMD,
	ZAT_CMT_CMD,

	ZAT_CDS_CMD,
	ZAT_ZMENA_CMD,
	ZAT_CNMA_CMD,
	ZAT_CSMS_CMD,
	ZAT_SYCTIME_CMD,
	ZAT_MODE_CMD,
	ZAT_DSAUTH_CMD,
	ZAT_CEREGGET_CMD, //for test lockcell
	ZAT_ZSETUSB_CMD,
	ZAT_ZUSLOT_CMD,

	ZAT_CGDCONTSET_CMD,
	ZAT_ZGPCOAUTH_CMD,
	ZAT_ZSINR_CMD,
	ZAT_ZEMSCIQ_CMD,
	ZAT_ZEMSCI_CMD,
	ZAT_ZEMSCI_REP_CMD,
	ZAT_CSIM_CMD,
	ZAT_CARDMODE_CMD,
	ZAT_CRSM_CMD,
	ZAT_RAMDUMP_CMD,
	ZAT_CUSD_CMD,
	ZAT_CUSDRSP_CMD,
	ZAT_LOCKNETAUTH_CMD,
	ZAT_ZSEC_CMD,
	ZAT_ZNCK_GET_CMD,
	ZAT_ZNCK_SET_CMD,
	ZAT_POWERSAVE_CMD,
	ZAT_GETMAC2_CMD,
	ZAT_BOARDNUM_READ_CMD,
	ZAT_VOLTEPOWERSAVE_CMD,
	ZAT_CVMOD_SET_CMD,
	ZAT_ZGPCOCHAP_CMD,
	ZAT_ZGPCOAUTHSET_CMD,
	ZAT_CRSM_PLMNLEN_CMD,
	ZAT_BATDET_CMD,
	ZAT_ZEMSCIQSET_CMD,

	ZAT_CMD_MAX
} T_zAt_CmdId;

/* AT��Ӧ�ṹ */
typedef struct
{
	UINT8  result;//�������ϱ���    
	UINT8  pduFlag;
	int  atCmdId;//��Ӧ�ĺ�ֵ
	CHAR   resParas[ZAT_MAX_LEN];
} T_zAt_AtRes;

typedef enum
{
	TIMER_SYN_MODEM_INFO = 1, /*ͬ��MODEM��Ϣ�Ķ�ʱ��*/
	TIMER_RETRY_CPMS_CMD,     /*����CPMS����Ķ�ʱ��*/
	TIMER_RETRY_ZSEC_CMD,     /*����ZSEC����Ķ�ʱ��*/
	TIMER_PPP_STATE_DETECT,     /*ppp��ʱ���Ķ�ʱ��*/
	TIMER_RETRY_CPIN_CMD,     /*����CPIN����Ķ�ʱ��*/
	TIMER_QMI_STATE_DETECT,     /*QMI��ʱ���Ķ�ʱ��*/
	TIMER_SMS_RELOAD ,        /*SMSģ���ط�CMGL����Ķ�ʱ��*/
	TIMER_RETRY_CSCA_CMD,     /*����CSCA����Ķ�ʱ��*/
	TIMER_RETRY_ZSNT_CMD,
	TIMER_ECM_STATE_DETECT,     /*ECM DIAL��ʱ���Ķ�ʱ��*/
	TIMER_RETRY_CNUM_CMD,     /*����CNUM����Ķ�ʱ��*/
	ZAT_TIMER_ATMAIN_DETECT,
	ZAT_TIMER_ATMAIN_PPPD_DETECT,
	TIMER_RESETCPE_CMDS,
	TIMER_LOCK_CELL_COUNT, /*��С����ʱ*/
	TIMER_AUTO_SNTP_CMD,
	ZAT_TIMER_SINR_DETECT,
} T_zAt_EmSoftTimerId;
/**************************************************************************
 * ����������
 **************************************************************************/

/**************************************************************************
 * ȫ�ֱ���������
 **************************************************************************/
#endif
