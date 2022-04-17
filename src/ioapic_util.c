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

//Source for most of this shit: https://pdos.csail.mit.edu/6.828/2018/readings/ia32/ioapic.pdf thanks MIT

static phys_addr_t io_apic_addr = 0xFEC00000; //this was retrieved from an acpi table dump with acpidump -> iasl fun.

static void __iomem* io_apic_base; //this is set with an ioremap of io_apic_addr

//Note the above address is a physical address so it needs to be remapped. 



//a bunch of print utility functions

static char* get_delivery_mode(unsigned long delmod){
    switch(delmod){
        case 0: 
            return "Fixed";
        case 1:
            return "Lowest Priority";
        case 2:
            return "SMI";
        case 3:
            return "Reserved";
        case 4:
            return "NMI";
        case 5:
            return "INIT";
        case 6:
            return "Reserved";
         default:
            return "ExtINT";
    }
}

static void pr_redir_info(unsigned long redir_val, int redir_num){
    pr_info("Redirection Number: %d\n", redir_num);
    pr_info("Raw Value: 0x%lx", redir_val);
    unsigned long masked = redir_val >> 16 & 0x1;
    pr_info("\tInterrupt Vector: 0x%lx\n", redir_val & 0xff);
    pr_info("\tDelivery Mode - Value: 0x%lx, Meaning: %s\n", (redir_val >> 8) & 0x7, get_delivery_mode((redir_val >> 8) & 0x7 ));
    unsigned long destination_mode = (redir_val >> 11) & 0x1;
    pr_info("\tDestination Mode - Value: 0x%lx, Meaning: %s\n", destination_mode, destination_mode == 0 ? "Physical Mode" : "Logical Mode");
    pr_info("\tInterrupt Polarity - Value: 0x%lx, Meaning: %s\n", redir_val >> 13 & 0x1, (redir_val >> 13 & 0x1) == 0 ? "High Active" : "Low Active");
    pr_info("\tTrigger Mode - Value: 0x%lx, Meaning: %s\n", redir_val >> 15 & 0x1, (redir_val >> 15 & 0x1) == 0 ? "Edge Sensitive" : "Level Sensitive");
    pr_info("\tMasked - Value: 0x%lx, Meaning: %s\n", masked, masked == 0 ? "Unmasked" : "Masked");
    if(destination_mode == 0){
        pr_info("\tDestination APIC ID: 0x%lx\n", redir_val >> 56);
    }else{
        pr_info("\tDestination Set of Processors: 0x%lx\n", redir_val >> 56);
    }
}


static unsigned int read_io_apic_reg(char offset){
    writel((unsigned int)offset, io_apic_base + IOREGSEL);
    barrier(); //we actually need a barrier here because later loads can be reordered with earlier stores to different locations. (Intel Vol 3A 8-9)
    //also I know writel has a barrier, but I'm going to keep the barrier here out of spite to make me feel smart. 
    return readl(io_apic_base + IOWIN); //read from IOWIN to get the actual register data
}

int ioapic_util_init(void)
{
    int i;
    io_apic_base = ioremap(io_apic_addr, 0xc); //its pretty important that we do this so that memory is not cached. 
    pr_info("IO-APIC ID: 0x%x\n", read_io_apic_reg(IOAPICID) >> 24 & 0xf);
    unsigned int version = read_io_apic_reg(IOAPICVER);
    pr_info("IO-APIC Version Number: 0x%x\n", version & 0xff);
    unsigned int redir_entries = (version >> 16) & 0xff;
    pr_info("Num Redirection Entires: 0x%x\n",redir_entries);

    for(i = 0; i<=redir_entries; i++){ //its <= because redir_entries is the actual number of redirection entries-1;
        unsigned long redir = read_io_apic_reg(IOAPICREDTBL(i)); 
        redir |= ((unsigned long)read_io_apic_reg(IOAPICREDTBL(i)+1) << 32); //get the upper 32 bits
        pr_redir_info(redir, i);       
    }



    
    


  
 

  return 0;
}
void ioapic_util_cleanup(void)
{


    iounmap(io_apic_base); 

	pr_info("Unloading IO-APIC Util\n");
}


MODULE_AUTHOR("mineo333");
MODULE_DESCRIPTION("Fucking around with the ioapic");

module_init(ioapic_util_init);
module_exit(ioapic_util_cleanup);
MODULE_LICENSE("GPL");
  