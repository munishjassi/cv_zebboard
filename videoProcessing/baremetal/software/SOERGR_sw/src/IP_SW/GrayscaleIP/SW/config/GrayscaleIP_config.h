// configurations for using GrayscaleIP functionality in the system
// should be auto generated after GRIP has finished producing a SoC architecture

#ifndef __GRAYSCALEIP_CONFIG_H__
#define __GRAYSCALEIP_CONFIG_H__

// these actual details would later be filled in or overwritten by GRIP when a final design architecture is achieved
// for now assuming the SoC has 2 IP instances and both use grip_rule = 1 (vdma + IP solution)
#define GRAYSCALEIP_NUM_INSTANCES 1
#define GRAYSCALEIP_NUM_GRIPRULE1_INSTANCES 1
#define GRAYSCALEIP_NUM_GRIPRULE2_INSTANCES 0

typedef struct {
    unsigned int baseaddr;
    unsigned int grip_rule;
    unsigned int vdma_baseaddr;
    unsigned int intr_id;
} GRAYSCALEIP_IPinfo;


extern GRAYSCALEIP_IPinfo GRAYSCALEIP_INFO[GRAYSCALEIP_NUM_INSTANCES];

#endif













