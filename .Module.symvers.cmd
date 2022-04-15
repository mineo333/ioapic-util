cmd_/home/mineo333/ioapic/Module.symvers := sed 's/\.ko$$/\.o/' /home/mineo333/ioapic/modules.order | scripts/mod/modpost -m -a  -o /home/mineo333/ioapic/Module.symvers -e -i Module.symvers   -T -
