/* Copyright (C) 2003-2013 Runtime Revolution Ltd.

This file is part of LiveCode.

LiveCode is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License v3 as published by the Free
Software Foundation.

LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

//
// Script class declarations
//
#ifndef	EXECPOINT_H
#define	EXECPOINT_H

#ifndef __MC_VARIABLE__
#include "variable.h"
#endif

#define EP_PAD 256
#define EP_MASK 0xFFFFFF00

#ifndef __MC_EXEC_CONTEXT__
#include "exec-context.h"
#endif

class MCExecPoint
{
	MCExecContext& ctxt;
	MCValueRef value;

public:
	MCExecPoint(MCExecContext& p_ctxt)
		: ctxt(p_ctxt)
	{
		value = MCValueRetain(kMCEmptyString);
	}
	
	MCExecPoint(MCExecPoint& p_other)
		: ctxt(p_other . ctxt)
	{
		value = MCValueRetain(kMCEmptyString);
	}
	
	~MCExecPoint(void)
	{
		MCValueRelease(value);
	}
	
	MCExecContext& GetContext(void)
	{
		return ctxt;
	}
	
	//////////
	
	MCValueRef getvalueref(void);
	bool setvalueref(MCValueRef value);
	
	bool copyasbool(bool& r_value);
	bool copyasint(integer_t& r_value);
	bool copyasuint(uinteger_t& r_value);
	bool copyasdouble(double& r_value);
	
	bool copyasvalueref(MCValueRef& r_value);
	bool copyasstringref(MCStringRef& r_value);
	bool copyasnameref(MCNameRef& r_value);
	bool copyasdataref(MCDataRef& r_value);
	bool copyasarrayref(MCArrayRef& r_value);
	
	bool copyaslegacypoint(MCPoint& r_point);
	bool copyaslegacyrectangle(MCRectangle& r_rectangle);
	bool copyaslegacycolor(MCColor& r_color);
	
	//////////
	
	void setempty(void);
	void setpoint(const MCPoint& point);
	void setboolean(Bool value);
	void setuint(uint4 value);
	void setint(int4 value);
	void setsvalue(const MCString& value);
	void setnvalue(double value);
	void setstaticcstring(const char *value);
	
	//////////
	
	bool isempty(void);
	bool isarray(void);
	
	MCString getsvalue(void);
	double getnvalue(void);
	
	int4 getint4(void);
	uint4 getuint4(void);
	int2 getint2(void);
	uint2 getuint2(void);
	
	//////////
	
	void clear(void);
	
	//////////
	
	bool ton(void);
	bool tos(void);
	bool tona(void);
	
	//////////
	
	void concatcstring(const char *str, Exec_concat sep, Boolean first);
	void concatmcstring(const MCString& str, Exec_concat sep, Boolean first);
	void concatstringref(MCStringRef str, Exec_concat sep, Boolean first);
	void concatnameref(MCNameRef name, Exec_concat sep, Boolean first);
	void concatuint(uint4 num, Exec_concat sep, Boolean first);
	
	//////////
	
#if 0
	MCExecPoint()
	{
		memset(this, 0, sizeof(MCExecPoint));
		itemdel = ',';
		columndel = '\t';
		linedel = '\n';
		rowdel = '\n';
		nffw = 8;
		nftrailing = 6;
		cutoff = 35;
		value = MCValueRetain(kMCEmptyString);
	}
	MCExecPoint(const MCExecPoint &ep)
	{
		*this = ep;
		value = MCValueRetain(kMCEmptyString);
	}
	MCExecPoint(MCObject *object, MCHandlerlist *hlist, MCHandler *handler)
	{
		memset(this, 0, sizeof(MCExecPoint));
		curobj = object;
		curhlist = hlist;
		curhandler = handler;
		itemdel = ',';
		columndel = '\t';
		linedel = '\n';
		rowdel = '\n';
		nffw = 8;
		nftrailing = 6;
		cutoff = 35;
		value = MCValueRetain(kMCEmptyString);
	}
	void restore(const MCExecPoint &ep)
	{
		curobj = ep.curobj;
		curhlist = ep.curhlist;
		curhandler = ep.curhandler;
	}
	~MCExecPoint()
	{
		MCValueRelease(value);
	}
	void sethlist(MCHandlerlist *hl)
	{
		curhlist = hl;
	}
	void sethandler(MCHandler *newhandler)
	{
		curhandler = newhandler;
	}

	MCObject *getobj() const
	{
		return curobj;
	}
	MCHandlerlist *gethlist() const
	{
		return curhlist;
	}
	MCHandler *gethandler() const
	{
		return curhandler;
	}
	uint2 getnffw() const
	{
		return nffw;
	}
	uint2 getnftrailing() const
	{
		return nftrailing;
	}
	uint2 getnfforce() const
	{
		return nfforce;
	}
	Boolean getcasesensitive() const
	{
		return casesensitive;
	}
	uint2 getcutoff() const
	{
		return cutoff;
	}
	Boolean getconvertoctals() const
	{
		return convertoctals;
	}
	char getitemdel() const
	{
		return itemdel;
	}
	char getcolumndel() const
	{
		return columndel;
	}
	char getrowdel() const
	{
		return rowdel;
	}
	char getlinedel()
	{
		return linedel;
	}
	Boolean getwholematches() const
	{
		return wholematches;
	}
	Boolean getusesystemdate() const
	{
		return usesystemdate;
	}
	Boolean getuseunicode() const
	{
		return useunicode;
	}
	Exec_stat setcasesensitive(uint2 line, uint2 pos)
	{
		return getboolean(casesensitive, line, pos, EE_PROPERTY_NAB);
	}
	Exec_stat setcutoff(uint2 line, uint2 pos)
	{
		return getuint2(cutoff, line, pos, EE_PROPERTY_NOTANUM);
	}
	Exec_stat setconvertoctals(uint2 line, uint2 pos)
	{
		return getboolean(convertoctals, line, pos, EE_PROPERTY_NAB);
	}
	Exec_stat setusesystemdate(uint2 line, uint2 pos)
	{
		return getboolean(usesystemdate, line, pos, EE_PROPERTY_NAB);
	}
	Exec_stat setuseunicode(uint2 line, uint2 pos)
	{
		return getboolean(useunicode, line, pos, EE_PROPERTY_NAB);
	}
	Exec_stat setitemdel(uint2 line, uint2 pos);
	Exec_stat setcolumndel(uint2 line, uint2 pos);
	Exec_stat setlinedel(uint2 line, uint2 pos);
	Exec_stat setrowdel(uint2 line, uint2 pos);
	Exec_stat setwholematches(uint2 line, uint2 pos)
	{
		return getboolean(wholematches, line, pos, EE_PROPERTY_NAB);
	}

	void setcasesensitive(bool p_value) {casesensitive = p_value;}
	void setconvertoctals(bool p_value) {convertoctals = p_value;}
	void setwholematches(bool p_value) {wholematches = p_value;}
	void setuseunicode(bool p_value) {useunicode = p_value;}
	void setusesystemdate(bool p_value) {usesystemdate = p_value;}
	void setcutoff(uint2 p_value) {cutoff = p_value;}
	void setitemdel(char_t p_value) {itemdel = p_value;}
	void setcolumndel(char_t p_value) {columndel = p_value;}
	void setlinedel(char_t p_value) {linedel = p_value;}
	void setrowdel(char_t p_value) {rowdel = p_value;}
	void setnumberformat(uint2 fw, uint2 trailing, uint2 force) {nffw = fw; nftrailing = trailing; nfforce = force;}

	void setobj(MCObject *p_object)
	{
		curobj = p_object;
	}

	void setline(uint2 l)
	{
		line = l;
	}
	uint2 getline()
	{
		return line;
	}

	MCParentScriptUse *getparentscript(void) const
	{
		return parentscript;
	}

	void setparentscript(MCParentScriptUse *obj)
	{
		parentscript = obj;
	}

	void setnumberformat();

	//////////

	// Returns true if the exec point contains the empty value.
	bool isempty(void) const;
	// Returns true if the exec point contains a (non-empty) array.
	bool isarray(void) const;
	// Returns true if the exec point contains a string.
	bool isstring(void) const;
	// Returns true if the exec point contains a number.
	bool isnumber(void) const;

	void clear(void);
	void setsvalue(const MCString& string);
	void copysvalue(const MCString& string);
	void copysvalue(const char *string, uindex_t length);
	void setnvalue(real8 number);
	void setnvalue(uinteger_t number);
	void setnvalue(integer_t number);
	void grabbuffer(char *buffer, uint32_t length);

	bool reserve(uindex_t capacity, char*& r_buffer);
	void commit(uindex_t size);

	bool modify(char*& r_buffer, uindex_t& r_length);
	void resize(uindex_t size);

	const char *getcstring(void);
	MCString getsvalue(void);
	MCString getsvalue0(void);
	real8 getnvalue();

	Exec_stat tos(void);
	Exec_stat ton(void);
	Exec_stat tona(void);

	uint4 getuint4();
	uint2 getuint2();
	int4 getint4();

	Exec_stat getreal8(real8 &, uint2, uint2, Exec_errors);
	Exec_stat getuint2(uint2 &, uint2, uint2, Exec_errors);
	Exec_stat getint4(int4 &, uint2, uint2, Exec_errors);
	Exec_stat getuint4(uint4 &, uint2, uint2, Exec_errors);
	Exec_stat getboolean(Boolean &, uint2, uint2, Exec_errors);

	void setint(int4);
	void setuint(uint4);
    void setint64(int64_t);
    void setuint64(uint64_t);
	void setr8(real8 n, uint2 fw, uint2 trailing, uint2 force);

	Exec_stat ntos();
	Exec_stat ston();
	void lower();
	void upper();

	void insert(const MCString &, uint4 s, uint4 e);
	uint1 *pad(char value, uint4 count);
	void substring(uint4 s, uint4 e);
	void tail(uint4 s);
	void fill(uint4 s, char c, uint4 n);
	void texttobinary();
	void binarytotext();
#endif

#if 0
	Boolean isempty(void) const
	{
		return format == VF_UNDEFINED || format != VF_ARRAY && format != VF_NUMBER && svalue . getlength() == 0;
	}
	
	Value_format getformat()
	{
		return format;
	}
	void setsvalue(const MCString &s)
	{
		svalue = s;
		format = VF_STRING;
	}
	void setnvalue(const real8 &n)
	{
		nvalue = n;
		format = VF_NUMBER;
	}
	void setnvalue(uint4 n)
	{
		nvalue = (real8)n;
		format = VF_NUMBER;
	}
	void setnvalue(int4 n)
	{
		nvalue = (real8)n;
		format = VF_NUMBER;
	}
	void setnvalue(uint2 n)
	{
		nvalue = (real8)n;
		format = VF_NUMBER;
	}
	void setnvalue(int2 n)
	{
		nvalue = (real8)n;
		format = VF_NUMBER;
	}
	void setboth(const MCString &s, const real8 &n)
	{
		svalue = s;
		nvalue = n;
		format = VF_BOTH;
	}
	void copysvalue(const char *s, uint4 l);
	void copysvalue(const char *s) {copysvalue(s, strlen(s));}
	// Ensure that there is a string value available and the exec-point owns it.e
	void grabsvalue();
	// Ensure that the execpoint owns its array (if it has one)
	void grabarray();
	// Ensure that the execpoint owns its value
	void grab(void);
	void clear();
	// Take the memory pointed to by buffer as the value, and set the execpoint to a string
	void grabbuffer(char *p_buffer, uint4 p_length);

	char *getbuffer(uint4 newsize);
	uint4 getbuffersize()
	{
		return size;
	}
	void setbuffer(char *s, uint4 l)
	{
		buffer = s;
		size = l;
	}
	Boolean changed()
	{
		return svalue.getstring() == buffer;
	}

	void setstrlen()
	{
		svalue.set(buffer, strlen(buffer));
		format = VF_STRING;
	}
	void setlength(uint4 l)
	{
		svalue.set(buffer, l);
		format = VF_STRING;
	}
	// MW-2007-07-03: [[ Bug 5123 ]] - Strict array checking modification
	Exec_stat tos()
	{
		return format == VF_NUMBER ? ntos() : (format != VF_ARRAY ? ES_NORMAL : ES_ERROR);
	}
	
	// MW-2007-07-03: [[ Bug 5123 ]] - Strict array checking modification
	Exec_stat ton()
	{
		return format == VF_STRING ? ston() : (format != VF_ARRAY ? ES_NORMAL : ES_ERROR);
	}
	
	// MW-2007-07-03: [[ Bug 5123 ]] - Strict array checking modification
	Exec_stat tona(void)
	{
		return format == VF_STRING ? ston() : ES_NORMAL;
	}

	// Ensure that the value in the ExecPoint is a NUL-terminated string and return
	// a pointer to it.
	const char *getcstring(void);

	// Ensure that the value in the ExecPoint is NUL-terminated then return it as
	// a MCString.
	const MCString& getsvalue0(void)
	{
		getcstring();
		return svalue;
	}

	const MCString &getsvalue()
	{
		if (format==VF_NUMBER)
			tos();
		return svalue;
	}
	real8 getnvalue() const
	{
		return nvalue;
	}
	uint4 getuint4() const
	{
		return (uint4)(nvalue < 0.0 ? 0 : nvalue + 0.5);
	}
	uint2 getuint2() const
	{
		return (uint2)(nvalue < 0.0 ? 0 : nvalue + 0.5);
	}
	int4 getint4() const
	{
		return (int4)(nvalue < 0.0?nvalue - 0.5:nvalue + 0.5);
	}
	Exec_stat ntos();
	Exec_stat ston();
	void lower();
	void upper();
	
	void tail(uint4 s);
	Boolean usingbuffer();
	void fill(uint4 s, char c, uint4 n);
	void texttobinary();
	void binarytotext();
	void parseURL(Url_type &urltype, const char *&hostname, const char *&rname, uint4 &port, const char *&auth);

#endif

#if 0
	void utf16toutf8(void);
	void utf8toutf16(void);

	void utf16tonative(void);
	void nativetoutf16(void);
	
	void utf8tonative(void);
	void nativetoutf8(void);

	// MW-2012-02-16: [[ IntrinsicUnicode ]] Maps the content to unicode or native
	//   depending on 'want_unicode'. Content is assumed to be in unicode if 'is_unicode'
	//   is true; otherwise it is assumed to be native.
	void mapunicode(bool is_unicode, bool want_unicode);
	
	// MW-2011-06-22: [[ SERVER ]] Provides augmented functionality for finding
	//   variables if there is no handler (i.e. global server scope).
	Parse_stat findvar(MCNameRef p_name, MCVarref** r_var);

	//////////

	void setstaticcstring(const char *string);
	void setstaticchars(const char *string, uindex_t length);
	void setstaticbytes(const void *bytes, uindex_t length);
	void setstaticmcstring(const MCString& string);

	void setempty(void);
	void setcstring(const char *string);
	void setmcstring(const MCString& string);
	void setstringf(const char *spec, ...);
	void setchars(const char *string, uindex_t length);
	void setchar(char character);
	void setbytes(const void *bytes, uindex_t length);
	void setbyte(uint8_t byte);
	void setbool(bool value);
	void setboolean(Boolean value);
	void setpoint(int16_t x, int16_t y);
	void setpoint(MCPoint point);
	void setrectangle(int32_t left, int32_t top, int32_t right, int32_t bottom);
	void setrectangle(const MCRectangle& rect);
	void setrectangle(const MCRectangle32& rect);
	void setcolor(uint32_t r, uint32_t g, uint32_t b);
	void setcolor(const MCColor& color);
	void setcolor(const MCColor& color, const char *colorname);
	void setpixel(uint32_t pixel);
	void setnewline(void);

	void appendcstring(const char *string);
	void appendmcstring(const MCString& string);
	void appendstringf(const char *spec, ...);
	void appendchars(const char *string, uindex_t length);
	void appendchar(char character);
	void appendunichars(const uint2 *string, uindex_t length);
	void appendunichar(uint2 character);
	void appendbytes(const void *bytes, uindex_t length);
	void appendbyte(uint8_t byte);
	void appenduint(uint32_t integer);
	void appendint(int32_t integer);
	void appendreal(double real);
	void appendnewline(void);
	void appendnewline(bool unicode);

	void concatcstring(const char *string, Exec_concat sep, bool first);
	void concatchars(const char *chars, uindex_t count, Exec_concat sep, bool first);
	void concatmcstring(const MCString& string, Exec_concat sep, bool first);
	void concatuint(uint32_t value, Exec_concat sep, bool first);
	void concatint(int32_t value, Exec_concat sep, bool first);
	void concatreal(double real, Exec_concat sep, bool first);

	void replacechar(char from, char to);

	void concatnameref(MCNameRef name, Exec_concat sep, bool first);
	bool copyasnameref(MCNameRef& r_name);

	void concatstringref(MCStringRef string, Exec_concat sep, bool first);
	bool copyasstringref(MCStringRef& r_string);
	bool copyasmutablestringref(MCStringRef& r_string);

	bool copyasdataref(MCDataRef& r_data);

	bool copyaslegacypoint(MCPoint& r_point);
	bool copyaslegacyrectangle(MCRectangle& r_rectangle);
	bool copyaslegacycolor(MCColor& r_color);

	//

	// Methods that work with MCObject::get/set*prop.
	bool copyasbool(bool& r_value);
	bool copyasint(integer_t& r_value);
	bool copyasuint(uinteger_t& r_value);
	bool copyasdouble(double& r_value);
	bool copyaschar(char_t& r_value);
	bool copyasnumber(MCNumberRef& r_value);
	bool copyasstring(MCStringRef& r_value);
	bool copyasarray(MCArrayRef& r_value);
	bool copyasvariant(MCValueRef& r_value);
	
	bool copyaspoint(MCPoint& r_value);
	bool copyasrect(MCRectangle& r_value);

	// Variant methods.

	// Set the ep to the contents of value.
	bool setvalueref(MCValueRef value);
	// Get the valueref from the ep.
	MCValueRef getvalueref(void);
	// Set the ep to the contents of value. If value is nil, then empty is used.
	bool setvalueref_nullable(MCValueRef value);
	// Make a copy of the contents of the ep as a value.
	bool copyasvalueref(MCValueRef& r_value);
	
	// These two methods attempt to convert the given value to a string and return
	// false if this fails. Note that failure to be the given type and exception
	// failure are both returned as false (hence these are slightly sloppy).
	bool convertvaluereftostring(MCValueRef value, MCStringRef& r_string);
	bool convertvaluereftonumber(MCValueRef value, MCNumberRef& r_number);

	// This method attempts to convert the given value to an numeric type. Returning false
	// if this is not possible. Note that this method does not throw, so a false
	// return value really means the value cannot be a number.
	bool convertvaluereftobool(MCValueRef value, bool& r_uint);
	bool convertvaluereftouint(MCValueRef value, uinteger_t& r_uint);
	bool convertvaluereftoint(MCValueRef value, integer_t& r_int);
	bool convertvaluereftoreal(MCValueRef value, real64_t& r_real);

	// This method attempts to convert the given value to a boolean. Returning false
	// if this is not possible. Note that this method does not throw, so a false
	// return value really means the value cannot be a boolean.
	bool convertvaluereftoboolean(MCValueRef value, MCBooleanRef& r_boolean);

	// Array methods.

	// Return the array contained within the ep - isarray() must return true for this
	// to be a valid call.
	MCArrayRef getarrayref(void);
	// Copy the contents of the ep as an array - non arrays convert to empty array.
	bool copyasarrayref(MCArrayRef& r_array);
	// Copy the contents of the ep as a mutable array - non arrays convert to empty array.
	bool copyasmutablearrayref(MCArrayRef& r_array);

	// Set the ep to a string containing the list of keys from the given array.
	bool listarraykeys(MCArrayRef array, char delimiter);
	// Set the ep to the value of the given key in the array.
	bool fetcharrayelement(MCArrayRef array, MCNameRef key);
	// Store the contents of the ep as the given key in the given array.
	bool storearrayelement(MCArrayRef array, MCNameRef key);
	bool appendarrayelement(MCArrayRef array, MCNameRef key);

	// Compatibility methods - these should eventually be phased out.

	bool fetcharrayindex(MCArrayRef array, index_t index);
	bool storearrayindex(MCArrayRef array, index_t index);

	// Set the ep to the value of the given key (as a c-string) in the given array.
	bool fetcharrayelement_cstring(MCArrayRef array, const char *key);
	// Store the contents of the ep as the given key (as a c-string) in the given array.
	bool storearrayelement_cstring(MCArrayRef array, const char *key);
	// Store the contents of the ep as the given key (as a c-string) in the given array.
	bool appendarrayelement_cstring(MCArrayRef array, const char *key);
	// Returns true if the given array has the given key (as a c-string);
	bool hasarrayelement_cstring(MCArrayRef array, const char *key);
	// Set the ep to the value of the given key (as a c-string) in the given array.
	bool fetcharrayelement_oldstring(MCArrayRef array, const MCString& key);
	// Store the contents of the ep as the given key (as a c-string) in the given array.
	bool storearrayelement_oldstring(MCArrayRef array, const MCString& key);
	// Returns true if the given array has the given key (as a c-string);
	bool hasarrayelement_oldstring(MCArrayRef array, const MCString& key);
	bool appendarrayelement_oldstring(MCArrayRef array, const MCString& key);

	// Perform put ep op src into ep.
	Exec_stat factorarray(MCExecPoint& src, Operators op);

private:
	void dounicodetomultibyte(bool p_native, bool p_reverse);

	void concat(uint4 n, Exec_concat, Boolean);
	void concat(int4 n, Exec_concat, Boolean);
	void concat(const MCString &, Exec_concat, Boolean);

	bool converttostring(void);
	bool converttomutablestring(void);
	bool converttonumber(void);
	bool converttoboolean(void);
	bool converttoarray(void);
	bool converttomutablearray(void);
#endif
};

#if 0
inline void MCExecPoint::utf16toutf8(void)
{
	dounicodetomultibyte(false, false);
}

inline void MCExecPoint::utf8toutf16(void)
{
	dounicodetomultibyte(false, true);
}

inline void MCExecPoint::utf16tonative(void)
{
	dounicodetomultibyte(true, false);
}

inline void MCExecPoint::nativetoutf16(void)
{
	dounicodetomultibyte(true, true);
}

inline void MCExecPoint::utf8tonative(void)
{
	utf8toutf16();
	utf16tonative();
}

inline void MCExecPoint::nativetoutf8(void)
{
	nativetoutf16();
	utf16toutf8();
}
#endif

#endif
