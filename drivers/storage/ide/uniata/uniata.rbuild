<?xml version="1.0"?>
<!DOCTYPE module SYSTEM "../../../../tools/rbuild/project.dtd">
<module name="uniata" type="kernelmodedriver" installbase="system32/drivers" allowwarnings="true" installname="uniata.sys">
	<bootstrap base="$(CDOUTPUT)" />
	<define name="__USE_W32API" />
	<define name="_DEBUG" />
	<include base="uniata">.</include> 
	<include base="uniata">inc</include> 
	<library>ntoskrnl</library>
	<library>hal</library>
	<library>scsiport</library>
	<file>id_ata.cpp</file>
	<file>id_badblock.cpp</file>
	<file>id_dma.cpp</file>
	<file>id_init.cpp</file>
	<file>id_probe.cpp</file>
	<file>id_queue.cpp</file>
	<file>id_sata.cpp</file>
	<file>ros_glue.cpp</file>
	<file>stdafx.cpp</file>
	<file>idedma.rc</file>
</module>
