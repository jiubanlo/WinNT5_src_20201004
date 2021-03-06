//----------------------------------------------------------------------------
// Flexcpp.h -- define interfaces for lexical analyzer classes generated
//                by flex
// Copyright (c) 1993 The Regents of the University of California.
// All rights reserved.
//
// This code is derived from software contributed to Berkeley by
// Kent Williams and Tom Epperly.
//
// Redistribution and use in source and binary forms are permitted provided
// that: (1) source distributions retain this entire copyright notice and
// comment, and (2) distributions including binaries display the following
// acknowledgement:  ``This product includes software developed by the
// University of California, Berkeley and its contributors'' in the
// documentation or other materials provided with the distribution and in
// all advertising materials mentioning features or use of this software.
// Neither the name of the University nor the names of its contributors may
// be used to endorse or promote products derived from this software without
// specific prior written permission.
// THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

// This file defines FlexLexer, an abstract class which specifies the
// external interface provided to flex C++ lexer objects, and YYLEXER,
// which defines a particular lexer class.
//
// If you want to create multiple lexer classes, you use the -P flag
// to rename each YYLEXER to some other xxFlexLexer.  You then
// include <FlexLexer.h> in your other sources once per lexer class:
//
//      #undef YYLEXER
//      #define YYLEXER xxFlexLexer
//      #include <FlexLexer.h>
//
//      #undef YYLEXER
//      #define YYLEXER zzFlexLexer
//      #include <FlexLexer.h>
//      ...
//
// History:    10-Oct-97   emilyb   ported from Monarch SQL code
//
//----------------------------------------------------------------------------

#ifndef __FLEX_LEXER_H
// Never included before - need to define base class.
#define __FLEX_LEXER_H

#ifndef YY_CHAR
#define YY_CHAR WCHAR
#endif

DECLARE_DEBUG(yacc)

#if CIDBG

#define yaccDebugOut(x) yaccInlineDebugOut x
#define yaccAssert(x)   Win4Assert x
#define yaccDebugStr(x) yaccInlineDebugOut x

#else // CIDBG

#define yaccDebugStr(x)
#define yaccDebugOut(x)
#define yaccAssert(x)

#endif // CIDBG

// extern "C++" {

struct yy_buffer_state;
typedef int yy_state_type;

class FlexLexer {
public:
   // virtual ~FlexLexer() = 0;

        const YY_CHAR* YYText() { return yytext; }
        int YYLeng()            { return yyleng; }

        virtual void
                yy_switch_to_buffer( struct yy_buffer_state* new_buffer ) = 0;
        virtual struct yy_buffer_state*
                yy_create_buffer( int size ) = 0;
        virtual void yy_delete_buffer( struct yy_buffer_state* b ) = 0;
        virtual void yyrestart( ) = 0;

        virtual int yylex(YYSTYPE *yylval) = 0;

        int lineno() const              { return yylineno; }

        int debug() const               { return yy_flex_debug; }
        void set_debug( int flag )      { yy_flex_debug = flag; }

protected:
        YY_CHAR* yytext;
        int yyleng;
        int yylineno;           // only maintained if you use %option yylineno
        int yy_flex_debug;      // only has effect with -d or "%option debug"
};

// }
#endif

#if defined(YYLEXER) || ! defined(yyFlexLexerOnce)
// Either this is the first time through (yyFlexLexerOnce not defined),
// or this is a repeated include to define a different flavor of
// YYLEXER, as discussed in the flex man page.
#define yyFlexLexerOnce

class YYPARSER; // forward declaration

class YYLEXER : public FlexLexer {
public:
        // arg_yyin and arg_yyout default to the cin and cout, but we
        // only make that assignment when initializing in yylex().
        YYLEXER();
        virtual ~YYLEXER();

        void yy_switch_to_buffer( struct yy_buffer_state* new_buffer );
        struct yy_buffer_state* yy_create_buffer(  int size );
        void yy_delete_buffer( struct yy_buffer_state* b );
        void yyrestart( );

        int yylex(YYSTYPE *yylval);

        void yyprimebuffer(const YY_CHAR *pszBuffer);
        int yybufferinput(YY_CHAR *pszBuffer, int cMax);
        CCountedDynArray<WCHAR> _allocations;

protected:
        virtual int LexerInput( YY_CHAR* buf, int max_size );
        virtual void LexerOutput( const YY_CHAR* buf, int size );
        virtual void LexerError( const YY_CHAR* msg );
#ifdef DEBUG
        virtual void LexerTrace(const YY_CHAR* buf, int size, int tknNum);
#endif

        void yyunput( int c, YY_CHAR* buf_ptr );
        int yyinput();

        void ResetLexer();
        short CreateTknValue(YYSTYPE *ppStg, short tknNum, BOOL fLong, BOOL fQuote);
        short DetermineTokenType();
        BOOL  IsTokenEmpty();
        BOOL  IsNotOperator();
        void yy_load_buffer_state();
        void yy_init_buffer( struct yy_buffer_state* b );
        void yy_flush_buffer( struct yy_buffer_state* b );

        int yy_start_stack_ptr;
        int yy_start_stack_depth;
        int* yy_start_stack;

        void yy_push_state( int new_state );
        void yy_pop_state();
        int yy_top_state();

        yy_state_type yy_get_previous_state();
        yy_state_type yy_try_NUL_trans( yy_state_type current_state );
        int yy_get_next_buffer();

        //istream* yyin;  // input source for default LexerInput
        //ostream* yyout; // output sink for default LexerOutput

        struct yy_buffer_state* yy_current_buffer;

        // yy_hold_char holds the character lost when yytext is formed.
        YY_CHAR yy_hold_char;

        // Number of characters read into yy_ch_buf.
        int yy_n_chars;

        // Points to current character in buffer.
        YY_CHAR* yy_c_buf_p;
        YY_CHAR *pCurBuffer, *pszInputBuffer;
        int cInputBuffer;

        int yy_init;            // whether we need to initialize
        int yy_start;           // start state number
        int yyprimetoken;       // priming token

        // Flag which is used to allow yywrap()'s to do buffer switches
        // instead of setting up a fresh yyin.  A bit of a hack ...
        int yy_did_buffer_switch_on_eof;

        // The following are not always needed, but may be depending
        // on use of certain flex features (like REJECT or yymore()).

        yy_state_type yy_last_accepting_state;
        YY_CHAR* yy_last_accepting_cpos;

        yy_state_type* yy_state_buf;
        yy_state_type* yy_state_ptr;

        YY_CHAR* yy_full_match;
        int* yy_full_state;
        int yy_full_lp;

        int yy_lp;
        int yy_looking_for_trail_begin;

        int yy_more_flag;
        int yy_more_len;
        int yy_more_offset;
        int yy_prev_more_offset;

        BOOL fContinueImplicitPhrase;
        BOOL fContinueRegex;
        BOOL fContinueMaybeRegex;
        BOOL fTreatFreetextAsPhrase;
#ifdef DEBUG
        int tkndebug;
#endif
};

#endif
