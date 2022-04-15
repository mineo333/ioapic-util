cmd_/home/mineo333/ioapic/modules.order := {   echo /home/mineo333/ioapic/ioapic_util.ko; :; } | awk '!x[$$0]++' - > /home/mineo333/ioapic/modules.order
