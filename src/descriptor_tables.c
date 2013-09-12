/*
 * descriptor_tables.c - Initialises the GDT and IDT, and defines the default
 * ISR and IRQ handler.
 * Based on code from Bran's kernel development tutorials.
 * Rewritten for JamesM's kernel development tutorials.
 */
#include <common.h>
#include <descriptor_tables.h>

extern void	gdt_flush(uint32_t gdt_p);
static void	init_gdt(void);
static void	gdt_set_gate(uint32_t n, uint32_t base, uint32_t limit,
		    uint8_t access, uint8_t gran);

extern void	idt_flush(uint32_t idt_p);
static void	init_idt(void);
static void	idt_set_gate(uint32_t n, uint32_t base, uint16_t sel,
		    uint8_t flags);

static struct gdt_entry		gdt_entries[5];
static struct gdt_ptr		gdt_ptr;
static struct idt_entry		idt_entries[256];
static struct idt_ptr		idt_ptr;


/*
 * Initialisation routine - zeroes all the interrupt service routines,
 * initialises the GDT and IDT.
 */
void
init_descriptor_tables(void)
{

	init_gdt();
	init_idt();
}


static void
init_gdt(void)
{
	gdt_ptr.limit = sizeof(gdt_entries) - 1;
	gdt_ptr.base  = (uint32_t)&gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0);                /* Null segment */
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* Code segment */
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* Data segment */
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* User mode code segment */
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* User mode data segment */

	gdt_flush((uint32_t)&gdt_ptr);
}


/*
 * Set the value of one GDT entry.
 */
static void
gdt_set_gate(uint32_t n, uint32_t base, uint32_t limit, uint8_t access,
    uint8_t gran)
{
	if (n >= countof(gdt_entries))
		PANIC("bad gdt_entries index");

	gdt_entries[n].base_lo = (base & 0xFFFF);
	gdt_entries[n].base_mi = (base >> 16) & 0xFF;
	gdt_entries[n].base_hi = (base >> 24) & 0xFF;

	gdt_entries[n].limit_lo    = (limit & 0xFFFF);
	gdt_entries[n].granularity = (limit >> 16) & 0x0F;

	gdt_entries[n].granularity |= gran & 0xF0;
	gdt_entries[n].access       = access;
}


static void
init_idt(void)
{
	idt_ptr.limit = sizeof(idt_entries) - 1;
	idt_ptr.base  = (uint32_t)&idt_entries;

	idt_set_gate( 0,  (uint32_t)isr0, 0x08, 0x8E);
	idt_set_gate( 1,  (uint32_t)isr1, 0x08, 0x8E);
	idt_set_gate( 2,  (uint32_t)isr2, 0x08, 0x8E);
	idt_set_gate( 3,  (uint32_t)isr3, 0x08, 0x8E);
	idt_set_gate( 4,  (uint32_t)isr4, 0x08, 0x8E);
	idt_set_gate( 5,  (uint32_t)isr5, 0x08, 0x8E);
	idt_set_gate( 6,  (uint32_t)isr6, 0x08, 0x8E);
	idt_set_gate( 7,  (uint32_t)isr7, 0x08, 0x8E);
	idt_set_gate( 8,  (uint32_t)isr8, 0x08, 0x8E);
	idt_set_gate( 9,  (uint32_t)isr9, 0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

	idt_flush((uint32_t)&idt_ptr);
}


static void
idt_set_gate(uint32_t n, uint32_t base, uint16_t sel, uint8_t flags)
{
	if (n >= countof(idt_entries))
		PANIC("Bad idt_entries index");

	idt_entries[n].base_lo = base & 0xFFFF;
	idt_entries[n].base_hi = (base >> 16) & 0xFFFF;

	idt_entries[n].sel     = sel;
	idt_entries[n].always0 = 0;
	/* We must uncomment the OR below when we get to using user-mode. It
	  sets the interrupt gate's privilege level to 3. */
	idt_entries[n].flags   = flags /* | 0x60 */;
}
