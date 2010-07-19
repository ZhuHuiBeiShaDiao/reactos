<?xml version="1.0"?>

<if property="ARCH" value="i386">
	<property name="BASEADDRESS_FREELDR" value="0x8000" />
<endif>

<!DOCTYPE group SYSTEM "../../../tools/rbuild/project.dtd">
<group>
	<if property="ARCH" value="i386">
		<module name="freeldr" type="bootloader" baseaddress="$(BASEADDRESS_FREELDR)">
			<linkerscript>freeldr_$(ARCH).lnk</linkerscript>
			<bootstrap installbase="loader" />
			<library>freeldr_startup</library>
			<library>freeldr_base64k</library>
			<library>freeldr_base</library>
			<library>mini_hal</library>
			<library>freeldr_arch</library>
			<library>freeldr_main</library>
			<library>rossym</library>
			<library>cmlib</library>
			<library>rtl</library>
			<library>libcntpr</library>
			<group linkerset="ld">
				<!-- linkerflag>-static</linkerflag -->
				<linkerflag>-nostartfiles</linkerflag>

			</group>
		</module>
	</if>
	<if property="ARCH" value="arm">
		<module name="freeldr" type="bootloader" installbase=".." installname="freeldr.sys">
			<bootstrap installbase="loader" />
			<library>freeldr_arch</library>
			<library>freeldr_startup</library>
			<library>freeldr_base64k</library>
			<library>freeldr_base</library>
			<library>freeldr_main</library>
			<library>rossym</library>
			<library>cmlib</library>
			<library>rtl</library>
			<library>libcntpr</library>
			<group linkerset="ld">
			    <linkerflag>-static</linkerflag>
				<linkerflag>-lgcc</linkerflag>
			    <if property="SARCH" value="omap3">
			        <linkerflag>-Wl,--image-base=0x80FFF000</linkerflag>
			    </if>
                <if property="SARCH" value="versatile">
			        <linkerflag>-Wl,--image-base=0x0001F000</linkerflag>
			    </if>				
			</group>
		</module>
	</if>
	<if property="ARCH" value="powerpc">
		<module name="ofwldr" type="elfexecutable" buildtype="OFWLDR">
			<library>freeldr_startup</library>
			<library>freeldr_base64k</library>
			<library>freeldr_base</library>
			<library>freeldr_arch</library>
			<library>freeldr_main</library>
			<library>rossym</library>
			<library>cmlib</library>
			<library>rtl</library>
			<library>libcntpr</library>
			<library>ppcmmu</library>
		</module>
	</if>
	<if property="ARCH" value="amd64">
		<module name="freeldr" type="bootloader">
			<bootstrap installbase="loader" />
			<library>freeldr_startup</library>
			<library>freeldr_base64k</library>
			<library>freeldr_base</library>
			<library>freeldr_arch</library>
			<library>freeldr_main</library>
			<library>rossym</library>
			<library>cmlib</library>
			<library>rtl</library>
			<library>libcntpr</library>
			<group linkerset="ld">
				<linkerflag>-static</linkerflag>
				<linkerflag>-lgcc</linkerflag>
			</group>
		</module>
	</if>
</group>
