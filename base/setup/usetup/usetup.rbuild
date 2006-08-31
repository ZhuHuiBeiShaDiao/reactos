<module name="usetup" type="nativecui" installbase="system32" installname="usetup.exe" allowwarnings="false">
	<bootstrap base="reactos/system32" nameoncd="smss.exe" />
	<include base="usetup">.</include>
	<include base="zlib">.</include>
	<include base="inflib">.</include>
	<include base="ReactOS">include/reactos/drivers</include>
	<define name="__USE_W32API" />
	<define name="_DISABLE_TIDENTS" />
	<define name="_WIN32_WINNT">0x0502</define>
	<define name="__NO_CTYPE_INLINES" />
	<linkerflag>-lgcc</linkerflag>
	<library>zlib</library>
	<library>inflib</library>
	<library>vfatlib</library>
	<library>ntdll</library>
	<!--pch>usetup.h</pch-->
	<compilationunit name="unit.c">
		<file>bootsup.c</file>
		<file>cabinet.c</file>
		<file>console.c</file>
		<file>drivesup.c</file>
		<file>filequeue.c</file>
		<file>filesup.c</file>
		<file>format.c</file>
		<file>fslist.c</file>
		<file>genlist.c</file>
		<file>inffile.c</file>
		<file>inicache.c</file>
		<file>keytrans.c</file>
		<file>partlist.c</file>
		<file>progress.c</file>
		<file>registry.c</file>
		<file>settings.c</file>
		<file>usetup.c</file>
	</compilationunit>
	<file>usetup.rc</file>
</module>
