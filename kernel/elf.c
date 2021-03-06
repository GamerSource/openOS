#include "include/elf.h"

uint8_t init_elf(void* image)
{
    /*
     * FIXME Wir muessen eigentlich die Laenge vom Image pruefen, damit wir bei
     * korrupten ELF-Dateien nicht ueber das Dateiende hinauslesen.
     */

    struct elf_header* header = image;
    struct elf_program_header* ph;
    int i;

    /* Ist es ueberhaupt eine ELF-Datei? */
    if (header->magic != ELF_MAGIC) {
        kprintf("Keine gueltige ELF-Magic!\n");
        return NO_ELF;
    }

    /*
     * Alle Program Header durchgehen und den Speicher an die passende Stelle
     * kopieren.
     *
     * FIXME Wir erlauben der ELF-Datei hier, jeden beliebigen Speicher zu
     * ueberschreiben, einschliesslich dem Kernel selbst.
     */
    ph = (struct elf_program_header*) (((char*) image) + header->ph_offset);
    for (i = 0; i < header->ph_entry_count; i++, ph++) {
        void* dest = (void*) ph->virt_addr;
        void* src = ((char*) image) + ph->offset;

        /* Nur Program Header vom Typ LOAD laden */
        if (ph->type != 1) {
            continue;
        }

        memset(dest, 0, ph->mem_size);
        memcpy(dest, src, ph->file_size);
    }

    init_task((void*) header->entry);
	return ELF_TASK_LOADED;
}