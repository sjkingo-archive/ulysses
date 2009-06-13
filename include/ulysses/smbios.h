#ifndef _ULYSSES_SMBIOS_H
#define _ULYSSES_SMBIOS_H

/* System Management BIOS (SMBIOS)
 *
 * See http://www.dmtf.org/standards/published_documents/DSP0134_2.6.1.pdf for
 * specification, and http://wiki.osdev.org/SMBIOS for more information.
 */

struct smbios_entry {
    char sm_str[4]; /* _SM_ */
    unsigned char checksum;
    unsigned char length;
    unsigned char ver_maj;
    unsigned char ver_min;
    unsigned short struct_size;
    unsigned char revision;
    char formatted_area[5];

    char dmi_str[5]; /* _DMI_ */
    unsigned char dmi_checksum;
    unsigned short table_len;
    unsigned int table_addr; /* address of first table */
    unsigned short table_count; /* number of tables */
    unsigned char bcd_revision;
};

struct smbios_table {
    unsigned char type;
    unsigned char length;
    unsigned short handle;
};

/* read_smbios()
 *  Locate and read the SMBIOS information.
 */
void read_smbios(void);

#endif
