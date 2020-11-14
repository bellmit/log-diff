#ifndef VTFFN_H_INCLUDED
#define VTFFN_H_INCLUDED

/**  2015.07 Dehua Tan **/

#define VT_STRINGIZE(X)     #X
#define VT_STR_VALUE(X)     VT_STRINGIZE(X)

#define __GOOD_FUNCTION__   __FUNCTION__
#define __GOOD_FILE__       vt_good_filename(__FILE__)

__BEGIN_DECLS

    const char* vt_good_funcname(const char* pretty_function_name);
    const char* vt_good_filename(const char* filepath);

__END_DECLS

#endif // VTFFN_H_INCLUDED
