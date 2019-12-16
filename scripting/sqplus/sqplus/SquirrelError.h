#ifndef SQUIRRELERROR_H
#define SQUIRRELERROR_H

/*struct SquirrelError {
    SquirrelError();
    SquirrelError(const SQChar* s):desc(s){}
    const SQChar *desc;
};*/


#ifndef scstrncpy 
  #ifdef SQUNICODE
    #define scstrncpy wcsncpy
  #else
    #define scstrncpy strncpy
  #endif 
#endif

// SquirrelError class that allows for merging two error strings.
#define SQ_MAX_ERR_LEN 200 
struct SquirrelError {
    // This ctor uses sq_getlasterror 
	SquirrelError(); 
    
    // ctor for const string
	SquirrelError( const SQChar *_desc) : desc(_desc) { } 
    
    // ctor for merging an error string with a param
    SquirrelError(const SQChar* s, const SQChar* s2) : desc(desc_buf) { 
		if( !s ) { desc_buf[0]=0; return; }
		int l1 = (int)scstrlen(s);
		int l2 = int( s2 ? scstrlen(s2) : 0 );
		if( l1+l2>=SQ_MAX_ERR_LEN ){
			if( l1>=SQ_MAX_ERR_LEN ){
				l1 = SQ_MAX_ERR_LEN-1;
				l2 = 0;
			}
			else {
				l2 = SQ_MAX_ERR_LEN-l1-1;
			}
		}
		scstrncpy(desc_buf,s,l1);
		scstrncpy(desc_buf+l1,s2,l2);
		desc_buf[l1+l2] = 0;
	}
    
    const SQChar *desc;
	SQChar desc_buf[SQ_MAX_ERR_LEN];
    
    operator const SQChar* () { return desc; } 
};
 
#endif // SQUIRRELERROR_H
