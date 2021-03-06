/**
 * @copyright
 * ====================================================================
 * Copyright (c) 2000-2006 CollabNet.  All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at http://subversion.tigris.org/license-1.html.
 * If newer versions of this license are posted there, you may use a
 * newer version instead, at your option.
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://subversion.tigris.org/.
 * ====================================================================
 * @endcopyright
 *
 * @file svn_xml.h
 * @brief XML code shared by various Subversion libraries.
 */



#ifndef SVN_XML_H
#define SVN_XML_H

#include <apr.h>
#include <apr_pools.h>
#include <apr_hash.h>

#include "svn_error.h"
#include "svn_string.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/** The namespace all Subversion XML uses. */
#define SVN_XML_NAMESPACE "svn:"

/** Used as style argument to svn_xml_make_open_tag() and friends. */
enum svn_xml_open_tag_style {
  /** <tag ...> */
  svn_xml_normal = 1,

  /** <tag ...>, no cosmetic newline */
  svn_xml_protect_pcdata,

  /** <tag .../>  */
  svn_xml_self_closing
};
  


/** Determine if a string of character @a data of length @a len is a
 * safe bet for use with the svn_xml_escape_* functions found in this
 * header. 
 * 
 * Return @c TRUE if it is, @c FALSE otherwise.
 *
 * Essentially, this function exists to determine whether or not
 * simply running a string of bytes through the Subversion XML escape
 * routines will produce legitimate XML.  It should only be necessary
 * for data which might contain bytes that cannot be safely encoded
 * into XML (certain control characters, for example).
 */
svn_boolean_t svn_xml_is_xml_safe(const char *data, 
                                  apr_size_t len);

/** Create or append in @a *outstr an xml-escaped version of @a string,
 * suitable for output as character data.
 *
 * If @a *outstr is @c NULL, store a new stringbuf, else append to the
 * existing stringbuf there.
 */
void svn_xml_escape_cdata_stringbuf(svn_stringbuf_t **outstr,
                                    const svn_stringbuf_t *string,
                                    apr_pool_t *pool);

/** Same as svn_xml_escape_cdata_stringbuf(), but @a string is an
 * @c svn_string_t.
 */
void svn_xml_escape_cdata_string(svn_stringbuf_t **outstr,
                                 const svn_string_t *string,
                                 apr_pool_t *pool);

/** Same as svn_xml_escape_cdata_stringbuf(), but @a string is a
 * null-terminated C string.
 */
void svn_xml_escape_cdata_cstring(svn_stringbuf_t **outstr,
                                  const char *string,
                                  apr_pool_t *pool);


/** Create or append in @a *outstr an xml-escaped version of @a string,
 * suitable for output as an attribute value.
 *
 * If @a *outstr is @c NULL, store a new stringbuf, else append to the
 * existing stringbuf there.
 */
void svn_xml_escape_attr_stringbuf(svn_stringbuf_t **outstr,
                                   const svn_stringbuf_t *string,
                                   apr_pool_t *pool);

/** Same as svn_xml_escape_attr_stringbuf(), but @a string is an
 * @c svn_string_t.
 */
void svn_xml_escape_attr_string(svn_stringbuf_t **outstr,
                                const svn_string_t *string,
                                apr_pool_t *pool);

/** Same as svn_xml_escape_attr_stringbuf(), but @a string is a
 * null-terminated C string.
 */
void svn_xml_escape_attr_cstring(svn_stringbuf_t **outstr,
                                 const char *string,
                                 apr_pool_t *pool);

/**
 * Return UTF-8 string @a string if it contains no characters that are
 * unrepresentable in XML.  Else, return a copy of @a string,
 * allocated in @a pool, with each unrepresentable character replaced
 * by "?\uuu", where "uuu" is the three-digit unsigned decimal value
 * of that character.
 *
 * Neither the input nor the output need be valid XML; however, the
 * output can always be safely XML-escaped.
 *
 * @note The current implementation treats all Unicode characters as
 * representable, except for most ASCII control characters (the
 * exceptions being CR, LF, and TAB, which are valid in XML).  There
 * may be other UTF-8 characters that are invalid in XML; see
 * http://subversion.tigris.org/servlets/ReadMsg?list=dev&msgNo=90591
 * and its thread for details.
 *
 * @since New in 1.2.
 */
const char *svn_xml_fuzzy_escape(const char *string,
                                 apr_pool_t *pool);


/*---------------------------------------------------------------*/

/* Generalized Subversion XML Parsing */

/** A generalized Subversion XML parser object */
typedef struct svn_xml_parser_t svn_xml_parser_t;

typedef void (*svn_xml_start_elem)(void *baton,
                                   const char *name,
                                   const char **atts);

typedef void (*svn_xml_end_elem)(void *baton, const char *name);

/* data is not NULL-terminated. */
typedef void (*svn_xml_char_data)(void *baton,
                                  const char *data,
                                  apr_size_t len);


/** Create a general Subversion XML parser */
svn_xml_parser_t *svn_xml_make_parser(void *baton,
                                      svn_xml_start_elem start_handler,
                                      svn_xml_end_elem end_handler,
                                      svn_xml_char_data data_handler,
                                      apr_pool_t *pool);


/** Free a general Subversion XML parser */
void svn_xml_free_parser(svn_xml_parser_t *svn_parser);


/** Push @a len bytes of xml data in @a buf at @a svn_parser.  
 *
 * If this is the final push, @a is_final must be set.  
 *
 * An error will be returned if there was a syntax problem in the XML,
 * or if any of the callbacks set an error using
 * svn_xml_signal_bailout().  
 *
 * If an error is returned, the @c svn_xml_parser_t will have been freed
 * automatically, so the caller should not call svn_xml_free_parser().
 */ 
svn_error_t *svn_xml_parse(svn_xml_parser_t *parser,
                           const char *buf,
                           apr_size_t len,
                           svn_boolean_t is_final);



/** The way to officially bail out of xml parsing.
 *
 * Store @a error in @a svn_parser and set all expat callbacks to @c NULL.
 */
void svn_xml_signal_bailout(svn_error_t *error,
                            svn_xml_parser_t *svn_parser);





/*** Helpers for dealing with the data Expat gives us. ***/

/** Return the value associated with @a name in expat attribute array @a atts,
 * else return @c NULL.
 *
 * (There could never be a @c NULL attribute value in the XML,
 * although the empty string is possible.)
 * 
 * @a atts is an array of c-strings: even-numbered indexes are names,
 * odd-numbers hold values.  If all is right, it should end on an
 * even-numbered index pointing to @c NULL. 
 */
const char *svn_xml_get_attr_value(const char *name, const char **atts);



/* Converting between Expat attribute lists and APR hash tables. */


/** Create an attribute hash from @c va_list @a ap. 
 *
 * The contents of @a ap are alternating <tt>char *</tt> keys and 
 * <tt>char *</tt> vals, terminated by a final @c NULL falling on an 
 * even index (zero-based).
 */
apr_hash_t *svn_xml_ap_to_hash(va_list ap, apr_pool_t *pool);

/** Create a hash that corresponds to Expat xml attribute list @a atts.
 *
 * The hash's keys and values are <tt>char *</tt>'s.
 *
 * @a atts may be null, in which case you just get an empty hash back
 * (this makes life more convenient for some callers).
 */
apr_hash_t *svn_xml_make_att_hash(const char **atts, apr_pool_t *pool);


/** Like svn_xml_make_att_hash(), but takes a hash and preserves any
 * key/value pairs already in it.
 */
void svn_xml_hash_atts_preserving(const char **atts,
                                  apr_hash_t *ht,
                                  apr_pool_t *pool);

/** Like svn_xml_make_att_hash(), but takes a hash and overwrites
 * key/value pairs already in it that also appear in @a atts.
 */
void svn_xml_hash_atts_overlaying(const char **atts,
                                  apr_hash_t *ht,
                                  apr_pool_t *pool);



/* Printing XML */

/** Create an XML header and return it in @a *str.
 *
 * Fully-formed XML documents should start out with a header,
 * something like 
 *         \<?xml version="1.0" encoding="utf-8"?\>
 * 
 * This function returns such a header.  @a *str must either be @c NULL, in
 * which case a new string is created, or it must point to an existing
 * string to be appended to.
 */
void svn_xml_make_header(svn_stringbuf_t **str, apr_pool_t *pool);


/** Store a new xml tag @a tagname in @a *str.
 *
 * If @a str is @c NULL, allocate @a *str in @a pool; else append the new 
 * tag to @a *str, allocating in @a str's pool
 *
 * Take the tag's attributes from varargs, a null-terminated list of
 * alternating <tt>char *</tt> key and <tt>char *</tt> val.  Do xml-escaping 
 * on each val.
 *
 * @a style is one of the enumerated styles in @c svn_xml_open_tag_style.
 */
void svn_xml_make_open_tag(svn_stringbuf_t **str,
                           apr_pool_t *pool,
                           enum svn_xml_open_tag_style style,
                           const char *tagname,
                           ...);


/** Like svn_xml_make_open_tag(), but takes a @c va_list instead of being
 * variadic.
 */
void svn_xml_make_open_tag_v(svn_stringbuf_t **str,
                             apr_pool_t *pool,
                             enum svn_xml_open_tag_style style,
                             const char *tagname,
                             va_list ap);


/** Like svn_xml_make_open_tag(), but takes a hash table of attributes
 * (<tt>char *</tt> keys mapping to <tt>char *</tt> values).
 *
 * You might ask, why not just provide svn_xml_make_tag_atts()?
 *
 * The reason is that a hash table is the most natural interface to an
 * attribute list; the fact that Expat uses <tt>char **</tt> atts instead is
 * certainly a defensible implementation decision, but since we'd have
 * to have special code to support such lists throughout Subversion
 * anyway, we might as well write that code for the natural interface
 * (hashes) and then convert in the few cases where conversion is
 * needed.  Someday it might even be nice to change expat-lite to work
 * with apr hashes.
 *
 * See conversion functions svn_xml_make_att_hash() and
 * svn_xml_make_att_hash_overlaying().  Callers should use those to
 * convert Expat attr lists into hashes when necessary.
 */
void svn_xml_make_open_tag_hash(svn_stringbuf_t **str,
                                apr_pool_t *pool,
                                enum svn_xml_open_tag_style style,
                                const char *tagname,
                                apr_hash_t *attributes);


/** Makes a close tag. */
void svn_xml_make_close_tag(svn_stringbuf_t **str,
                            apr_pool_t *pool,
                            const char *tagname);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SVN_XML_H */
