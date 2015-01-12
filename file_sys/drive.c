#include "drive.h"

#include "hardware_config.h"

static void _void_handler() {
	return;
}

static void _init_irq() {
	static int initialized = 0;
	unsigned int i;

	if(initialized)
		return;

	for(i=0; i<16; i++) {
		IRQVECTOR[i] = _void_handler;
	}
}

static void _goto_sector(unsigned int cylinder, unsigned int sector) {
	check_hda();

	PRINT_ASSERT_ERROR_MSG(cylinder < NB_CYLINDER && sector < NB_SECTOR, "Unvalid cylinder or sector size");

	_out(HDA_DATAREGS, (cylinder >> 8) & 0xFF);
	_out(HDA_DATAREGS+1, cylinder & 0xFF);
	_out(HDA_DATAREGS+2, (sector >> 8) & 0xFF);
	_out(HDA_DATAREGS+3, sector & 0xFF);
	_out(HDA_CMDREG, CMD_SEEK);

	_sleep(HDA_IRQ);
}

void check_hda() {
	int sector_size = 0;
	int nb_cylinder = 0;
	int nb_sector = 0;

	_out(HDA_CMDREG, CMD_DSKINFO);

	nb_cylinder = (_in(HDA_DATAREGS) << 8)+_in(HDA_DATAREGS+1);
	if(nb_cylinder != NB_CYLINDER)
		PRINT_DEFINE_ERROR_VALUE(NB_CYLINDER, nb_cylinder);

	nb_sector = (_in(HDA_DATAREGS+2) << 8)+_in(HDA_DATAREGS+3);
	if(nb_sector != NB_SECTOR)
		PRINT_DEFINE_ERROR_VALUE(NB_SECTOR, nb_sector);

	sector_size = (_in(HDA_DATAREGS+4) << 8)+_in(HDA_DATAREGS+5);
	if(sector_size != SECTOR_SIZE)
		PRINT_DEFINE_ERROR_VALUE(SECTOR_SIZE, sector_size);

}

void read_sector(unsigned int cylinder, unsigned int sector, unsigned char* buffer) {
	check_hda();
	_init_irq();

	_goto_sector(cylinder, sector);

	_out(HDA_DATAREGS, 0);
	_out(HDA_DATAREGS+1, 1);
	_out(HDA_CMDREG, CMD_READ);
	_sleep(HDA_IRQ);

	memcpy(buffer, MASTERBUFFER, SECTOR_SIZE);
}

void read_sector_n(unsigned int cylinder, unsigned int sector, unsigned char* buffer, unsigned int n) {
	check_hda();
	_init_irq();


	PRINT_ASSERT_ERROR_MSG(n < SECTOR_SIZE, "Parameter n need to be inferior to sector size");

	_goto_sector(cylinder, sector);

	_out(HDA_DATAREGS, 0);
	_out(HDA_DATAREGS+1, 1);
	_out(HDA_CMDREG, CMD_READ);
	_sleep(HDA_IRQ);

	memcpy(buffer, MASTERBUFFER, n);
}

void write_sector(unsigned int cylinder, unsigned int sector, const unsigned char* buffer) {
	check_hda();
	_init_irq();


	_goto_sector(cylinder, sector);

	memcpy(MASTERBUFFER, buffer, SECTOR_SIZE);

	_out(HDA_DATAREGS, 0);
	_out(HDA_DATAREGS+1, 1);
	_out(HDA_CMDREG, CMD_WRITE);
	_sleep(HDA_IRQ);
}

void write_sector_n(unsigned int cylinder, unsigned int sector, const unsigned char* buffer, unsigned int n) {
	check_hda();
	_init_irq();

	PRINT_ASSERT_ERROR_MSG(n < SECTOR_SIZE, "Parameter n need to be inferior to sector size");

	_goto_sector(cylinder, sector);

	/*
	 *	Read sector
	 */
	_out(HDA_DATAREGS, 0);
	_out(HDA_DATAREGS+1, 1);
	_out(HDA_CMDREG, CMD_READ);
	_sleep(HDA_IRQ);

	memcpy(MASTERBUFFER, buffer, n);

	_out(HDA_DATAREGS, 0);
	_out(HDA_DATAREGS+1, 1);
	_out(HDA_CMDREG, CMD_WRITE);
	_sleep(HDA_IRQ);
}

void format_sector(unsigned int cylinder, unsigned int sector, unsigned int n_sector, unsigned int value) {
	check_hda();
	_init_irq();

	_goto_sector(cylinder, sector);

	_out(HDA_DATAREGS, (n_sector >> 8) & 0xFF);
	_out(HDA_DATAREGS+1, n_sector & 0xFF);
	_out(HDA_DATAREGS+2, (value >> 24) & 0xFF);
	_out(HDA_DATAREGS+3, (value >> 16) & 0xFF);
	_out(HDA_DATAREGS+4, (value >> 8) & 0xFF);
	_out(HDA_DATAREGS+5, value & 0xFF);
	_out(HDA_CMDREG, CMD_FORMAT);
	_sleep(HDA_IRQ);
}
