#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef	enum	{
	Class_none	= 0,
	Class_lower	= (1 << 0),
	Class_upper	= (1 << 1)
} classes_t;

static	unsigned	nonfatal;
static	char *		ofile;

static	int
compar(
	const void *	l,
	const void *	r
)
{
	unsigned char const *	lc = l;
	unsigned char const *	rc = r;

	return( strncmp( lc, rc, 1 ) );
}

static	void
process(
	FILE *		fyle
)
{
	while( !feof( fyle ) )	{
		unsigned char	buf[ BUFSIZ + 1 ];
		char *		bp;
		int		c;

		if( !fgets( (char *) buf, sizeof( buf ) - 1, fyle ) )	{
			/* EOF						 */
			break;
		}
		buf[ BUFSIZ ] = '\0';
		bp = buf;
		for( c = *bp; c; )	{
			classes_t	seen;
			unsigned char *	bow;
			unsigned char *	eow;

			seen = Class_none;
			while( c && !isalpha( c ) )	{
				c = *++bp;
			}
			if( !c )	{
				break;
			}
			/* Find boundaries of the word			 */
			bow = bp;
			while( c && isalpha( c ) )	{
				if( islower( c ) )	{
					seen |= Class_lower;
				} else if( isupper( c ) )	{
					seen |= Class_upper;
					*bp = tolower( c );
				}
				c = *++bp;
			}
			eow = bp;
			/* Sort (all lowercase) letters			 */
			qsort( bow, eow - bow, sizeof( *bp ), compar );
			/* Restore original shape			 */
			if( seen == Class_upper )	{
				/* Was originally all upper case	 */
				while( bow < eow )	{
					*bow = toupper( *bow );
					++bow;
				}
			} else if( seen == (Class_upper|Class_lower) )	{
				/* Was originally mixed case		 */
				*bow = toupper( *bow );
			}
		}
		fputs( buf, stdout );
	}
}

int
main(
	int		argc,
	char * *	argv
)
{
	int		c;

	while( (c = getopt( argc, argv, "o:" )) != EOF )	{
		switch( c )	{
		default:
			fprintf( stderr, "huh?\n" );
			exit( 1 );
		case 'o':
			ofile = optarg;
			break;
		}
	}
	if( ofile )	{
		(void) unlink( ofile );
		if( freopen( ofile, "wt", stdout ) != stdout )	{
			fprintf( stderr, "cannot write '%s'.\n", optarg );
			exit( 1 );
		}
	}
	if( optind < argc )	{
		while( optind < argc )	{
			FILE *	fyle;

			optarg = argv[ optind++ ];
			fyle = fopen( optarg, "rt" );
			if( fyle )	{
				process( fyle );
				(void) fclose( fyle );
			} else	{
				fprintf( stderr, "cannot read '%s'.\n", optarg );
				++nonfatal;
			}
		}
	} else	{
		process( stdin );
	}
	return( nonfatal ? 1 : 0 );
}
