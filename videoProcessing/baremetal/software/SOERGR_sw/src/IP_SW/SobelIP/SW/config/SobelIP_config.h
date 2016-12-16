// configurations for using SobelIP functionality in the system
// should be auto generated after GRIP has finished producing a SoC architecture

#ifndef __SOBELIP_CONFIG_H__
#define __SOBELIP_CONFIG_H__


// these actual details would later be filled in or overwritten by GRIP when a final design architecture is achieved
// for now assuming the SoC has 1 IP instance and both use grip_rule = 1 (vdma + IP solution)
#define SOBELIP_NUM_INSTANCES 1
#define SOBELIP_NUM_GRIPRULE1_INSTANCES 1

typedef struct {
    unsigned int baseaddr;
    unsigned int grip_rule;
    unsigned int vdma_baseaddr;
	unsigned int intr_id;
} SOBELIP_IPinfo;


extern SOBELIP_IPinfo SOBELIP_INFO[SOBELIP_NUM_INSTANCES];

#endif













