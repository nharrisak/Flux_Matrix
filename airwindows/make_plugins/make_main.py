
import os

path = '../airwindows/plugins/MacAU'
files = os.listdir( path )
files = [ f for f in files if not os.path.isfile( os.path.join( path, f ) ) ]
files.sort()

try:
	os.mkdir( 'int' )
except FileExistsError:
	pass

for f in files:
	n = f;
	if f == 'Point':
		n = 'Poynt'
	with open( f'int/{f}.cpp', 'w' ) as F:
		F.write( '#include <iostream>\n' )
		F.write( f'#include "{path}/{f}/{n}.cpp"\n' )
		
		F.write( 'int main()\n' )
		F.write( '{\n' )
		F.write( '\tAudioUnit component;\n' )
		
		F.write( '\t{\n' )
		F.write( f'\t\t{n} p( component );\n' )
		F.write( '\t\tp.dumpParameters();\n' )
		F.write( '\t}\n' )
		
		F.write( '\treturn 0;\n' )
		F.write( '}\n' )
