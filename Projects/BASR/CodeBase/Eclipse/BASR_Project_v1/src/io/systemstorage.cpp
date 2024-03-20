/*
 * systemstorage.cpp
 *
 *  Created on: Nov 2, 2018
 *      Author: hassan
 */

#define SYSTEMSTORAGE_CPP_

#include "../io/systemstorage.h"


void mountUsbDrive() {
	int32_t systemReturn = 0;

	systemReturn = system("mkdir /tmp/usb");
	systemReturn = system("mount /dev/sda1 /tmp/usb");

	//this needs to be revised
	if(systemReturn > 10) {
		cout << "Execution Error: " << systemReturn << endl;
	}
}

void mountSsdDrive() {
	int32_t systemReturn = 0;

	systemReturn = system("mkdir /tmp/ssd");
	systemReturn = system("mount /dev/sda1 /tmp/ssd");

	//this needs to be revised
	if(systemReturn > 10) {
		cout << "Execution Error: " << systemReturn << endl;
	}
}

void copyActualTlToEmmc() {
	int32_t systemReturn = 0;

	systemReturn = chmod("/mnt/dtm_tl", ACCESSPERMS);
	systemReturn = system("cp /home/dtm/dtm_tl/library.tle /mnt/dtm_tl/");

	//this needs to be revised
	if(systemReturn > 10) {
		cout << "Execution Error: " << systemReturn << endl;
	}
}

void copyDummyTlFromEmmc() {
	int32_t systemReturn = 0;

	systemReturn = chmod("/home/dtm/dtm_tl", ACCESSPERMS);
	systemReturn = system("cp /mnt/emmc/library.tle /home/dtm/dtm_tl/");

	//this needs to be revised
	if(systemReturn > 10) {
		cout << "Execution Error: " << systemReturn << endl;
	}
}

void formatTlPartition() {
	int32_t systemReturn = 0;

	systemReturn = system("umount /dev/mmcblk0p2");

	systemReturn = system("echo y | mkfs.ext4 /dev/mmcblk0p2");

	//this needs to be revised
	if(systemReturn > 10) {
		cout << "Execution Error: " << systemReturn << endl;
	}

	systemReturn = mount("/dev/mmcblk0p2", "/mnt/dtm_tl", "ext4", MS_NOEXEC, NULL);
}

void formatFrPartition() {
	int32_t systemReturn = 0;

	systemReturn = system("umount /dev/mmcblk0p3");

	systemReturn = system("echo y | mkfs.ext4 /dev/mmcblk0p3");

	//this needs to be revised
	if(systemReturn > 10) {
		cout << "Execution Error: " << systemReturn << endl;
	}

	systemReturn = mount("/dev/mmcblk0p3", "/home/dtm/dtm_fr", "ext4", MS_NOEXEC, NULL);
}
