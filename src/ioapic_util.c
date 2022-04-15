#include "depend.h"


/*

Quick kernel module I made to study the IO-APIC in the Linux Kernel

*/
//these are offsets from the base.
#define IOREGSEL 0 //you mostly write to this one (It is r/w tho)
#define IOWIN 0x10 //you mostly read from this one (It is r/w tho)


#define IOAPICID          0x00
#define IOAPICVER         0x01
#define IOAPICARB         0x02
#define IOAPICREDTBL(n)   (0x10 + 2 * n) // io apic registers. These are written to IOREGSEL and are read from IOWIN
#define MAX_REDIR_ENTRIES 24

//Source for most of this shit: https://pdos.csail.mit.edu/6.828/2018/readings/ia32/ioapic.pdf thanks MIT

static phys_addr_t io_apic_addr = 0xFEC00000; //this was retrieved from an acpi table dump with acpidump -> iasl fun.

static void __iomem* io_apic_base; //this is set with an ioremap of io_apic_addr

//Note the above address is a physical address so it needs to be remapped. 

unsigned int read_io_apic_reg(char offset){
    writel((unsigned int)offset, io_apic_base + IOREGSEL);

    return readl(io_apic_base + IOREGSEL);
}


int ioapic_util_init(void)
{
    int i;
    io_apic_base = ioremap(io_apic_addr, 0xc); //its pretty important that we do this so that memory is not cached. 
    pr_info("Version: %x\n", read_io_apic_reg(IOAPICVER));
    for(i = 0; i<MAX_REDIR_ENTRIES; i++){
        unsigned long redir = read_io_apic_reg(IOAPICREDTBL(i)); 
        redir |= ((unsigned long)read_io_apic_reg(IOAPICREDTBL(i)+1) << 32); //get the upper 32 bits
        pr_info("redirecton entry %d: %lx\n", i, redir);

        
    }

    

    
    


  
 

  return 0;
}
void ioapic_util_cleanup(void)
{


    iounmap(io_apic_base); 

	pr_info("Unoading IO-APIC Util\n");
}


MODULE_AUTHOR("mineo333");
MODULE_DESCRIPTION("Fucking around with the ioapic");

module_init(ioapic_util_init);
module_exit(ioapic_util_cleanup);
MODULE_LICENSE("GPL");
  