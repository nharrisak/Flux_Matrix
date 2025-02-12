import os, sys, re

def process( text ):
	d = {}
	for t in text:
		s = re.split( r'.*\"(\w+)\".+\"(.+)\"', t )
		if len(s) == 4:
			d[ s[1] ] = s[2]
	text = "".join( text )
	if 'name' in d:
		text = text.replace( '.name = "faust",', '.name = "%s",' % d[ 'name' ] )
	if 'description' in d:
		text = text.replace( '.description = "faust",', '.description = "%s",' % d[ 'description' ] )
	if 'guid' in d:
		guid = d[ 'guid' ]
		if len(guid) == 4:
			text = text.replace( ".guid = NT_MULTICHAR( 'F', 'a', 'u', 's' ),", ".guid = NT_MULTICHAR( '%s', '%s', '%s', '%s' )," % ( guid[0], guid[1], guid[2], guid[3] ) )
	return text

def main():
	if len(sys.argv) != 2:
		print("Usage: python apply_metadata.py <.cpp file>")
		sys.exit(1)
	
	filename = sys.argv[1]
	if not os.path.exists(filename):
		print( f"Error: File {filename} not found" )
		sys.exit(1)
	
	with open( filename, 'r', encoding='utf-8' ) as f:
		text = f.readlines()
	
	text = process( text )
	
	with open( filename, 'w', encoding='utf-8' ) as f:
		f.write( text )

if __name__ == "__main__":
	main()
