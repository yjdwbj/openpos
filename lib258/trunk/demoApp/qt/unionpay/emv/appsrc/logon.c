/*
	SAND China
--------------------------------------------------------------------------
	FILE  logon.c								(Copyright SAND 2001)
--------------------------------------------------------------------------
    INTRODUCTION
    ============
    Created :		2001-07-02		Xiaoxi Jiang
    Last modified :	2001-07-02		Xiaoxi Jiang
    Module :  Shanghai Golden Card Network
    Purpose :
        Source file for logon process.

    List of routines in file :

    File history :

*/

#include <include.h>
#include <global.h>
#include <xdata.h>


#include <emvinterface.h>
#define TEST
//extern const unsigned char SupportRIDNum;
void LOGON_Unpack62Field(unsigned char *Field,short iLength);
void LOGON_Pack62Field(unsigned char *Field,short *iLength);

unsigned char LOGON_ProcessCOMMPARAM(unsigned char ucFlag)
{
    unsigned char ucResult,ucI,ucJ,ucK;
    unsigned char aucBuf[128],aucTemp[32],ucMIPLen;
    unsigned char aucTempIP[16],aucTempM[25];
    unsigned char aucTempPort[12],aucBuff[12];
    ULONG_C51 ulIP;
    short iLen;
    unsigned int uiIPPort;

    util_Printf("\n=====++=ͨѶ��·Ϊǰ��ת��=++==ucFlag[%d]======\n",ucFlag);
    ucResult = SUCCESS;
    memset(aucBuf,0,sizeof(aucBuf));
    iLen=0;
    ucResult = ISO8583_GetBitValue(63,aucBuf,&iLen,sizeof(aucBuf));
    if (ucResult != SUCCESS) return(ucResult);
    for(ucI=0;ucI<iLen;ucI++)
    {
        if (ucI%10==0)util_Printf("\n");
        util_Printf("%02x ",aucBuf[ucI]);
    }
    util_Printf("\n");

    if (ucFlag ==0 || ucFlag == 2)
    {
        //�绰����1
        memset(aucTemp,0,sizeof(aucTemp));
        memset(aucTempM,0,sizeof(aucTempM));
        memcpy(aucTemp,aucBuf,15);
        util_Printf("�绰11:%s\n",aucTemp);

        for(ucI=0,ucJ=0;ucI<15;ucI++)
        {
            util_Printf("[%02x]",aucTemp[ucI]);
            if (aucTemp[0]==' ')
            {
                ucResult = ERR_END;
                break;
            }

            if ((aucTemp[ucI] != ' ')
            && (aucTemp[ucI] != '-')
            && (aucTemp[ucI] != '.')
            )
            {
                memcpy(&aucTempM[ucJ],&aucTemp[ucI],1);
                ucJ++;
            }
        }
        if (ucResult == SUCCESS)
        {
            ucMIPLen = strlen((char *)aucTempM);
            aucTempM[ucMIPLen] = '.';
            Os__saved_copy(aucTempM,DataSave0.ConstantParamData.aucHostTelNumber1,ucMIPLen+1);
            util_Printf("\n�绰1:%s\n",DataSave0.ConstantParamData.aucHostTelNumber1);
        }

        //�绰����2
        memset(aucTemp,0,sizeof(aucTemp));
        memset(aucTempM,0,sizeof(aucTempM));
        memcpy(aucTemp,&aucBuf[15],15);
        util_Printf("\n�绰22:%s\n",aucTemp);

        for(ucI=0,ucJ=0;ucI<15;ucI++)
        {
            util_Printf("[%02x]",aucTemp[ucI]);
            if (aucTemp[0] ==' ')
            {
                ucResult = ERR_END;
                break;
            }
            if ((aucTemp[ucI]!=' ')
            && (aucTemp[ucI]!='-')
            && (aucTemp[ucI] != '.')
            )
            {
                memcpy(&aucTempM[ucI],&aucTemp[ucI],1);
                ucJ++;
            }
        }
        if (ucResult == SUCCESS)
        {
            ucMIPLen = strlen((char *)aucTempM);
            aucTempM[ucMIPLen] = '.';
            Os__saved_copy(aucTempM,DataSave0.ConstantParamData.aucHostTelNumber2,ucMIPLen+1);
            util_Printf("\n�绰2:%s\n",DataSave0.ConstantParamData.aucHostTelNumber2);
        }
    }

    if (ucFlag ==1 || ucFlag == 2)
    {
        //����IP
        memset(aucTemp,0,sizeof(aucTemp));
        memset(aucTempIP,0,sizeof(aucTempIP));
        memcpy(aucTemp,&aucBuf[30],15);
        aucTemp[15]=0x20;

        if (aucTemp[0] != ' ' )
        {
            util_Printf("\nδת��IP:%s\n",aucTemp);
            memset(aucBuff,0,sizeof(aucBuff));
            for(ucI=0,ucJ=0,ucK=0;ucI<=15;ucI++)
            {
                if (aucTemp[ucI] =='.' || aucTemp[ucI] ==0x20)
                {
                    ulIP= asc_long(&aucTemp[ucK],ucI-ucK);
                    memcpy(&aucBuff[ucJ],&ulIP,sizeof(unsigned long));
                    ucJ++;
                    ucK=ucI+1;
                }
                if(aucTemp[ucI] ==0x20)
                    break;
            }
            memcpy((unsigned char*)&DataSave0.ConstantParamData.ulHostIPBack1,aucBuff,4);
            memcpy((unsigned char*)&DataSave0.ConstantParamData.ulHostIPBack2,aucBuff,4);
            memcpy((unsigned char*)&DataSave0.ConstantParamData.ulHostIPBack3,aucBuff,4);

            memcpy((unsigned char *)&DataSave0.ConstantParamData.ulHostIP,aucBuff,4);
            XDATA_Write_Vaild_File(DataSaveConstant);
         }

        //����IP�˿�
        memset(aucTemp,0,sizeof(aucTemp));
        memset(aucTempPort,0,sizeof(aucTempPort));
        memcpy(aucTemp,&aucBuf[45],8);
        if (aucTemp[0] !=' ' )
        {
            for(ucI=0,ucJ=0;ucI<8;ucI++)
            {
                if (aucTemp[ucI] != ' ')
                {
                    memcpy(&aucTempPort[ucI],&aucTemp[ucI],1);
                    ucJ++;
                }
            }
            if (ucJ<6)
            {
                memset(aucTemp,0x30,sizeof(aucTemp));
                memcpy(&aucTemp[6-ucJ],aucTempPort,ucJ);
            }
            uiIPPort = asc_long(aucTemp,6);
            DataSave0.ConstantParamData.uiHostPort = uiIPPort;
            util_Printf("\nIP1PORT:[%d][%3d]\n",uiIPPort,DataSave0.ConstantParamData.uiHostPort);
        }
        //��������IP
        memset(aucTemp,0,sizeof(aucTemp));
        memset(aucBuff,0,sizeof(aucBuff));
        memcpy(aucTemp,&aucBuf[53],15);
        aucTemp[15]=0x20;
    //
        util_Printf("\nδת������IP:%s\n",aucTemp);
        for(ucI=0,ucJ=0,ucK=0;ucI<=15;ucI++)
        {
            if (aucTemp[ucI] =='.' || aucTemp[ucI] ==0x20)
            {
                ulIP= asc_long(&aucTemp[ucK],ucI-ucK);
                memcpy(&aucBuff[ucJ],&ulIP,sizeof(unsigned long));
                ucJ++;
                ucK=ucI+1;
            }
            if(aucTemp[ucI] ==0x20)
                break;
        }
    //
        memcpy((unsigned char*)&DataSave0.ConstantParamData.ulHostIPMIS1,aucBuff,4);
        memcpy((unsigned char*)&DataSave0.ConstantParamData.ulHostIPMIS2,aucBuff,4);
        memcpy((unsigned char*)&DataSave0.ConstantParamData.ulHostIPMIS3,aucBuff,4);

        memcpy((unsigned char *)&DataSave0.ConstantParamData.ulBackIP,aucBuff,4);

        //����IP���ݶ˿�
        memset(aucTemp,0,sizeof(aucTemp));
        memset(aucTempPort,0,sizeof(aucTempPort));
        memcpy(aucTemp,&aucBuf[68],8);

        for(ucI=0,ucJ=0;ucI<8;ucI++)
        {
            if (aucTemp[ucI] != ' ')
            {
                memcpy(&aucTempPort[ucI],&aucTemp[ucI],1);
                ucJ++;
            }
        }
        if (ucJ<6)
        {
            memset(aucTemp,0x30,sizeof(aucTemp));
            memcpy(&aucTemp[6-ucJ],aucTempPort,ucJ);
        }
        uiIPPort = asc_long(aucTemp,6);
        DataSave0.ConstantParamData.uiHostBackPort = uiIPPort;
        util_Printf("\nIP1PORT:[%d][%3d]\n",uiIPPort,DataSave0.ConstantParamData.uiHostBackPort);
     }

     XDATA_Write_Vaild_File(DataSaveConstant);
     return (SUCCESS);
}

unsigned char LOGON_CashierProcess(unsigned char ucFlag)
{
	unsigned char ucResult,ucI;
	short iLen;
	unsigned char aucBuf[10];
	unsigned char logonindex;
	ULONG_C51	ulBatchNumber;
	unsigned char ucTransType;

	ucResult = SUCCESS;
#ifdef TEST
	util_Printf("DataSave0.ChangeParamData.ucPackgeHeadEve=%02x\n",DataSave0.ChangeParamData.ucPackgeHeadEve);
#endif
	//if (DataSave0.ChangeParamData.ucKeyRight)
	//{
	//	//Os__clr_display(255);
	//	//Os__GB2312_display(1,0,(uchar *)" ��ʼ����Կʧ��");
	//	//Os__GB2312_display(2,0,(uchar *)"�޷���������ǩ��");
	//	MSG_WaitKey(3);
	//	return(ERR_END);
	//}
	DataSave0.ConstantParamData.ucSelIP = 1;
	XDATA_Write_Vaild_File(DataSaveConstant);

	//
	if(G_NORMALTRANS_ucTransType !=TRANS_LOGONOFF )
		DataSave0.ChangeParamData.ucCashierLogonFlag = 0;
	XDATA_Write_Vaild_File(DataSaveChange);

	if(!G_RUNDATA_ucDialFlag )
		COMMS_PreComm();
    	ucTransType = G_NORMALTRANS_ucTransType;
	util_Printf("Log [�����жϱ�־] = [%02x]\n",DataSave0.ConstantParamData.ucFunctStep);
//	if(DataSave0.ConstantParamData.ucFunctStep)
//	{
//		G_NORMALTRANS_ucTransType = TRANS_SETTLE;
//		ucResult=SERV_SettleProcess();
//		if( ucResult!=SUCCESS )
//		{
//			//Os__clr_display(255);
//			//Os__GB2312_display(1,0,(unsigned char *)"   �����½���");
//			MSG_WaitKey(10);
//			COMMS_FinComm();
//			return(SUCCESS);
//		}
//	}else
	{
	/* Input Cashier No*/
#if 0
		ucResult =CASH_GetLogonCasherNo(&ucIndex);
		if(!ucResult)
		{
			util_Printf("\nucIndex======dfdf===%d",ucIndex);
			DataSave0.ChangeParamData.ucCashierLogonIndex =ucIndex;
			logonindex = ucIndex;
			LOGON_WriteCashierLogonIndex(DataSave0.ChangeParamData.ucCashierLogonIndex);
		}
		else
		    return ucResult;
#else
        	logonindex = DataSave0.ChangeParamData.ucCashierLogonIndex =1;
        	LOGON_WriteCashierLogonIndex(DataSave0.ChangeParamData.ucCashierLogonIndex);
#endif
		if(DataSave0.ChangeParamData.ucScriptInformValid)
		{
			ucResult=Trans_IssuerScriptInformSend();
			if(ucResult) return ucResult;
		}
#if 0        
		util_Printf("������־---logon--- = %02x\n",DataSave0.Trans_8583Data.ReversalISO8583Data.ucValid);
		if( DataSave0.Trans_8583Data.ReversalISO8583Data.ucValid == DATASAVEVALIDFLAG)
		{
			ucTransType = G_NORMALTRANS_ucTransType;
			G_NORMALTRANS_ucTransType=TRANS_REVERSAL;
			ucResult = Trans_SendReversalTrans();
			if (ucResult)	return(ucResult);
			G_NORMALTRANS_ucTransType = ucTransType;
		}else
		{
			Os__saved_set(&DataSave0.Trans_8583Data.ReversalISO8583Data.ucAuthFlag,0,1);
			XDATA_Write_Vaild_File(DataSaveTrans8583);
		}
#endif        
	}

	G_NORMALTRANS_ucTransType = ucTransType;
	if(ucResult == SUCCESS)
	{
		if(G_NORMALTRANS_ucTransType == TRANS_LOGONOFF)
		{
			if(DataSave0.ChangeParamData.ucCashierLogonFlag!=CASH_LOGONFLAG)
			{
				ucResult =  ERR_CASH_NOTLOGON;
			}
			if(ucResult == SUCCESS)
			{
				if(((DataSave0.TransInfoData.ForeignTransTotal.uiTotalNb!=0)
					||(DataSave0.TransInfoData.TransTotal.uiTotalNb!=0))
					)
				{
					ucResult = ERR_SETTLE_FIRST;
				}
			}
		}
	}

	if( ucResult == SUCCESS)
	{
		ucResult = LOGON_Online();
	}

	if( ucResult == SUCCESS)
	{
		switch(G_NORMALTRANS_ucTransType)
		{
		case TRANS_LOGONON:
			if(DataSave0.ConstantParamData.ENCRYPTTYPEParam==ENCRYPTTYPE_SINGLE)
			{
				ucResult = UTIL_Single_StoreNewKey();
			}else
			if(DataSave0.ConstantParamData.ENCRYPTTYPEParam==ENCRYPTTYPE_DOUBLE)
			{
				ucResult = UTIL_Double_StoreNewKey();
			}
			if( ucResult != SUCCESS)
			{
				Os__saved_set(&DataSave0.ChangeParamData.ucStoreKeyRight,1,1);
				XDATA_Write_Vaild_File(DataSaveChange);
				break;
			}
			Os__saved_set(&DataSave0.ChangeParamData.ucStoreKeyRight,0,1);
			XDATA_Write_Vaild_File(DataSaveChange);

			ucResult = ISO8583_GetBitValue(60,aucBuf,&iLen,sizeof(aucBuf));

			if( ucResult == SUCCESS)
			{
				/*�ý����־*/
				Os__saved_set((unsigned char *)&DataSave0.ConstantParamData.aucSettleFlag,
						1,sizeof(unsigned char));
				XDATA_Write_Vaild_File(DataSaveConstant);
				/*ǩ���ɹ����ɽ���־ɾ��*/
				Os__saved_set((unsigned char *)&DataSave0.ConstantParamData.ucLogonModiBatch,
						0,sizeof(unsigned char));
				XDATA_Write_Vaild_File(DataSaveConstant);
				ulBatchNumber = asc_long(&aucBuf[2],6);

				Os__saved_copy(	(unsigned char *)&ulBatchNumber,
						(unsigned char *)&DataSave0.ChangeParamData.ulBatchNumber,
						sizeof(ULONG_C51));
				//��ǩ����־
				LOGON_WriteCashierLogonFlag(CASH_LOGONFLAG);
				XDATA_Write_Vaild_File(DataSaveChange);

                            //��֧��ǰ��ת���򽫿��ܸ�����ز���
				if (DataSave0.ConstantParamData.ucMBFEFlag == 0x31)
			       {
                                memset(aucBuf,0,sizeof(aucBuf));
                                iLen=0;
                                ucResult = ISO8583_GetBitValue(61,aucBuf,&iLen,sizeof(aucBuf));
                                util_Printf("\n=====++=61��=++========\n");
			            for(ucI=0;ucI<iLen;ucI++)
			            {
			                if (ucI%10==0)util_Printf("\n");
			                util_Printf("%02x ",aucBuf[ucI]);
			            }
			            util_Printf("\n");
			            if (ucResult == SUCCESS)
        		            {
        		                if (aucBuf[0] ==0x31 && aucBuf[1] != 0x31)          //���µ绰
        		                    LOGON_ProcessCOMMPARAM(0);
        		                else if (aucBuf[0] !=0x31 && aucBuf[1] == 0x31)   //����IP
        		                    LOGON_ProcessCOMMPARAM(1);
        		                else if (aucBuf[0] ==0x31 && aucBuf[1] == 0x31) //���߶���Ҫ����
        		                    LOGON_ProcessCOMMPARAM(2);
        		            }
        		            else
        		                ucResult=SUCCESS;
			       }
				//������Կ������� ׼��EMV�������ؼ���Կ����
				util_Printf("\nǩ������ ��������Ϊ[%02x]\n",G_NORMALTRANS_ucTransType);
				util_Printf("\nǩ������ ��ͷ[%02x]\n",DataSave0.ChangeParamData.ucPackgeHeadEve);

				if (DataSave0.ConstantParamData.ucLoadEMVParam =='1')
				{
				       ucTransType=G_NORMALTRANS_ucTransType;
				       if((!CAPK_TermCAPKNum()) ||(DataSave0.ChangeParamData.ucPackgeHeadEve==4)) //
        				{
        					G_RUNDATA_ucDialFlag = 1;
        					ucResult=EMV_LoadCAPK();
        				}
        				util_Printf("EMV_LoadCAPK ucResult %02x\n",ucResult);
        				if (!ucResult)
        				{
                				if((!TERMAPP_TermAppNum()) ||(DataSave0.ChangeParamData.ucPackgeHeadEve==5))
                				{
                					G_RUNDATA_ucDialFlag = 1;
                					ucResult=EMV_LoadAIDParam();
                				}
                			}
        				G_NORMALTRANS_ucTransType = ucTransType;
			       }
				util_Printf("EMV_LoadAIDParam ucResult %02x\n",ucResult);
				//EMV��������Կ�������̽���
			}
			else
			{
			    LOGON_WriteCashierLogonIndex(logonindex);
			}
			break;
		case TRANS_LOGONOFF:
			LOGON_WriteCashierLogonFlag(0);
			UTIL_IncreaseTraceNumber();
			break;
		default:
			break;
		}
	}
	else
	{
		if( G_NORMALTRANS_ucTransType==TRANS_LOGONON )
	    		LOGON_WriteCashierLogonIndex(logonindex);
	}
	if(ucResult == SUCCESS
		&&( G_NORMALTRANS_ucTransType==TRANS_LOGONON ))
	{
		if(DataSave0.ChangeParamData.ucPackgeHeadEve)
		{
			ucTransType=G_NORMALTRANS_ucTransType;
			ucResult=EMV_PackgeHeadEveProcess(DataSave0.ChangeParamData.ucPackgeHeadEve);
			Os__saved_set(&DataSave0.ChangeParamData.ucPackgeHeadEve,0,1);
			G_NORMALTRANS_ucTransType=ucTransType;
		}
	}
	if (!ucFlag)
		COMMS_FinComm();
	DataSave0.ChangeParamData.ucDownLoadFlag=0;
	DataSave0.ChangeParamData.ucPackgeHeadEve = 0;
	if(G_NORMALTRANS_ucTransType !=TRANS_LOGONOFF )
		DataSave0.ChangeParamData.ucCashierLogonFlag = 0;
	XDATA_Write_Vaild_File(DataSaveChange);
	
	
util_Printf("\n 2012_jianggs 0000001 LOGON_CashierProcess ucResult: %02x, %02x\n ",ucResult, G_NORMALTRANS_ucTransType);

	if(!ucResult)
	{
		switch(G_NORMALTRANS_ucTransType)
		{
			case TRANS_LOGONON:
			case TRANS_CASHIERLOGON:
				DataSave0.ChangeParamData.ucCashierLogonFlag=0x55;
				XDATA_Write_Vaild_File(DataSaveChange);

util_Printf("\n 2012_jianggs 0000001 LOGON_CashierProcess ucFlag: %02x\n ",ucFlag);

				if (!ucFlag)
					MSG_DisplayMsgKey(true,1,4,MSG_LOGONSUCCESS,3);
				break;
			case TRANS_LOGONOFF:
			case TRANS_CASHIERLOGOFF:
				DataSave0.ChangeParamData.ucCashierLogonFlag=0;
				XDATA_Write_Vaild_File(DataSaveChange);
				if (!ucFlag)
					MSG_DisplayMsgKey(true,1,4,MSG_LOGOFFSUCCESS,3);
				break;
			default:
				break;
		}
	}
	else
        {
		switch(G_NORMALTRANS_ucTransType)
		{
			case TRANS_LOGONON:
			     MSG_DisplayMsgKey(true,1,2,MSG_LOGONFAIL,2);
			     break;
			case TRANS_LOGONOFF:
                            //Os__clr_display(255);
                            //Os__GB2312_display(2,2,(uchar *)"ǩ��ʧ��");
                            UTIL_GetKey(2);
				break;
			default:
				break;
		}
        }
	if( (ucResult != SUCCESS)&&(ucResult != ERR_CANCEL) &&(ucResult != ERR_END) )
	{
		MSG_DisplayErrMsg(ucResult);
		ucResult = ERR_CANCEL;
	}
	return(ucResult);
}

unsigned char LOGON_Online(void)
{
	unsigned char ucResult,ucI;
	unsigned char aucBuf[500];
	unsigned char aucDate[7];
	unsigned char aucTime[5],ucRIDIndex,ucAIDIndex,ucCAPKIndex;
	unsigned char aucTmpBuf[20],ucContinueFlag=false;
	short iLen;

	ucRIDIndex=0;
	ucAIDIndex=0;
	ucCAPKIndex=0;

	//Os__clr_display(255);
	ucResult = SUCCESS;

	do{
		ISO8583_Clear();
		/* Set Msgid ��Ϣ����*/
		switch(G_NORMALTRANS_ucTransType)
		{
			case TRANS_LOGONON:
			case TRANS_LOADPARAM:
				 ISO8583_SetMsgID(800);
				 break;
			case TRANS_LOGONOFF:
				ISO8583_SetMsgID(820);
				 break;
			case TRANS_ONLINETEST:
			case TRANS_SENDSTATS:
				if(DataSave0.ChangeParamData.euLoadParamType != LOADPARAMTYPE_PKLOADEND
					&&DataSave0.ChangeParamData.euLoadParamType != LOADPARAMTYPE_AIDLOADEND)
				 	ISO8583_SetMsgID(820);
				else
					ISO8583_SetMsgID(800);
				 break;
			default:
				break;
		}

		/* Set system trace audit number bit 11 ������ˮ*/
		memset(aucBuf,0,sizeof(aucBuf));
		long_asc(aucBuf,6,&DataSave0.ChangeParamData.ulTraceNumber);
		switch(G_NORMALTRANS_ucTransType)
		{
			case TRANS_LOGONON:
			case TRANS_LOGONOFF:
			    	 ISO8583_SetBitValue(11,aucBuf,6);
				 UTIL_IncreaseTraceNumber();
				 break;
			default:
				break;
		}

		/* Set Card Acceptor Terminal Identification bit 41 */
		ISO8583_SetBitValue(41,DataSave0.ConstantParamData.aucTerminalID,8);
		/* Set Card Acceptor Identification Code bit 42 */
		ISO8583_SetBitValue(42,DataSave0.ConstantParamData.aucMerchantID,15);

		memset(aucBuf,0,sizeof(aucBuf));
		memcpy(aucBuf,"00",2);
		long_asc(&aucBuf[2],6,&DataSave0.ChangeParamData.ulBatchNumber);
		switch(G_NORMALTRANS_ucTransType)
		{
			case TRANS_LOGONON:
				if(DataSave0.ConstantParamData.ENCRYPTTYPEParam == ENCRYPTTYPE_SINGLE)
					memcpy(&aucBuf[8],"001",3);
				if(DataSave0.ConstantParamData.ENCRYPTTYPEParam == ENCRYPTTYPE_DOUBLE)
					memcpy(&aucBuf[8],"003",3);
				 break;
			case TRANS_LOGONOFF:
				 memcpy(&aucBuf[8],"002",3);
				 break;
			case TRANS_ONLINETEST:
				 memcpy(&aucBuf[8],"301",3);
				 break;
			case TRANS_LOADPARAM:
				 switch(DataSave0.ChangeParamData.euLoadParamType)
				 {
				 	case LOADPARAMTYPE_MAG:
						 memcpy(&aucBuf[8],"360",3);
						 break;
					case LOADPARAMTYPE_PKLOAD:
						 memcpy(&aucBuf[8],"370",3);
						 break;
					case LOADPARAMTYPE_AIDLOAD:
						memcpy(&aucBuf[8],"380",3);
						break;
					case LOADPARAMTYPE_PKLOADEND:
						memcpy(&aucBuf[8],"371",3);
				 		break;
					case LOADPARAMTYPE_AIDLOADEND:
						memcpy(&aucBuf[8],"381",3);
						break;
				 }
				 break;
			case TRANS_SENDSTATS:
				switch(DataSave0.ChangeParamData.euLoadParamType)
				{
				 	case LOADPARAMTYPE_MAG:
				 		memcpy(&aucBuf[8],"362",3);
						break;
					case LOADPARAMTYPE_PKQUERY:
						 memcpy(&aucBuf[8],"372",3);
				 		break;
					case LOADPARAMTYPE_AIDQUERY:
						memcpy(&aucBuf[8],"382",3);
						break;
					case LOADPARAMTYPE_PKLOADEND:
						memcpy(&aucBuf[8],"371",3);
				 		break;
					case LOADPARAMTYPE_AIDLOADEND:
						memcpy(&aucBuf[8],"381",3);
						break;
				}
				break;
			default:
				break;
		}
#ifdef TEST
		memcpy(aucTmpBuf,aucBuf,12);
		aucTmpBuf[12] = 0;
		util_Printf("ISO8583_SetBitValue60=%s\n",aucTmpBuf);
#endif
		ISO8583_SetBitValue(60,aucBuf,11);
#if 1
		memset(aucBuf,0x00,sizeof(aucBuf));
		switch(G_NORMALTRANS_ucTransType)
		{
			case TRANS_LOADPARAM:
				 switch(DataSave0.ChangeParamData.euLoadParamType)
				 {
					case LOADPARAMTYPE_AIDLOAD:
						if(SupportAIDNum)
						{
							ucI=0;

							memcpy(&aucBuf[ucI],"\x9F\x06",2);
							ucI+=2;
							aucBuf[ucI++]=SupportAID[ucAIDIndex].ucAIDLen;

							memcpy(&aucBuf[ucI],SupportAID[ucAIDIndex].AID,SupportAID[ucAIDIndex].ucAIDLen);
							ucI+=SupportAID[ucAIDIndex].ucAIDLen;
							ISO8583_SetBitValue(62,aucBuf,ucI);
						}
						break;
					case LOADPARAMTYPE_PKLOAD:
						if(ucCAPKNum)
						{
							ucI=0;
							memcpy(&aucBuf[ucI],"\x9F\x06",2);
							ucI+=2;

							aucBuf[ucI++]=5;
							memcpy(&aucBuf[ucI],CAPKInfo[ucCAPKIndex].RID,5);
							ucI+=5;

							memcpy(&aucBuf[ucI],"\x9F\x22",2);
							ucI+=2;
							aucBuf[ucI++]=1;
							memcpy(&aucBuf[ucI],&CAPKInfo[ucCAPKIndex].CAPKI,1);
							ucI++;
							ISO8583_SetBitValue(62,aucBuf,ucI);
						}
						break;
					default:
						break;
				 }
				break;
			case TRANS_SENDSTATS:
				switch(DataSave0.ChangeParamData.euLoadParamType)
				{
				case LOADPARAMTYPE_MAG:
			 		memset(aucBuf,0,sizeof(aucBuf));
					 LOGON_Pack62Field(aucBuf,&iLen);
					 ISO8583_SetBitValue(62,aucBuf,iLen);
					break;
				case LOADPARAMTYPE_PKQUERY:
					 aucBuf[0]=0x31;
					 char_asc(&aucBuf[1],2,&ucCAPKNum);
					 iLen=strlen((char*)aucBuf);
					 ISO8583_SetBitValue(62,aucBuf,iLen);
					 break;
				case LOADPARAMTYPE_AIDQUERY:
					 aucBuf[0]=0x31;
					 char_asc(&aucBuf[1],2,&SupportAIDNum);
					 iLen=strlen((char*)aucBuf);
					 ISO8583_SetBitValue(62,aucBuf,iLen);
					 break;
				default:
					break;
				 }
			 	 break;
			default:
				break;
		}
#endif
		memset(aucBuf,' ',sizeof(aucBuf));
		memcpy(aucBuf,DataSave0.Cashier_SysCashier_Data.aucCashierNo[DataSave0.ChangeParamData.ucCashierLogonIndex],
				CASH_CASHIERNOLEN);
		switch(G_NORMALTRANS_ucTransType)
		{
		case TRANS_LOGONON:
			util_Printf("����63��");
			util_Printf("\naucBuf=%02x %02x %02x\n",aucBuf[0],aucBuf[1],aucBuf[2]);
			ISO8583_SetBitValue(63,aucBuf,3);
			break;
		default:
			break;
		}
		if( ucResult == SUCCESS )
		{
			ucResult = COMMS_GenSendReceive();
		}
		if( (ucResult == SUCCESS)
		  &&(G_NORMALTRANS_ucTransType == TRANS_LOGONON)
		  )
		{
			memset(aucBuf,0,sizeof(aucBuf));
			ucResult = ISO8583_GetBitValue(12,aucBuf,&iLen,sizeof(aucBuf));
			if(ucResult == SUCCESS)
			{
				memset(aucTime,0,sizeof(aucTime));
				memcpy(aucTime,aucBuf,4);
			}
			memset(aucBuf,0,sizeof(aucBuf));
			ucResult = ISO8583_GetBitValue(13,aucBuf,&iLen,sizeof(aucBuf));
			if(ucResult == SUCCESS)
			{
#ifdef USEUPDATADATATIME
				Os__read_date(aucDate);
				memcpy(&aucDate[0],&aucBuf[2],2);
				memcpy(&aucDate[2],&aucBuf[0],2);
				//memcpy(&aucDate[6],aucTime,4);
				Os__write_date(aucDate);  /*Ԥ���޸ģ�˰��Ҫ��ǩ������ʱ��*/
				Os__write_time(aucTime);
#else
				asc_bcd(DataSave0.ConstantParamData.aucRecvDate,2,aucBuf,4);
				asc_bcd(&DataSave0.ConstantParamData.aucRecvDate[2],2,aucTime,4);
				Os__read_date(aucDate);
				Os__read_time(aucTime);
				asc_bcd(&DataSave0.ConstantParamData.aucTermDate[0],1,&aucDate[2],2);
				asc_bcd(&DataSave0.ConstantParamData.aucTermDate[1],1,&aucDate[0],2);
				asc_bcd(&DataSave0.ConstantParamData.aucTermDate[2],2,&aucTime[0],4);
				XDATA_Write_Vaild_File(DataSaveConstant);
#endif
			}
			ucContinueFlag=false;
		}
#if 1
		if(ucResult == SUCCESS)
		{
			switch(G_NORMALTRANS_ucTransType)
			{
			case TRANS_LOADPARAM:
				memset(aucBuf,0,sizeof(aucBuf));
				util_Printf("\nISO8583_GetBitValue 62");
				ucResult = ISO8583_GetBitValue(62,aucBuf,&iLen,sizeof(aucBuf));
				 if(ucResult == SUCCESS)
				 {
				 	if(DataSave0.ChangeParamData.euLoadParamType==LOADPARAMTYPE_MAG
				 		&& DataSave0.ConstantParamData.ucInforFlag=='1')
				 		LOGON_Unpack62Field(aucBuf,iLen);
					else if(DataSave0.ChangeParamData.euLoadParamType==LOADPARAMTYPE_PKLOAD)
					{
						if(iLen>1&&(aucBuf[0]=='1'))
					 		EMVONLINE_CAPKProcess(NULL,&aucBuf[1],iLen-1);
						ucCAPKIndex++;
						if(ucCAPKIndex<ucCAPKNum)
						 	ucContinueFlag=TRUE;
						else ucContinueFlag=false;
					}
					else if(DataSave0.ChangeParamData.euLoadParamType==LOADPARAMTYPE_AIDLOAD)
					{
						if(iLen>1&&(aucBuf[0]=='1'))
					 		EMVONLINE_AIDParamProcess(NULL,&aucBuf[1],iLen-1);
						ucAIDIndex++;
						if(ucAIDIndex<SupportAIDNum)
						 	ucContinueFlag=TRUE;
						else ucContinueFlag=false;
					}
				 }
				  else
				 {
				 	ucResult=SUCCESS;
				 	ucContinueFlag=false;
				 }
				 break;
			case TRANS_SENDSTATS:
				switch(DataSave0.ChangeParamData.euLoadParamType)
				{
				case LOADPARAMTYPE_MAG:
					SEL_ClrAccumulatDial();
					break;
				case LOADPARAMTYPE_PKQUERY:
					memset(aucBuf,0,sizeof(aucBuf));
					ucResult = ISO8583_GetBitValue(62,aucBuf,&iLen,sizeof(aucBuf));
					util_Printf("\n aucBuf[0] %02x  iLen %d",aucBuf[0],iLen);
					if(ucResult == SUCCESS)
					{
					 	switch(aucBuf[0])
					 	{
							case '0':
								ucContinueFlag = false;//��û�к��������˳� changed 08-8-22 PM5:27
								break;
							case '1':
							case '3':
								EMVONLINE_AddRID(&aucBuf[1],iLen-1);
								ucContinueFlag=false;
								break;
							case '2':
								EMVONLINE_AddRID(&aucBuf[1],iLen-1);
								ucRIDIndex++;
								ucContinueFlag=true;
								break;
					 	}
					}
					break;
				case LOADPARAMTYPE_AIDQUERY:
					memset(aucBuf,0,sizeof(aucBuf));
					ucResult = ISO8583_GetBitValue(62,aucBuf,&iLen,sizeof(aucBuf));
					util_Printf("\n aucBuf[0] %02X iLen %l",aucBuf[0],iLen);
					if(ucResult == SUCCESS)
					{
					 	switch(aucBuf[0])
					 	{
					 		case '0':
					 			break;
					 		case '1':
					 		case '3':
					 			EMVONLINE_AddAID(&aucBuf[1],iLen-1);
					 			ucContinueFlag=false;
					 			break;
					 		case '2':
					 			EMVONLINE_AddAID(&aucBuf[1],iLen-1);
								ucAIDIndex++;
					 			ucContinueFlag=true;
					 			break;
					 	}
					}
					break;
				}
				break;
			default:
				ucContinueFlag=false;
				break;
			}
		}
#endif
	util_Printf("\nucResult %02x ucContinueFlag %02x",ucResult,ucContinueFlag);
	}while(!ucResult&&ucContinueFlag);
	util_Printf("\n-----LOGON --ucResult = %02X------2-----\n",ucResult);
	return(ucResult);
}
void LOGON_WriteSuperLogonFlag(unsigned char ucFlag)
{
	Os__saved_set(&DataSave0.ChangeParamData.ucSuperLogonFlag,
		ucFlag,1);
	XDATA_Write_Vaild_File(DataSaveChange);
}

void LOGON_WriteCashierLogonFlag(unsigned char ucFlag)
{
	Os__saved_set(&DataSave0.ChangeParamData.ucCashierLogonFlag,
		ucFlag,1);
	XDATA_Write_Vaild_File(DataSaveChange);
}

void LOGON_WriteCashierLogonIndex(unsigned char ucIndex)
{
	Os__saved_set(&DataSave0.ChangeParamData.ucCashierLogonIndex,
    	ucIndex,1);
	XDATA_Write_Vaild_File(DataSaveChange);
}
void LOGON_Unpack62Field(unsigned char *Field,short iLength)
{
	unsigned char  ucBitCode;
	unsigned char  ucChar,ucI;
	unsigned char  *aucPtr1,*aucPtr2;
	unsigned short   uiTempShort,uiParamLen;
	unsigned char 	aucBuf[50],ucResult;
	aucPtr1 = aucPtr2 = Field;
	uiTempShort = 0;

	Os__saved_set((unsigned char *)&DataSave0.ConstantParamData.ucLoadFlag,
		1,sizeof(unsigned char));

	do{
		asc_bcd(&ucBitCode,1,aucPtr2,2);
#ifdef TEST
		util_Printf("ucBitCode=%02x\n",ucBitCode);
#endif
		aucPtr2+=2;
		switch(ucBitCode)
		{
			case 0x11:
				Os__saved_copy(aucPtr2,DataSave0.ConstantParamData.aucTermianlApp,
						PARAM_TERMINALAPP);
#ifdef TEST
				util_Printf("0x11=%02x,%02x\n",*aucPtr2,*(aucPtr2+1));
#endif
				aucPtr2+=PARAM_TERMINALAPP;
				break;
			case 0x12:
				uiTempShort = (unsigned short)asc_long(aucPtr2,2);
				Os__saved_copy(	(unsigned char *)&uiTempShort,
					(unsigned char *)&DataSave0.ConstantParamData.uiReceiveTimeout,
					sizeof(unsigned short));
#ifdef TEST
				util_Printf("0x12=%02x,%02x\n",*aucPtr2,*(aucPtr2+1));
#endif
				aucPtr2+=2;
				break;
			case 0x13:
				ucChar = *aucPtr2;
				if(ucChar<0x30)
					ucChar = 0;
				else
					ucChar-=0x30;
				Os__saved_copy(&ucChar,&DataSave0.ConstantParamData.ucDialRetry,
					sizeof(unsigned char));
#ifdef TEST
				util_Printf("0x13=%02x\n",*aucPtr2);
#endif
				aucPtr2++;
				break;
			case 0x14:
				for(ucI=0;ucI<PARAM_HOSTNUMBERLEN;ucI++)
					if( (*(aucPtr2+ucI)==0x00)
					  ||(*(aucPtr2+ucI)==0x20)
					  )
					  	break;
				Os__saved_copy(aucPtr2,(unsigned char *)&DataSave0.ConstantParamData.aucHostTelNumber1,
					ucI);
				util_Printf("0x14=%02x\n",*aucPtr2);
				DataSave0.ConstantParamData.aucHostTelNumber1[ucI] = '.';
				util_Printf("0x14aucHostTelNumber1=%02x\n",DataSave0.ConstantParamData.aucHostTelNumber1);
				aucPtr2+=PARAM_HOSTNUMBERLEN;
				break;
			case 0x15:
				for(ucI=0;ucI<PARAM_HOSTNUMBERLEN;ucI++)
					if( (*(aucPtr2+ucI)==0x00)
					  ||(*(aucPtr2+ucI)==0x20)
					  )
					  	break;
				Os__saved_copy(aucPtr2,(unsigned char *)&DataSave0.ConstantParamData.aucHostTelNumber2,
					ucI);
				util_Printf("0x15=%02x\n",*aucPtr2);
				DataSave0.ConstantParamData.aucHostTelNumber2[ucI] = '.';
				util_Printf("0x15aucHostTelNumber1=%02x\n",DataSave0.ConstantParamData.aucHostTelNumber2);
				aucPtr2+=PARAM_HOSTNUMBERLEN;
				break;
			case 0x16:
				for(ucI=0;ucI<PARAM_HOSTNUMBERLEN;ucI++)
				if( (*(aucPtr2+ucI)==0x00)
				  ||(*(aucPtr2+ucI)==0x20)
				  )
				  	break;
				Os__saved_copy(aucPtr2,(unsigned char *)&DataSave0.ConstantParamData.aucHostTelNumber3,
					ucI);
				util_Printf("0x16=%02x\n",*aucPtr2);
				DataSave0.ConstantParamData.aucHostTelNumber3[ucI] = '.';
				util_Printf("0x16aucHostTelNumber1=%02x\n",DataSave0.ConstantParamData.aucHostTelNumber3);
				aucPtr2+=PARAM_HOSTNUMBERLEN;
				break;
			case 0x17:
				for(ucI=0;ucI<PARAM_HOSTNUMBERLEN;ucI++)
				if( (*(aucPtr2+ucI)==0x00)
				  ||(*(aucPtr2+ucI)==0x20)
				  )
				  	break;
				Os__saved_copy(aucPtr2,(unsigned char *)&DataSave0.ConstantParamData.aucHostTelNumber4,
					ucI);
				util_Printf("0x17=%02x\n",*aucPtr2);
				DataSave0.ConstantParamData.aucHostTelNumber4[ucI] = '.';
				util_Printf("0x17aucHostTelNumber1=%02x\n",DataSave0.ConstantParamData.aucHostTelNumber4);
				aucPtr2+=PARAM_HOSTNUMBERLEN;
				break;
			case 0x18:
				ucChar = *aucPtr2;
				if(ucChar<0x30)
					ucChar = 0;
				else
					ucChar-=0x30;
				Os__saved_copy(&ucChar,&DataSave0.ConstantParamData.ucXorTipFee,
					sizeof(unsigned char));
#ifdef TEST
				util_Printf("0x18=%02x\n",*aucPtr2);
#endif
				aucPtr2++;
				break;
			case 0x19:
				uiTempShort = (unsigned short)asc_long(aucPtr2,2);
				Os__saved_copy(	(unsigned char *)&uiTempShort,
					(unsigned char *)&DataSave0.ConstantParamData.usTipFeePre,
					sizeof(unsigned short));
#ifdef TEST
				util_Printf("0x19=%02x,%02x\n",*aucPtr2,*(aucPtr2+1));
#endif
				aucPtr2+=2;
				break;
			case 0x20:
				ucChar = *aucPtr2;
				if(ucChar<0x30)
					ucChar = 0;
				else
					ucChar-=0x30;
				Os__saved_copy(&ucChar,&DataSave0.ConstantParamData.ucInputCardNo,
					sizeof(unsigned char));
#ifdef TEST
				util_Printf("0x20=%02x\n",*aucPtr2);
#endif
				aucPtr2++;
				break;
			case 0x21:
				ucChar = *aucPtr2;
				Os__saved_copy(&ucChar,&DataSave0.ConstantParamData.ucSelfLogoff,
					sizeof(unsigned char));
#ifdef TEST
				util_Printf("0x21=%02x\n",*aucPtr2);
#endif
				aucPtr2++;
				break;
			case 0x22:
				memset(aucBuf,0,sizeof(aucBuf));
				memcpy(aucBuf,aucPtr2,PARAM_MERCHANTNAMELEN);
				Os__saved_copy(	aucBuf,(unsigned char *)&DataSave0.ConstantParamData.aucMerchantName,
					PARAM_MERCHANTNAMELEN);
				aucPtr2+=PARAM_MERCHANTNAMELEN;
				break;
			case 0x23:
				ucChar = *aucPtr2;
				if(ucChar<0x30)
					ucChar = 0;
				else
					ucChar-=0x30;
				Os__saved_copy(&ucChar,&DataSave0.ConstantParamData.ucTransRetry,
					sizeof(unsigned char));
#ifdef TEST
				util_Printf("0x23=%02x\n",*aucPtr2);
#endif
				aucPtr2++;
				break;
			case 0x24:
				ucChar = *aucPtr2;
				Os__saved_copy(&ucChar,&DataSave0.ConstantParamData.ucOFFLineSendType,
					sizeof(unsigned char));
#ifdef TEST
				util_Printf("0x24=%02x\n",*aucPtr2);
#endif
				aucPtr2++;
				break;
			case 0x25:
				ucChar = *aucPtr2-'0';
				Os__saved_copy(&ucChar,&DataSave0.ConstantParamData.ucUseKeyIndex,
					sizeof(unsigned char));
#ifdef TEST
				util_Printf("0x25=%02x\n",*aucPtr2);
#endif
				aucPtr2++;
				break;
			case 0x26:
				Os__saved_copy(	aucPtr2,
					(unsigned char *)&DataSave0.ConstantParamData.aucGlobalTransEnable,
					PARAM_GLOBALTRANSENABLELEN);
#ifdef TEST
				util_Printf("0x26=%02x,%02x\n",*aucPtr2,*(aucPtr2+1));
#endif
				aucPtr2+=PARAM_GLOBALTRANSENABLELEN;
				break;
/********************EMV Configation Table Message****************/
			case 0x80:
				uiParamLen=*aucPtr2;
	//bo			ucResult=EMVONLINE_UpdateEMVParam(aucPtr2+1,uiParamLen);
				aucPtr2+=uiParamLen+1;
				break;
			case 0x81:
				uiParamLen=*aucPtr2;
	//bo			ucResult=EMVONLINE_AddRID(aucPtr2+1,uiParamLen);
				aucPtr2+=uiParamLen+1;
				break;
			case 0x82:
				uiParamLen=*aucPtr2;
				aucPtr2+=uiParamLen+1;
				break;
#if 1
			case 0x83:
				uiParamLen=*aucPtr2;
				ucResult=EMVONLINE_CAPKProcess(OP_ADD,aucPtr2+1,uiParamLen);
				aucPtr2+=uiParamLen+1;
				break;
			case 0x84:
				uiParamLen=*aucPtr2;
				ucResult=EMVONLINE_CAPKProcess(OP_DELETE,aucPtr2+1,uiParamLen);
				aucPtr2+=uiParamLen+1;
				break;
			case 0x85:
				uiParamLen=*aucPtr2;
				ucResult=EMVONLINE_CAPKProcess(OP_MODIFYEXPIRE,aucPtr2+1,uiParamLen);
				aucPtr2+=uiParamLen+1;
				break;
#endif
/********************EMV Configation Table Message End****************/

/*
			case 0x51:
				Os__saved_copy(	aucPtr2,(unsigned char *)&DataSave0.ConstantParamData.aucPOSDialConnect,
					24);
				aucPtr2+=24;
				break;
*/
			default:
				break;
		}
	}while( (aucPtr2 - aucPtr1)<iLength);
	XDATA_Write_Vaild_File(DataSaveConstant);
	return;
}
void LOGON_Pack62Field(unsigned char *Field,short *iLength)
{
	short	uiOffset;
	unsigned char ucI,ucChar,aucBuf[300];
	unsigned short  uiTempShort;


	memset(aucBuf,' ',sizeof(aucBuf));
	uiOffset = 0;
	memcpy(&aucBuf[uiOffset],"011",3);
	uiOffset+=3;
	memcpy(&aucBuf[uiOffset],"021",3);
	uiOffset+=3;
	memcpy(&aucBuf[uiOffset],"031",3);
	uiOffset+=3;
	memcpy(&aucBuf[uiOffset],"041",3);
	uiOffset+=3;
	memcpy(&aucBuf[uiOffset],"051",3);
	uiOffset+=3;
	memcpy(&aucBuf[uiOffset],"11",2);
	uiOffset+=2;
	memcpy(&aucBuf[uiOffset],"60",
						PARAM_TERMINALAPP);
	uiOffset+=PARAM_TERMINALAPP;
	memcpy(&aucBuf[uiOffset],"12",2);
	uiOffset+=2;
	uiTempShort = DataSave0.ConstantParamData.uiReceiveTimeout;
	short_asc(&aucBuf[uiOffset],2,&uiTempShort);
	uiOffset+=2;
	memcpy(&aucBuf[uiOffset],"13",2);
	uiOffset+=2;
	ucChar = DataSave0.ConstantParamData.ucDialRetry+0x30;
	aucBuf[uiOffset] = ucChar;
	uiOffset++;
	memcpy(&aucBuf[uiOffset],"14",2);
	uiOffset+=2;
	memcpy(&aucBuf[uiOffset],DataSave0.ConstantParamData.aucHostTelNumber1,
						PARAM_HOSTNUMBERLEN);
	for(ucI=uiOffset;ucI<uiOffset+PARAM_HOSTNUMBERLEN;ucI++)
		if(aucBuf[ucI] == '.')
			break;
	memset(&aucBuf[ucI],' ',PARAM_HOSTNUMBERLEN+uiOffset-ucI);
	uiOffset+=PARAM_HOSTNUMBERLEN;
	memcpy(&aucBuf[uiOffset],"15",2);
	uiOffset+=2;
	memcpy(&aucBuf[uiOffset],DataSave0.ConstantParamData.aucHostTelNumber2,
						PARAM_HOSTNUMBERLEN);
	for(ucI=uiOffset;ucI<uiOffset+PARAM_HOSTNUMBERLEN;ucI++)
		if(aucBuf[ucI] == '.')
			break;
	memset(&aucBuf[ucI],' ',PARAM_HOSTNUMBERLEN+uiOffset-ucI);
	uiOffset+=PARAM_HOSTNUMBERLEN;
	memcpy(&aucBuf[uiOffset],"16",2);
	uiOffset+=2;
	memcpy(&aucBuf[uiOffset],DataSave0.ConstantParamData.aucHostTelNumber3,
						PARAM_HOSTNUMBERLEN);
	for(ucI=uiOffset;ucI<uiOffset+PARAM_HOSTNUMBERLEN;ucI++)
		if(aucBuf[ucI] == '.')
			break;
	memset(&aucBuf[ucI],' ',PARAM_HOSTNUMBERLEN+uiOffset-ucI);
	uiOffset+=PARAM_HOSTNUMBERLEN;
	memcpy(&aucBuf[uiOffset],"17",2);
	uiOffset+=2;
	memcpy(&aucBuf[uiOffset],DataSave0.ConstantParamData.aucHostTelNumber4,
						PARAM_HOSTNUMBERLEN);
	for(ucI=uiOffset;ucI<uiOffset+PARAM_HOSTNUMBERLEN;ucI++)
		if(aucBuf[ucI] == '.')
			break;
	memset(&aucBuf[ucI],' ',PARAM_HOSTNUMBERLEN+uiOffset-ucI);
	uiOffset+=PARAM_HOSTNUMBERLEN;
	memcpy(&aucBuf[uiOffset],"18",2);
	uiOffset+=2;
	ucChar = DataSave0.ConstantParamData.ucXorTipFee+0x30;
	aucBuf[uiOffset] = ucChar;
	uiOffset++;
	memcpy(&aucBuf[uiOffset],"19",2);
	uiOffset+=2;
	uiTempShort = DataSave0.ConstantParamData.usTipFeePre;
	short_asc(&aucBuf[uiOffset],2,&uiTempShort);
	uiOffset+=2;
	memcpy(&aucBuf[uiOffset],"20",2);
	uiOffset+=2;
	ucChar = DataSave0.ConstantParamData.ucInputCardNo+0x30;
	aucBuf[uiOffset] = ucChar;
	uiOffset++;
	memcpy(&aucBuf[uiOffset],"21",2);
	uiOffset+=2;
	ucChar = DataSave0.ConstantParamData.ucSelfLogoff;
	aucBuf[uiOffset] = ucChar;
	uiOffset++;
	memcpy(&aucBuf[uiOffset],"23",2);
	uiOffset+=2;
	ucChar = DataSave0.ConstantParamData.ucTransRetry+0x30;
	aucBuf[uiOffset] = ucChar;
	uiOffset++;
	memcpy(&aucBuf[uiOffset],"24",2);
	uiOffset+=2;
	ucChar = DataSave0.ConstantParamData.ucOFFLineSendType;
	aucBuf[uiOffset] = ucChar;
	uiOffset++;
	memcpy(&aucBuf[uiOffset],"25",2);
	uiOffset+=2;
	ucChar = DataSave0.ConstantParamData.ucUseKeyIndex+'0';
	aucBuf[uiOffset] = ucChar;
	uiOffset++;
	memcpy(&aucBuf[uiOffset],"51",2);
	uiOffset+=2;
//	memcpy(&aucBuf[uiOffset],DataSave0.ConstantParamData.aucPOSDialConnect,24);
	//memcpy(&aucBuf[uiOffset],"010000888888",12);
	memcpy(&aucBuf[uiOffset],"000000000000",12);
	long_asc( &aucBuf[uiOffset],4,&DataSave0.ChangeParamData.ulDialTimeTotal );
	//uiOffset+=4;
//	if( DataSave0.ChangeParamData.ulDialTotalCount!=0 )
	{
		long_asc( &aucBuf[uiOffset+4],5,&DataSave0.ChangeParamData.ulDialTotalCount );
		long_asc( &aucBuf[uiOffset+4+5],3,&DataSave0.ChangeParamData.ulDialConnectRate );
	}
	uiOffset+=12;
	memcpy(Field,aucBuf,uiOffset);
	*iLength = uiOffset;
	return;
}

unsigned char LOGON_Dispatch_Month(unsigned int uiYear,unsigned char *pucMonth)
{
	unsigned char ucMonth;
	unsigned char ucAddDay;
	unsigned char ucDay;

	ucMonth = *pucMonth;
	ucDay = *(pucMonth+1);
	if(ucMonth > 1)
		ucMonth = ucMonth-1;
	else
		ucMonth = 12;
	switch(ucMonth)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			ucAddDay = (ucDay+31);
			ucDay = (ucDay+31)-1;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			ucAddDay = (ucDay+30);
			ucDay = (ucDay+30)-1;
			break;
		case 2:
			if((uiYear%400 == 0)||((uiYear%4 == 0)&&(uiYear%100)))
			{
				ucAddDay = (ucDay+29);
				ucDay = (ucDay+29)-1;
			}else
			{
				ucAddDay = (ucDay+28);
				ucDay = (ucDay+28)-1;
			}
			break;
	}
	*pucMonth = ucMonth;
	*(pucMonth+1) = ucDay;
	return(ucAddDay);
}

unsigned char LOGON_Dispatch_DateTime(unsigned char *pucDate)
{
	unsigned char aucDate[9],aucTime[9];
	unsigned char aucTermDate[9];
	unsigned int  uiYear;
	unsigned long ulConv;

	Os__read_date(aucDate);
	Os__read_time(aucTime);

	aucTermDate[0] = (unsigned char)asc_long(&aucDate[2],2);
	aucTermDate[1] = (unsigned char)asc_long(&aucDate[0],2);
	aucTermDate[2] = (unsigned char)asc_long(&aucTime[0],2);
	aucTermDate[3] = (unsigned char)asc_long(&aucTime[2],2);

	memcpy(&aucDate[2],"20",2);
	uiYear = asc_long(&aucDate[2],4);

	memset(aucDate,0,sizeof(aucDate));
	memset(aucTime,0,sizeof(aucTime));
	util_Printf("\nLOGON_Dispatch_DateTime");
	{
		unsigned char ucI;
		util_Printf("\nDataSave0.ConstantParamData.aucTermDate ");
		for(ucI=0;ucI<4;ucI++)
			util_Printf("%02x ",DataSave0.ConstantParamData.aucTermDate[ucI]);

	}
	aucDate[0] = (unsigned char)bcd_long(&DataSave0.ConstantParamData.aucTermDate[0],2);
	aucDate[1] = (unsigned char)bcd_long(&DataSave0.ConstantParamData.aucTermDate[1],2);
	aucDate[2] = (unsigned char)bcd_long(&DataSave0.ConstantParamData.aucTermDate[2],2);
	aucDate[3] = (unsigned char)bcd_long(&DataSave0.ConstantParamData.aucTermDate[3],2);


	/*����ֵĲ�*/
	if(aucTermDate[3] >= aucDate[3])
		aucTime[3] = aucTermDate[3] - aucDate[3];
	else
	{
		aucTime[3] = (aucTermDate[3]+60) - aucDate[3];
		if(aucTermDate[2]>0)
			aucTermDate[2] = aucTermDate[2] - 1;
		else
		{
			aucTermDate[2] = (aucTermDate[2]+24) - 1;
			if(aucTermDate[1]>1)
				aucTermDate[1] = aucTermDate[1] - 1;
			else
			{
				/*�����*/
				memcpy(aucDate,aucTermDate,4);
				LOGON_Dispatch_Month(uiYear,aucTermDate);

			}
		}
	}
	/*����ʱ�Ĳ�*/
	if(aucTermDate[2] >= aucDate[2])
		aucTime[2] = aucTermDate[2] - aucDate[2];
	else
	{
		aucTime[2] = (aucTermDate[2]+24) - aucDate[2];
		if(aucTermDate[1]>1)
			aucTermDate[1] = aucTermDate[1] - 1;
		else
		{
			/*�����*/
			LOGON_Dispatch_Month(uiYear,aucTermDate);
		}
	}
	/*�����յĲ�*/
	if(aucTermDate[1] >= aucDate[1])
		aucTime[1] = aucTermDate[1] - aucDate[1];
	else
	{
		/*�����*/
		aucTime[1] =  LOGON_Dispatch_Month(uiYear,aucTermDate) - aucDate[1];
	}
	if(aucTermDate[0] >= aucDate[0])
		aucTime[0] = aucTermDate[0] - aucDate[0];
	else
		aucTime[0] = (aucTermDate[0]+12) - aucDate[0];

	memset(aucTermDate,0,sizeof(aucTermDate));
	{
		unsigned char ucI;
		util_Printf("\nDataSave0.ConstantParamData.aucRecvDate ");
		for(ucI=0;ucI<4;ucI++)
			util_Printf("%02x ",DataSave0.ConstantParamData.aucRecvDate[ucI]);

	}
	aucTermDate[0] = (unsigned char)bcd_long(&DataSave0.ConstantParamData.aucRecvDate[0],2);
	aucTermDate[1] = (unsigned char)bcd_long(&DataSave0.ConstantParamData.aucRecvDate[1],2);
	aucTermDate[2] = (unsigned char)bcd_long(&DataSave0.ConstantParamData.aucRecvDate[2],2);
	aucTermDate[3] = (unsigned char)bcd_long(&DataSave0.ConstantParamData.aucRecvDate[3],2);
	{
		int i;
		util_Printf("\n---a01--aucTermDate = ");
		for(i=0;i<4;i++)
			util_Printf("%02x ",aucTermDate[i]);
		util_Printf("\n");
	}
	if((aucTermDate[3]+aucTime[3])<60)
		aucTermDate[3] = aucTermDate[3]+aucTime[3];
	else
	{
		aucTermDate[3] = (aucTermDate[3]+aucTime[3])%60;
		aucTermDate[2] = aucTermDate[2]+1;
	}
	if((aucTermDate[2]+aucTime[2])<24)
		aucTermDate[2] = aucTermDate[2]+aucTime[2];
	else
	{
		aucTermDate[2] = (aucTermDate[2]+aucTime[2])%24;
		aucTermDate[1] = aucTermDate[1]+1;
	}
	{
		int i;
		util_Printf("\n---a02--aucTermDate = ");
		for(i=0;i<4;i++)
			util_Printf("%02x ",aucTermDate[i]);
		util_Printf("\n");
	}
	switch(aucTermDate[0])
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			if((aucTermDate[1]+aucTime[1])<=31)
				aucTermDate[1] = aucTermDate[1]+aucTime[1];
			else
			{
				aucTermDate[1] = (aucTermDate[1]+aucTime[1])%31;
				aucTermDate[0] = aucTermDate[0]+1;
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			if((aucTermDate[1]+aucTime[1])<=30)
				aucTermDate[1] = aucTermDate[1]+aucTime[1];
			else
			{
				aucTermDate[1] = (aucTermDate[1]+aucTime[1])%30;
				aucTermDate[0] = aucTermDate[0]+1;
			}
			break;
		case 2:
			if((uiYear%400 == 0)||((uiYear%4 == 0)&&(uiYear%100)))
			{
				if((aucTermDate[0]+aucTime[0])<=29)
					aucTermDate[0] = aucTermDate[0]+aucTime[0];
				else
				{
					aucTermDate[0] = (aucTermDate[0]+aucTime[0])%29;
					aucTermDate[0] = aucTermDate[0]+1;
				}
			}else
			{
				if((aucTermDate[0]+aucTime[0])<=28)
					aucTermDate[0] = aucTermDate[0]+aucTime[0];
				else
				{
					aucTermDate[0] = (aucTermDate[0]+aucTime[0])%28;
					aucTermDate[0] = aucTermDate[0]+1;
				}
			}
		default:
			break;
	}
	{
		int i;
		util_Printf("\n---a03--aucTermDate = ");
		for(i=0;i<4;i++)
			util_Printf("%02x ",aucTermDate[i]);
		util_Printf("\n");
	}
	aucTermDate[0] = aucTermDate[0]%12;
	if(aucTermDate[0] == 0)
		aucTermDate[0] = 12 ;
	{
		int i;
		util_Printf("\n---a04--aucTermDate = ");
		for(i=0;i<4;i++)
			util_Printf("%02x ",aucTermDate[i]);
		util_Printf("\n");
	}
	memset(aucTime,0,sizeof(aucTime));
	ulConv = aucTermDate[0];
	long_bcd(&aucTime[0],1,&ulConv);

	ulConv = aucTermDate[1];
	long_bcd(&aucTime[1],1,&ulConv);

	ulConv = aucTermDate[2];
	long_bcd(&aucTime[2],1,&ulConv);

	ulConv = aucTermDate[3];
	long_bcd(&aucTime[3],1,&ulConv);
	memcpy(pucDate,aucTime,4);
	{
		int i;
		util_Printf("\n---a11--pucDate = ");
		for(i=0;i<9;i++)
			util_Printf("%02x ",aucTime[i]);
		util_Printf("\n");
	}
	return SUCCESS;
}
