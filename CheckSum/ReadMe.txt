CheckSum

Computes the hexidecimal checksum needed by the GF-880x clock.

The binary is provided in x64\Debug.

You can compute several checksums at once.  If your command has spaces in it, then quote it.

Here are comparisons against the manual.

C:\git\fusor-stuff\CheckSum> checksum $PERDCFG,UART1,115200 $PERDSYS,VERSION $PERDSYS,ANTSEL,FORCE1L
$PERDCFG,UART1,115200*65
$PERDSYS,VERSION*2C
$PERDSYS,ANTSEL,FORCE1L*7B
